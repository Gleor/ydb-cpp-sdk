# Generated by devtools/yamaker.

LIBRARY()

WITHOUT_LICENSE_TEXTS()

LICENSE(Apache-2.0)

PEERDIR(
    contrib/libs/grpc
    contrib/libs/grpc/src/proto/grpc/reflection/v1alpha
    contrib/libs/grpc/third_party/address_sorting
    contrib/libs/grpc/third_party/upb
    contrib/libs/protobuf
    contrib/restricted/abseil-cpp-tstring/y_absl/synchronization
)

ADDINCL(
    GLOBAL contrib/libs/grpc/include
    ${ARCADIA_BUILD_ROOT}/contrib/libs/grpc
    contrib/libs/grpc
)

NO_COMPILER_WARNINGS()

SRCDIR(contrib/libs/grpc/src)

IF (OS_LINUX OR OS_DARWIN)
    CFLAGS(
        -DGRPC_POSIX_FORK_ALLOW_PTHREAD_ATFORK=1
    )
ENDIF()

SRCS(
    GLOBAL cpp/ext/proto_server_reflection_plugin.cc
    cpp/ext/proto_server_reflection.cc
)

END()