RECURSE( 
    client 
    client/cookies 
    cookies 
    coro 
    examples 
    fetch 
    fetch_gpl
    io 
    io/fuzz 
    io/list_codings 
    misc 
    multipart 
    push_parser 
    server 
    simple 
    static 
) 
 
IF (NOT OS_WINDOWS) 
    RECURSE_FOR_TESTS( 
        io/ut 
        io/ut/medium 
    ) 
ENDIF() 
