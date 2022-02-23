# Generated by devtools/yamaker.

LIBRARY(ystrings-internal-cordz_sample_token)

WITHOUT_LICENSE_TEXTS()

OWNER(
    somov
    g:cpp-contrib
)

LICENSE(Apache-2.0)

PEERDIR(
    contrib/restricted/abseil-cpp-tstring/y_absl/base
    contrib/restricted/abseil-cpp-tstring/y_absl/base/internal/low_level_alloc
    contrib/restricted/abseil-cpp-tstring/y_absl/base/internal/raw_logging
    contrib/restricted/abseil-cpp-tstring/y_absl/base/internal/spinlock_wait
    contrib/restricted/abseil-cpp-tstring/y_absl/base/internal/throw_delegate
    contrib/restricted/abseil-cpp-tstring/y_absl/base/log_severity
    contrib/restricted/abseil-cpp-tstring/y_absl/debugging
    contrib/restricted/abseil-cpp-tstring/y_absl/debugging/stacktrace
    contrib/restricted/abseil-cpp-tstring/y_absl/debugging/symbolize
    contrib/restricted/abseil-cpp-tstring/y_absl/demangle
    contrib/restricted/abseil-cpp-tstring/y_absl/numeric
    contrib/restricted/abseil-cpp-tstring/y_absl/profiling/internal/exponential_biased
    contrib/restricted/abseil-cpp-tstring/y_absl/strings
    contrib/restricted/abseil-cpp-tstring/y_absl/strings/internal/absl_cord_internal
    contrib/restricted/abseil-cpp-tstring/y_absl/strings/internal/absl_strings_internal
    contrib/restricted/abseil-cpp-tstring/y_absl/strings/internal/cordz_functions
    contrib/restricted/abseil-cpp-tstring/y_absl/strings/internal/cordz_handle
    contrib/restricted/abseil-cpp-tstring/y_absl/strings/internal/cordz_info
    contrib/restricted/abseil-cpp-tstring/y_absl/synchronization
    contrib/restricted/abseil-cpp-tstring/y_absl/synchronization/internal
    contrib/restricted/abseil-cpp-tstring/y_absl/time
    contrib/restricted/abseil-cpp-tstring/y_absl/time/civil_time
    contrib/restricted/abseil-cpp-tstring/y_absl/time/time_zone
)

ADDINCL(
    GLOBAL contrib/restricted/abseil-cpp-tstring
)

NO_COMPILER_WARNINGS()

SRCDIR(contrib/restricted/abseil-cpp-tstring/y_absl/strings/internal)

SRCS(
    cordz_sample_token.cc
)

END()
