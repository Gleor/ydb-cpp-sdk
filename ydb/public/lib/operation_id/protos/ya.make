PROTO_LIBRARY() 
 
OWNER(g:kikimr)
 
IF (OS_WINDOWS) 
    NO_OPTIMIZE_PY_PROTOS() 
ENDIF() 
 
SRCS( 
    operation_id.proto 
) 
 
EXCLUDE_TAGS(GO_PROTO)

END() 
