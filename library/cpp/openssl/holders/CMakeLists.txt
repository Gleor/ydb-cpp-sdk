add_library(cpp-openssl-holders)

target_link_libraries(cpp-openssl-holders PUBLIC
  yutil
  OpenSSL::SSL
)

target_sources(cpp-openssl-holders PRIVATE
  ${CMAKE_SOURCE_DIR}/library/cpp/openssl/holders/bio.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/openssl/holders/x509_vfy.cpp
)
