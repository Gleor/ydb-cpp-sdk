PROTO_LIBRARY()

OWNER(g:messagebus)

PEERDIR(
    library/cpp/monlib/encode/legacy_protobuf/protos 
)

SRCS(
    mon_proto.proto
)

EXCLUDE_TAGS(GO_PROTO)

END()
