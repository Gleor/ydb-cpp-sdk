PY23_TEST() 
 
OWNER(g:python-contrib) 
 
PEERDIR( 
    contrib/python/jmespath 
) 
 
TEST_SRCS( 
    __init__.py 
    test_compliance.py 
    test_parser.py 
) 
 
NO_LINT() 
 
END() 
