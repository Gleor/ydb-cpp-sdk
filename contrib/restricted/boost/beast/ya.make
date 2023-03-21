# Generated by devtools/yamaker from nixpkgs 22.05.

LIBRARY()

LICENSE(
    BSL-1.0 AND
    Zlib
)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

OWNER(
    g:cpp-contrib
    g:taxi-common
)

VERSION(1.67.0)

ORIGINAL_SOURCE(https://github.com/boostorg/beast/archive/boost-1.67.0.tar.gz)

PEERDIR(
    contrib/restricted/boost/asio
    contrib/restricted/boost/assert
    contrib/restricted/boost/bind
    contrib/restricted/boost/config
    contrib/restricted/boost/container
    contrib/restricted/boost/core
    contrib/restricted/boost/endian
    contrib/restricted/boost/intrusive
    contrib/restricted/boost/optional
    contrib/restricted/boost/smart_ptr
    contrib/restricted/boost/static_assert
    contrib/restricted/boost/system
    contrib/restricted/boost/throw_exception
    contrib/restricted/boost/type_traits
    contrib/restricted/boost/utility
    contrib/restricted/boost/winapi
)

ADDINCL(
    GLOBAL contrib/restricted/boost/beast/include
)

END()