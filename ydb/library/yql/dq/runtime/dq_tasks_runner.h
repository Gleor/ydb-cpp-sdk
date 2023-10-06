#pragma once

#include <ydb/library/yql/dq/common/dq_common.h>
#include <ydb/library/yql/dq/proto/dq_tasks.pb.h>
#include <ydb/library/yql/dq/runtime/dq_async_output.h>
#include <ydb/library/yql/dq/runtime/dq_compute.h>
#include <ydb/library/yql/dq/runtime/dq_input_channel.h>
#include <ydb/library/yql/dq/runtime/dq_input_producer.h>
#include <ydb/library/yql/dq/runtime/dq_output_channel.h>
#include <ydb/library/yql/dq/runtime/dq_output_consumer.h>
#include <ydb/library/yql/dq/runtime/dq_async_input.h>

#include <ydb/library/yql/minikql/computation/mkql_computation_pattern_cache.h>
#include <ydb/library/yql/minikql/mkql_alloc.h>
#include <ydb/library/yql/minikql/mkql_function_registry.h>
#include <ydb/library/yql/minikql/mkql_node_visitor.h>
#include <ydb/library/yql/minikql/mkql_node.h>
#include <ydb/library/yql/minikql/mkql_watermark.h>

#include <library/cpp/monlib/metrics/histogram_collector.h>

#include <util/generic/size_literals.h>
#include <util/system/types.h>

namespace NYql::NDq {

enum class ERunStatus : ui32 {
    Finished,
    PendingInput,
    PendingOutput
};

class TRunStatusTimeMetrics {
public:
    void UpdateStatusTime(TDuration computeCpuTime = TDuration::Zero()) {
        auto now = TInstant::Now();
        StatusTime[ui32(CurrentStatus)] += now - StatusStartTime - computeCpuTime;
        StatusStartTime = now;
    }

    void SetCurrentStatus(ERunStatus status, TDuration computeCpuTime) {
        Y_VERIFY(ui32(status) < StatusesCount);
        UpdateStatusTime(computeCpuTime);
        CurrentStatus = status;
    }

    TDuration operator[](ERunStatus status) const {
        const ui32 index = ui32(status);
        Y_VERIFY(index < StatusesCount);
        return StatusTime[index];
    }

    void Load(ERunStatus status, TDuration d) {
        const ui32 index = ui32(status);
        Y_VERIFY(index < StatusesCount);
        StatusTime[index] = d;
    }

    static constexpr ui32 StatusesCount = 3;

private:
    TInstant StatusStartTime = TInstant::Now();
    ERunStatus CurrentStatus = ERunStatus::PendingInput;
    TDuration StatusTime[StatusesCount];
};

struct TMkqlStat {
    NKikimr::NMiniKQL::TStatKey Key;
    i64 Value = 0;
};

struct TTaskRunnerStatsBase {
    // basic stats
    TDuration BuildCpuTime;
    TInstant FinishTs;
    TInstant StartTs;

    TDuration ComputeCpuTime;
    TRunStatusTimeMetrics RunStatusTimeMetrics; // ComputeCpuTime + RunStatusTimeMetrics == 100% time

    // profile stats
    TDuration WaitTime; // wall time of waiting for input, scans & output
    TDuration WaitOutputTime;

    NMonitoring::IHistogramCollectorPtr ComputeCpuTimeByRun; // in millis

    THashMap<ui64, const TDqInputChannelStats*> InputChannels; // Channel id -> Channel stats
    THashMap<ui64, const TDqAsyncInputBufferStats*> Sources; // Input index -> Source stats
    THashMap<ui64, const TDqOutputChannelStats*> OutputChannels; // Channel id -> Channel stats

    TVector<TMkqlStat> MkqlStats;

    TTaskRunnerStatsBase() = default;
    TTaskRunnerStatsBase(TTaskRunnerStatsBase&&) = default;
    TTaskRunnerStatsBase& operator=(TTaskRunnerStatsBase&&) = default;

    virtual ~TTaskRunnerStatsBase() = default;

