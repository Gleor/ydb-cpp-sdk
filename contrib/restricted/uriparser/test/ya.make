# Generated by devtools/yamaker.

GTEST(testrunner)

LICENSE(
    LGPL-2.1-only AND
    LGPL-2.1-or-later
)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/restricted/uriparser
)

ADDINCL(
    contrib/restricted/uriparser
)

NO_COMPILER_WARNINGS()

NO_UTIL()

EXPLICIT_DATA()

SRCS(
    FourSuite.cpp
    MemoryManagerSuite.cpp
    VersionSuite.cpp
    test.cpp
)

END()