LIBRARY()

LICENSE(
    Apache-2.0 AND
    Apache-2.0 WITH LLVM-exception AND
    BSD-2-Clause AND
    MIT AND
    NCSA
)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

VERSION(2021-04-02-7959d59028dd126416cdf10dbbd22162922e1336) 
 
OWNER(
    halyavin
    somov
    g:cpp-committee
    g:cpp-contrib
)

ADDINCL(
    GLOBAL contrib/libs/cxxsupp/libcxx/include
)

CXXFLAGS(-D_LIBCPP_BUILDING_LIBRARY)

IF (OS_ANDROID)
    DEFAULT(CXX_RT "default")
    IF (ARCH_I686 OR ARCH_ARM7)
        # 32-bit architectures require additional libandroid_support.so to be linked
        # We add --start-group / --end-group statements due to the issue in NDK < r22.
        # See: https://github.com/android/ndk/issues/1130
        #
        # Though these statements are not respected by LLD, they might have sense for other linkers.
        LDFLAGS(
            -Wl,--start-group
            -lc++abi
            -landroid_support
            -Wl,--end-group
        )
    ELSE()
        LDFLAGS(-lc++abi)
    ENDIF()
    CFLAGS(
        -DLIBCXX_BUILDING_LIBCXXABI
    )
ELSEIF (OS_IOS)
    # Take cxxabi implementation from system.
    LDFLAGS(-lc++abi)
    CFLAGS(
        -DLIBCXX_BUILDING_LIBCXXABI
    )
    # Yet take builtins library from Arcadia
    PEERDIR(
        contrib/libs/cxxsupp/builtins
    )
ELSEIF (CLANG OR MUSL OR OS_DARWIN OR USE_LTO)
    IF (ARCH_ARM7)
        # XXX: libcxxrt support for ARM is currently broken
        DEFAULT(CXX_RT "glibcxx_static")
        # ARM7 OS_SDK has old libstdc++ without aligned allocation support
        CFLAGS(
            GLOBAL -fno-aligned-new
        )
    ELSE()
        DEFAULT(CXX_RT "libcxxrt")
    ENDIF()
    IF (MUSL)
        PEERDIR(
            contrib/libs/musl/include
        )
    ENDIF()
ELSEIF (OS_WINDOWS)
    SRCS(
        src/support/win32/locale_win32.cpp
        src/support/win32/support.cpp
        src/support/win32/atomic_win32.cpp
        src/support/win32/new_win32.cpp
        src/support/win32/thread_win32.cpp
    )
    CFLAGS(
        GLOBAL -D_LIBCPP_VASPRINTF_DEFINED
        GLOBAL -D_WCHAR_H_CPLUSPLUS_98_CONFORMANCE_
    )
    IF (CLANG_CL)
        PEERDIR(
            contrib/libs/cxxsupp/builtins
        )
    ENDIF()
ELSE()
    DEFAULT(CXX_RT "glibcxx_static")
    CXXFLAGS(
        -Wno-unknown-pragmas
        -nostdinc++
    )
ENDIF()

IF (OS_LINUX)
    EXTRALIBS(-lpthread)
ENDIF()

IF (CLANG)
    CFLAGS(
        GLOBAL -nostdinc++
    )
ENDIF()

# The CXX_RT variable controls which C++ runtime is used.
# * libcxxrt        - https://github.com/pathscale/libcxxrt library stored in Arcadia
# * glibcxx         - GNU C++ Library runtime with default (static) linkage
# * glibcxx_static  - GNU C++ Library runtime with static linkage
# * glibcxx_dynamic - GNU C++ Library runtime with dynamic linkage
# * glibcxx_driver  - GNU C++ Library runtime provided by the compiler driver
# * default         - default C++ runtime provided by the compiler driver
#
# All glibcxx* runtimes are taken from system/compiler SDK

DEFAULT(CXX_RT "default")

