add_library(internal-proxies-ssse3)

target_link_libraries(internal-proxies-ssse3 PUBLIC
  internal-proxies-macro
  argonish-internal-argon2
  argonish-internal-blake2b
)

target_sources_custom(internal-proxies-ssse3
  .ssse3
  SRCS
  ${CMAKE_SOURCE_DIR}/library/cpp/digest/argonish/internal/proxies/ssse3/proxy_ssse3.cpp
  CUSTOM_FLAGS
  -mssse3
)