    template<typename T>
    void FromProto(const T& f)
    {
        this->BuildCpuTime = TDuration::MicroSeconds(f.GetBuildCpuTimeUs());
        this->ComputeCpuTime = TDuration::MicroSeconds(f.GetComputeCpuTimeUs());
        this->RunStatusTimeMetrics.Load(ERunStatus::PendingInput, TDuration::MicroSeconds(f.GetPendingInputTimeUs()));
        this->RunStatusTimeMetrics.Load(ERunStatus::PendingOutput, TDuration::MicroSeconds(f.GetPendingOutputTimeUs()));
        this->RunStatusTimeMetrics.Load(ERunStatus::Finished, TDuration::MicroSeconds(f.GetFinishTimeUs()));
        //s->TotalTime = TDuration::MilliSeconds(f.GetTotalTime());
        this->WaitTime = TDuration::MicroSeconds(f.GetWaitTimeUs());
        this->WaitOutputTime = TDuration::MicroSeconds(f.GetWaitOutputTimeUs());

        //s->MkqlTotalNodes = f.GetMkqlTotalNodes();
        //s->MkqlCodegenFunctions = f.GetMkqlCodegenFunctions();
        //s->CodeGenTotalInstructions = f.GetCodeGenTotalInstructions();
        //s->CodeGenTotalFunctions = f.GetCodeGenTotalFunctions();
        //s->CodeGenFullTime = f.GetCodeGenFullTime();
        //s->CodeGenFinalizeTime = f.GetCodeGenFinalizeTime();
        //s->CodeGenModulePassTime = f.GetCodeGenModulePassTime();

        for (const auto& input : f.GetInputChannels()) {
            this->MutableInputChannel(input.GetChannelId())->FromProto(input);
        }

        for (const auto& output : f.GetOutputChannels()) {
            this->MutableOutputChannel(output.GetChannelId())->FromProto(output);
        }

        // todo: (whcrc) fill sources and ComputeCpuTimeByRun?
    }

private:
    virtual TDqInputChannelStats* MutableInputChannel(ui64 channelId) = 0;
    virtual TDqAsyncInputBufferStats* MutableSource(ui64 sourceId) = 0;  // todo: (whcrc) unused, not modified by these pointers
    virtual TDqOutputChannelStats* MutableOutputChannel(ui64 channelId) = 0;
};

struct TDqTaskRunnerStats : public TTaskRunnerStatsBase {
    // these stats are owned by TDqTaskRunner
    TDqInputChannelStats* MutableInputChannel(ui64 channelId) override {
        return const_cast<TDqInputChannelStats*>(InputChannels[channelId]);
    }

    TDqAsyncInputBufferStats* MutableSource(ui64 sourceId) override {
        return const_cast<TDqAsyncInputBufferStats*>(Sources[sourceId]);
    }

    TDqOutputChannelStats* MutableOutputChannel(ui64 channelId) override {
        return const_cast<TDqOutputChannelStats*>(OutputChannels[channelId]);
    }
};

// Provides read access to TTaskRunnerStatsBase
// May or may not own the underlying object
class TDqTaskRunnerStatsView {
public:
    TDqTaskRunnerStatsView() : IsDefined(false) {}

    TDqTaskRunnerStatsView(const TDqTaskRunnerStats* stats)   // used in TLocalTaskRunnerActor, cause it holds this stats, and does not modify it asyncronously from TDqAsyncComputeActor
        : StatsPtr(stats)
        , IsDefined(true) {
    }

    TDqTaskRunnerStatsView(const TDqTaskRunnerStats* stats, THashMap<ui32, const TDqAsyncOutputBufferStats*>&& sinkStats,
        THashMap<ui32, const TDqAsyncInputBufferStats*>&& inputTransformStats)
        : StatsPtr(stats)
        , IsDefined(true)
        , SinkStats(std::move(sinkStats))
        , InputTransformStats(std::move(inputTransformStats)) {
    }

    const TTaskRunnerStatsBase* Get() {
        if (!IsDefined) {
            return nullptr;
        }
        return StatsPtr;
    }

    operator bool() const {
        return IsDefined;
    }

    const TDqAsyncOutputBufferStats* GetSinkStats(ui32 sinkId) const {
        return SinkStats.at(sinkId);
    }

