#include "yql_solomon_dq_integration.h"

#include <ydb/library/yql/ast/yql_expr.h>
#include <ydb/library/yql/dq/expr_nodes/dq_expr_nodes.h>
#include <ydb/library/yql/utils/log/log.h>
#include <ydb/library/yql/providers/common/dq/yql_dq_integration_impl.h>
#include <ydb/library/yql/providers/common/proto/gateways_config.pb.h>
#include <ydb/library/yql/providers/dq/common/yql_dq_settings.h>
#include <ydb/library/yql/providers/dq/expr_nodes/dqs_expr_nodes.h>
#include <ydb/library/yql/providers/solomon/expr_nodes/yql_solomon_expr_nodes.h>
#include <ydb/library/yql/providers/solomon/proto/dq_solomon_shard.pb.h>

#include <util/string/builder.h>

namespace NYql {

using namespace NNodes;

namespace {

constexpr i32 MaxLabelsCount = 16;
constexpr i32 MaxSensorsCount = 50;

NSo::NProto::ESolomonClusterType MapClusterType(TSolomonClusterConfig::ESolomonClusterType clusterType) {
    switch (clusterType) {
        case TSolomonClusterConfig::SCT_SOLOMON:
            return NSo::NProto::ESolomonClusterType::CT_SOLOMON;
        case TSolomonClusterConfig::SCT_MONITORING:
            return NSo::NProto::ESolomonClusterType::CT_MONITORING;
        default:
            YQL_ENSURE(false, "Invalid cluster type " << ToString<ui32>(clusterType));
    }
}

const TTypeAnnotationNode* GetItemType(const TExprNode& node) { 
    const TTypeAnnotationNode* typeAnn = node.GetTypeAnn();
    switch (typeAnn->GetKind()) {
        case ETypeAnnotationKind::Flow:
            return typeAnn->Cast<TFlowExprType>()->GetItemType(); 
        case ETypeAnnotationKind::Stream:
            return typeAnn->Cast<TStreamExprType>()->GetItemType(); 
        default: break;
    }
    YQL_ENSURE(false, "Invalid solomon sink type " << typeAnn->GetKind());
    return nullptr; 
}

void FillScheme(const TTypeAnnotationNode& itemType, NSo::NProto::TDqSolomonShardScheme& scheme) {
    int index = 0;
    for (const TItemExprType* structItem : itemType.Cast<TStructExprType>()->GetItems()) {
        const auto itemName = structItem->GetName();
        const auto* itemType = structItem->GetItemType();
        YQL_ENSURE(
            itemType->GetKind() != ETypeAnnotationKind::Optional,
            "Optional types are not supported in monitoring sink. FieldName: " << itemName);

        const auto dataType = NUdf::GetDataTypeInfo(itemType->Cast<TDataExprType>()->GetSlot());

        NSo::NProto::TDqSolomonSchemeItem schemeItem;
        schemeItem.SetKey(TString(itemName));
        schemeItem.SetIndex(index++);
        schemeItem.SetDataTypeId(dataType.TypeId);

        if (dataType.Features & NUdf::DateType || dataType.Features & NUdf::TzDateType) {
            YQL_ENSURE(!scheme.HasTimestamp(), "Multiple timestamps were provided for monitoing sink");
            *scheme.MutableTimestamp() = schemeItem;
        } else if (dataType.Features & NUdf::NumericType) {
            scheme.MutableSensors()->Add(std::move(schemeItem));
        } else if (dataType.Features & NUdf::StringType) {
            scheme.MutableLabels()->Add(std::move(schemeItem));
        } else {
            YQL_ENSURE(false, "Ivalid data type for monitoing sink: " << dataType.Name);
        }
    }

    YQL_ENSURE(scheme.HasTimestamp(), "Timestamp wasn't provided for monitoing sink");
    YQL_ENSURE(!scheme.GetSensors().empty(), "No sensors were provided for monitoing sink");
    YQL_ENSURE(!scheme.GetLabels().empty(), "No labels were provided for monitoing sink");

    YQL_ENSURE(scheme.GetLabels().size() <= MaxLabelsCount,
        "Max labels count is " << MaxLabelsCount << " but " << scheme.GetLabels().size() << " were provided");
    YQL_ENSURE(scheme.GetSensors().size() <= MaxSensorsCount,
        "Max sensors count is " << MaxSensorsCount << " but " << scheme.GetSensors().size() << " were provided");
}

class TSolomonDqIntegration: public TDqIntegrationBase {
public:
    TSolomonDqIntegration(const TSolomonState::TPtr& state)
        : State_(state.Get())
    {
    }

    TMaybe<ui64> CanRead(const TDqSettings&, const TExprNode&, TExprContext&, bool) override {
        YQL_ENSURE(false, "Unimplemented");
    }

    TExprNode::TPtr WrapRead(const TDqSettings&, const TExprNode::TPtr&, TExprContext&) override {
        YQL_ENSURE(false, "Unimplemented");
    }

    TMaybe<bool> CanWrite(const TDqSettings&, const TExprNode&, TExprContext&) override {
        YQL_ENSURE(false, "Unimplemented");
    }

    void FillSourceSettings(const TExprNode&, ::google::protobuf::Any&, TString& ) override {
        YQL_ENSURE(false, "Unimplemented");
    }

    void FillSinkSettings(const TExprNode& node, ::google::protobuf::Any& protoSettings, TString& sinkType) override {
        const auto maybeDqSink = TMaybeNode<TDqSink>(&node);
        if (!maybeDqSink) {
            return;
        }
        const auto dqSink = maybeDqSink.Cast();

        const auto settings = dqSink.Settings();
        const auto maybeShard = TMaybeNode<TSoShard>(settings.Raw());
        if (!maybeShard) {
            return;
        }

        const TSoShard shard = maybeShard.Cast();

        const auto solomonCluster = shard.SolomonCluster().StringValue();
        const auto* clusterDesc = State_->Configuration->ClusterConfigs.FindPtr(solomonCluster);
        YQL_ENSURE(clusterDesc, "Unknown cluster " << solomonCluster);

        NSo::NProto::TDqSolomonShard shardDesc;
        shardDesc.SetEndpoint(clusterDesc->GetCluster());
        shardDesc.SetProject(shard.Project().StringValue());
        shardDesc.SetCluster(shard.Cluster().StringValue());
        shardDesc.SetService(shard.Service().StringValue());

        shardDesc.SetClusterType(MapClusterType(clusterDesc->GetClusterType()));
        shardDesc.SetUseSsl(clusterDesc->GetUseSsl()); 

        const TTypeAnnotationNode* itemType = GetItemType(node); 
        FillScheme(*itemType, *shardDesc.MutableScheme());

        if (auto maybeToken = shard.Token()) { 
            shardDesc.MutableToken()->SetName(TString(maybeToken.Cast().Name().Value())); 
        }

        protoSettings.PackFrom(shardDesc);
        sinkType = "SolomonSink";
    }

private:
    TSolomonState* State_; // State owns dq integration, so back reference must be not smart.
};

}

THolder<IDqIntegration> CreateSolomonDqIntegration(const TSolomonState::TPtr& state) {
    return MakeHolder<TSolomonDqIntegration>(state);
}

}
