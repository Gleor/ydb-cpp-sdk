PROGRAM()

SRCS(
    main.proto
    main.cpp
)

PEERDIR(
    ydb/library/yql/public/purecalc
    ydb/library/yql/public/purecalc/io_specs/protobuf
    ydb/library/yql/public/purecalc/helpers/stream
)


   YQL_LAST_ABI_VERSION()


END()

RECURSE_ROOT_RELATIVE(
    yql/udfs/common/url
    yql/udfs/common/ip
)

RECURSE_FOR_TESTS(
    ut
)
