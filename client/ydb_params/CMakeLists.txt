add_library(cpp-client-ydb_params)

target_link_libraries(cpp-client-ydb_params PUBLIC
  yutil
  api-protos
  client-ydb_types-fatal_error_handlers
  cpp-client-ydb_value
)

target_sources(cpp-client-ydb_params PRIVATE
  ${CMAKE_SOURCE_DIR}/client/ydb_params/params.cpp
  ${CMAKE_SOURCE_DIR}/client/ydb_params/impl.cpp
)

add_library(YDB-CPP-SDK::Params ALIAS cpp-client-ydb_params)