    const TDqAsyncInputBufferStats* GetInputTransformStats(ui32 inputTransformId) const {
        return InputTransformStats.at(inputTransformId);
    }

private:
    const TDqTaskRunnerStats* StatsPtr;
    bool IsDefined;
    THashMap<ui32, const TDqAsyncOutputBufferStats*> SinkStats;
    THashMap<ui32, const TDqAsyncInputBufferStats*> InputTransformStats;
};

struct TDqTaskRunnerContext {
    const NKikimr::NMiniKQL::IFunctionRegistry* FuncRegistry = nullptr;
    IRandomProvider* RandomProvider = nullptr;
    ITimeProvider* TimeProvider = nullptr;
    TDqComputeContextBase* ComputeCtx = nullptr;
    NKikimr::NMiniKQL::TComputationNodeFactory ComputationFactory;
    NUdf::IApplyContext* ApplyCtx = nullptr;
    NKikimr::NMiniKQL::TCallableVisitFuncProvider FuncProvider;
    NKikimr::NMiniKQL::TScopedAlloc* Alloc = nullptr;
    NKikimr::NMiniKQL::TTypeEnvironment* TypeEnv = nullptr;
    std::shared_ptr<NKikimr::NMiniKQL::TComputationPatternLRUCache> PatternCache;
};

class IDqTaskRunnerExecutionContext {
public:
    virtual ~IDqTaskRunnerExecutionContext() = default;

    virtual IDqOutputConsumer::TPtr CreateOutputConsumer(const NDqProto::TTaskOutput& outputDesc,
        const NKikimr::NMiniKQL::TType* type, NUdf::IApplyContext* applyCtx,
        const NKikimr::NMiniKQL::TTypeEnvironment& typeEnv,
        const NKikimr::NMiniKQL::THolderFactory& holderFactory,
        TVector<IDqOutput::TPtr>&& outputs) const = 0;

    virtual IDqChannelStorage::TPtr CreateChannelStorage(ui64 channelId) const = 0;
};

class TDqTaskRunnerExecutionContext : public IDqTaskRunnerExecutionContext {
public:
    IDqOutputConsumer::TPtr CreateOutputConsumer(const NDqProto::TTaskOutput& outputDesc,
        const NKikimr::NMiniKQL::TType* type, NUdf::IApplyContext* applyCtx,
        const NKikimr::NMiniKQL::TTypeEnvironment& typeEnv,
        const NKikimr::NMiniKQL::THolderFactory& holderFactory,
        TVector<IDqOutput::TPtr>&& outputs) const override;

    IDqChannelStorage::TPtr CreateChannelStorage(ui64 channelId) const override;
};

struct TDqTaskRunnerSettings {
    bool CollectBasicStats = false;
    bool CollectProfileStats = false;
    bool TerminateOnError = false;
    bool UseCacheForLLVM = false;
    TString OptLLVM = "";
    THashMap<TString, TString> SecureParams;
    THashMap<TString, TString> TaskParams;
    TVector<TString> ReadRanges;
};

struct TDqTaskRunnerMemoryLimits {
    ui32 ChannelBufferSize = 0;
    ui32 OutputChunkMaxSize = 0;
    ui32 ChunkSizeLimit = 48_MB;
};

NUdf::TUnboxedValue DqBuildInputValue(const NDqProto::TTaskInput& inputDesc, const NKikimr::NMiniKQL::TType* type,
    TVector<IDqInputChannel::TPtr>&& channels, const NKikimr::NMiniKQL::THolderFactory& holderFactory);

IDqOutputConsumer::TPtr DqBuildOutputConsumer(const NDqProto::TTaskOutput& outputDesc, const NKikimr::NMiniKQL::TType* type,
    const NKikimr::NMiniKQL::TTypeEnvironment& typeEnv, const NKikimr::NMiniKQL::THolderFactory& holderFactory,
    TVector<IDqOutput::TPtr>&& channels);

using TDqTaskRunnerParameterProvider = std::function<
    bool(std::string_view name, NKikimr::NMiniKQL::TType* type, const NKikimr::NMiniKQL::TTypeEnvironment& typeEnv,
         const NKikimr::NMiniKQL::THolderFactory& holderFactory, NUdf::TUnboxedValue& value)
>;


/// TDqTaskSettings class that holds all the settings of the DqTask.
/// It accepts pointer and accepts ownership.
class TDqTaskSettings {
public:
    explicit TDqTaskSettings(NDqProto::TDqTask* task, TIntrusivePtr<NActors::TProtoArenaHolder> arena = nullptr)
        : Task_(nullptr)
        , Arena(std::move(arena))
    {
        if (!task->GetArena()) {
            HeapTask_ = std::make_unique<NDqProto::TDqTask>();
            HeapTask_->Swap(task);
            Task_ = HeapTask_.get();
            Y_VERIFY(!Arena);
        } else {
            Task_ = task;
            Y_VERIFY(Arena);
            Y_VERIFY(task->GetArena() == Arena->Get());
        }
    }

