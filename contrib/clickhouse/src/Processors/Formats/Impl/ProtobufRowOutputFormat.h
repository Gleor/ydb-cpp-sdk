#pragma once

#include "clickhouse_config.h"

#if USE_PROTOBUF
#    include <Processors/Formats/IRowOutputFormat.h>
#   include <Formats/FormatSchemaInfo.h>

namespace DB
{
class DB;
class ProtobufSerializer;
class ProtobufWriter;
class WriteBuffer;
struct FormatSettings;

/** Stream designed to serialize data in the google protobuf format.
  * Each row is written as a separated message.
  *
  * With use_length_delimiters=0 it can write only single row as plain protobuf message,
  * otherwise Protobuf messages are delimited according to documentation
  * https://github.com/protocolbuffers/protobuf/blob/master/src/google/protobuf/util/delimited_message_util.h
  * Serializing in the protobuf format requires the 'format_schema' setting to be set, e.g.
  * SELECT * from table FORMAT Protobuf SETTINGS format_schema = 'schema:Message'
  * where schema is the name of "schema.proto" file specifying protobuf schema.
  */
class ProtobufRowOutputFormat final : public IRowOutputFormat
{
public:
    ProtobufRowOutputFormat(
        WriteBuffer & out_,
        const Block & header_,
        const ProtobufSchemaInfo & schema_info_,
        const FormatSettings & settings_,
        bool with_length_delimiter_);

    String getName() const override { return "ProtobufRowOutputFormat"; }

    std::string getContentType() const override { return "application/octet-stream"; }

private:
    void write(const Columns & columns, size_t row_num) override;
    void writeField(const IColumn &, const ISerialization &, size_t) override {}

    std::unique_ptr<ProtobufWriter> writer;
    std::unique_ptr<ProtobufSerializer> serializer;
    const bool allow_multiple_rows;
};

}
#endif