DISABLE(NEED_GLIBCXX_CXX17_SHIMS)

IF (CXX_RT == "libcxxrt")
    PEERDIR(
        contrib/libs/cxxsupp/libcxxabi-parts
        contrib/libs/cxxsupp/libcxxrt
        contrib/libs/cxxsupp/builtins
    )
    ADDINCL(
        contrib/libs/cxxsupp/libcxxrt
    )
    CFLAGS(
        GLOBAL -DLIBCXX_BUILDING_LIBCXXRT
    )
    # These builtins are equivalent to clang -rtlib=compiler_rt and
    # are needed by potentially any code generated by clang.
    # With glibcxx runtime, builtins are provided by libgcc
ELSEIF (CXX_RT == "glibcxx" OR CXX_RT == "glibcxx_static")
    LDFLAGS(
        -Wl,-Bstatic
        -lsupc++
        -lgcc
        -lgcc_eh
        -Wl,-Bdynamic
    )
    CXXFLAGS(-D__GLIBCXX__=1)
    ENABLE(NEED_GLIBCXX_CXX17_SHIMS)
    CFLAGS(
        GLOBAL -DLIBCXX_BUILDING_LIBGCC
    )
ELSEIF (CXX_RT == "glibcxx_dynamic")
    LDFLAGS(
        -lgcc_s
        -lstdc++
    )
    CXXFLAGS(-D__GLIBCXX__=1)
    CFLAGS(
        GLOBAL -DLIBCXX_BUILDING_LIBGCC
    )
    ENABLE(NEED_GLIBCXX_CXX17_SHIMS)
ELSEIF (CXX_RT == "glibcxx_driver")
    CXXFLAGS(-D__GLIBCXX__=1)
ELSEIF (CXX_RT == "default")
    # Do nothing
ELSE()
    MESSAGE(FATAL_ERROR "Unexpected CXX_RT value: ${CXX_RT}")
ENDIF()

IF (NEED_GLIBCXX_CXX17_SHIMS)
    IF (GCC)
        # Assume GCC is bundled with a modern enough version of C++ runtime
    ELSEIF (OS_SDK == "ubuntu-12" OR OS_SDK == "ubuntu-14" OR OS_SDK == "ubuntu-16")
        # Prior to ubuntu-18, system C++ runtime for C++17 is incomplete
        SRCS(
            glibcxx_eh_cxx17.cpp
        )
    ENDIF()
ENDIF()

NO_UTIL()

NO_RUNTIME()

NO_COMPILER_WARNINGS()

IF (FUZZING)
    NO_SANITIZE()
    NO_SANITIZE_COVERAGE()
ENDIF()

SRCS(
    src/algorithm.cpp
    src/any.cpp
    src/atomic.cpp
    src/barrier.cpp
    src/bind.cpp
    src/charconv.cpp
    src/chrono.cpp
    src/condition_variable.cpp
    src/condition_variable_destructor.cpp
    src/debug.cpp
    src/exception.cpp
    src/filesystem/directory_iterator.cpp
    src/filesystem/operations.cpp
    src/format.cpp
    src/functional.cpp
    src/future.cpp
    src/hash.cpp
    src/ios.cpp
    src/ios.instantiations.cpp
    src/iostream.cpp
    src/locale.cpp
    src/memory.cpp
    src/mutex.cpp
    src/mutex_destructor.cpp
    src/optional.cpp
    src/random.cpp
    src/random_shuffle.cpp
    src/regex.cpp
    src/shared_mutex.cpp
    src/stdexcept.cpp
    src/string.cpp
    src/strstream.cpp
    src/system_error.cpp
    src/thread.cpp
    src/typeinfo.cpp
    src/utility.cpp
    src/valarray.cpp
    src/variant.cpp
    src/vector.cpp
)

IF (NOT OS_WINDOWS)
    SRCS(
        src/new.cpp
    )
ENDIF()

END()