    TDqTaskSettings(const TDqTaskSettings& task) {
        if (Y_LIKELY(task.HeapTask_)) {
            HeapTask_ = std::make_unique<NDqProto::TDqTask>();
            HeapTask_->CopyFrom(*task.Task_);
            Task_ = HeapTask_.get();
            Y_VERIFY(!task.Arena);
        } else {
            Y_FAIL("not allowed to copy dq settings for arena allocated messages.");
        }
    }

    ui64 GetId() const {
        return Task_->GetId();
    }

    bool GetCreateSuspended() const {
        return Task_->GetCreateSuspended();
    }

    const NDqProto::TDqTask& GetSerializedTask() const {
        Y_VERIFY(!ParamProvider, "GetSerialized isn't supported if external ParamProvider callback is specified!");
        return *Task_;
    }

    const ::NYql::NDqProto::TTaskInput& GetInputs(size_t index) const {
        return Task_->GetInputs(index);
    }

    const ::NYql::NDqProto::TTaskOutput& GetOutputs(size_t index) const {
        return Task_->GetOutputs(index);
    }

    const ::google::protobuf::RepeatedPtrField<::NYql::NDqProto::TTaskInput>& GetInputs() const {
        return Task_->GetInputs();
    }

    size_t InputsSize() const {
        return Task_->InputsSize();
    }

    size_t OutputsSize() const {
        return Task_->OutputsSize();
    }

    void SetParamsProvider(TDqTaskRunnerParameterProvider&& provider) {
        ParamProvider = std::move(provider);
    }

    void GetParameterValue(std::string_view name, NKikimr::NMiniKQL::TType* type, const NKikimr::NMiniKQL::TTypeEnvironment& typeEnv,
        const NKikimr::NMiniKQL::THolderFactory& holderFactory, NUdf::TUnboxedValue& value) const
    {
        if (ParamProvider && ParamProvider(name, type, typeEnv, holderFactory, value)) {
#ifndef NDEBUG
            YQL_ENSURE(!Task_->GetParameters().contains(name), "param: " << name);
#endif
        } else {
            auto it = Task_->GetParameters().find(name);
            YQL_ENSURE(it != Task_->GetParameters().end());

            auto guard = typeEnv.BindAllocator();
            TDqDataSerializer::DeserializeParam(it->second, type, holderFactory, value);
        }
    }

    ui64 GetStageId() const {
        return Task_->GetStageId();
    }

    const ::NYql::NDqProto::TProgram& GetProgram() const {
        return Task_->GetProgram();
    }

    const TProtoStringType & GetRateLimiterResource() const {
        return Task_->GetRateLimiterResource();
    }

    const TProtoStringType& GetRateLimiter() const {
        return Task_->GetRateLimiter();
    }

    const ::google::protobuf::Map<TProtoStringType, ::NYql::NDqProto::TData>& GetParameters() const {
        return Task_->GetParameters();
    }

    const ::google::protobuf::Map<TProtoStringType, TProtoStringType>& GetTaskParams() const {
        return Task_->GetTaskParams();
    }

    const ::google::protobuf::RepeatedPtrField<TString>& GetReadRanges() const {
        return Task_->GetReadRanges();
    }

    const ::google::protobuf::Map<TProtoStringType, TProtoStringType>& GetSecureParams() const {
        return Task_->GetSecureParams();
    }

    const ::google::protobuf::RepeatedPtrField<::NYql::NDqProto::TTaskOutput>& GetOutputs() const {
        return Task_->GetOutputs();
    }

    const ::google::protobuf::Any& GetMeta() const {
        return Task_->GetMeta();
    }

    bool GetUseLlvm() const {
        return Task_->GetUseLlvm();
    }

    bool HasUseLlvm() const {
        return Task_->HasUseLlvm();
    }

    const TVector<google::protobuf::Message*>& GetSourceSettings() const {
        return SourceSettings;
    }

    TVector<google::protobuf::Message*>& MutableSourceSettings() {
        return SourceSettings;
    }

    const TIntrusivePtr<NActors::TProtoArenaHolder>& GetArena() const {
        return Arena;
    }

    const google::protobuf::Map<TProtoStringType, TProtoStringType>& GetRequestContext() const {
        return Task_->GetRequestContext();
    }

private:

