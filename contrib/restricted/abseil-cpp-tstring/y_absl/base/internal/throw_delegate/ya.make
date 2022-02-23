# Generated by devtools/yamaker.

LIBRARY(ybase-internal-throw_delegate)

WITHOUT_LICENSE_TEXTS()

OWNER(
    somov
    g:cpp-contrib
)

LICENSE(Apache-2.0)

PEERDIR(
    contrib/restricted/abseil-cpp-tstring/y_absl/base/internal/raw_logging
    contrib/restricted/abseil-cpp-tstring/y_absl/base/log_severity
)

ADDINCL(
    GLOBAL contrib/restricted/abseil-cpp-tstring
)

NO_COMPILER_WARNINGS()

SRCDIR(contrib/restricted/abseil-cpp-tstring/y_absl/base/internal)

SRCS(
    throw_delegate.cc
)

END()
