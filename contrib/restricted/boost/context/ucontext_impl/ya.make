# Generated by devtools/yamaker.

LIBRARY()

WITHOUT_LICENSE_TEXTS()

VERSION(1.83.0)

ORIGINAL_SOURCE(https://github.com/boostorg/context/archive/boost-1.83.0.tar.gz)

LICENSE(BSL-1.0)

PEERDIR(
    contrib/restricted/boost/context/impl_common
    library/cpp/sanitizer/include
)

ADDINCL(
    contrib/restricted/boost/context/include
)

NO_COMPILER_WARNINGS()

NO_UTIL()

CFLAGS(
    GLOBAL -DBOOST_USE_UCONTEXT
    -DBOOST_CONTEXT_SOURCE
)

IF (SANITIZER_TYPE == "address")
    CFLAGS(
        GLOBAL -DBOOST_USE_ASAN
    )
ELSEIF (SANITIZER_TYPE == "thread")
    CFLAGS(
        GLOBAL -DBOOST_USE_TSAN
    )
ENDIF()

SRCDIR(contrib/restricted/boost/context/src)

SRCS(
    continuation.cpp
    fiber.cpp
)

END()