    // external callback to retrieve parameter value.
    TDqTaskRunnerParameterProvider ParamProvider;
    NDqProto::TDqTask* Task_ = nullptr;
    std::unique_ptr<NDqProto::TDqTask> HeapTask_;
    TIntrusivePtr<NActors::TProtoArenaHolder> Arena;
    TVector<google::protobuf::Message*> SourceSettings;  // used only in case if we execute compute actor locally
};

class IDqTaskRunner : public TSimpleRefCount<IDqTaskRunner>, private TNonCopyable {
public:
    virtual ~IDqTaskRunner() = default;

    virtual ui64 GetTaskId() const = 0;

    virtual void Prepare(const TDqTaskSettings& task, const TDqTaskRunnerMemoryLimits& memoryLimits,
        const IDqTaskRunnerExecutionContext& execCtx = TDqTaskRunnerExecutionContext()) = 0;
    virtual ERunStatus Run() = 0;

    virtual bool HasEffects() const = 0;

    virtual IDqInputChannel::TPtr GetInputChannel(ui64 channelId) = 0;
    virtual IDqAsyncInputBuffer::TPtr GetSource(ui64 inputIndex) = 0;
    virtual IDqOutputChannel::TPtr GetOutputChannel(ui64 channelId) = 0;
    virtual IDqAsyncOutputBuffer::TPtr GetSink(ui64 outputIndex) = 0;
    virtual std::pair<NUdf::TUnboxedValue, IDqAsyncInputBuffer::TPtr> GetInputTransform(ui64 inputIndex) = 0;
    virtual std::pair<IDqAsyncOutputBuffer::TPtr, IDqOutputConsumer::TPtr> GetOutputTransform(ui64 outputIndex) = 0;

    virtual IRandomProvider* GetRandomProvider() const = 0;

    // if memoryLimit = Nothing()  then don't set memory limit, use existing one (if any)
    // if memoryLimit = 0          then set unlimited
    // otherwise use particular memory limit
    virtual TGuard<NKikimr::NMiniKQL::TScopedAlloc> BindAllocator(TMaybe<ui64> memoryLimit = Nothing()) = 0;
    virtual bool IsAllocatorAttached() = 0;
    virtual const NKikimr::NMiniKQL::TTypeEnvironment& GetTypeEnv() const = 0;
    virtual const NKikimr::NMiniKQL::THolderFactory& GetHolderFactory() const = 0;
    virtual std::shared_ptr<NKikimr::NMiniKQL::TScopedAlloc> GetAllocatorPtr() const = 0;

    virtual const THashMap<TString, TString>& GetSecureParams() const = 0;
    virtual const THashMap<TString, TString>& GetTaskParams() const = 0;
    virtual const TVector<TString>& GetReadRanges() const = 0;

    virtual void UpdateStats() = 0;
    virtual const TDqTaskRunnerStats* GetStats() const = 0;
    virtual const TDqMeteringStats* GetMeteringStats() const = 0;

    [[nodiscard]]
    virtual TString Save() const = 0;
    virtual void Load(TStringBuf in) = 0;

    virtual void SetWatermarkIn(TInstant time) = 0;
    virtual const NKikimr::NMiniKQL::TWatermark& GetWatermark() const = 0;
};

TIntrusivePtr<IDqTaskRunner> MakeDqTaskRunner(const TDqTaskRunnerContext& ctx, const TDqTaskRunnerSettings& settings,
    const TLogFunc& logFunc);

} // namespace NYql::NDq

template <>
inline void Out<NYql::NDq::TTaskRunnerStatsBase>(IOutputStream& os, TTypeTraits<NYql::NDq::TTaskRunnerStatsBase>::TFuncParam stats) {
    os << "TTaskRunnerStatsBase:" << Endl
       << "\tBuildCpuTime: " << stats.BuildCpuTime << Endl
       << "\tStartTs: " << stats.StartTs << Endl
       << "\tFinishTs: " << stats.FinishTs << Endl
       << "\tComputeCpuTime: " << stats.ComputeCpuTime << Endl
       << "\tWaitTime: " << stats.WaitTime << Endl
       << "\tWaitOutputTime: " << stats.WaitOutputTime << Endl
       << "\tsize of InputChannels: " << stats.InputChannels.size() << Endl
       << "\tsize of Sources: " << stats.Sources.size() << Endl
       << "\tsize of OutputChannels: " << stats.OutputChannels.size();
}
