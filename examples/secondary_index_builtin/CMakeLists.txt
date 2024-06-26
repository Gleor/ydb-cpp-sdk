add_executable(secondary_index_builtin)

target_link_libraries(secondary_index_builtin PUBLIC
  yutil
  library-cpp-getopt
  YDB-CPP-SDK::Table
)

target_sources(secondary_index_builtin PRIVATE
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/main.cpp
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/secondary_index.cpp
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/secondary_index_create.cpp
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/secondary_index_fill.cpp
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/secondary_index_select.cpp
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/secondary_index_drop.cpp
  ${CMAKE_SOURCE_DIR}/examples/secondary_index_builtin/secondary_index_select_join.cpp
)

vcs_info(secondary_index_builtin)

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  target_link_libraries(secondary_index_builtin PUBLIC
    library-cpp-cpuid_check
  )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(secondary_index_builtin PRIVATE
    -ldl
    -lrt
    -Wl,--no-as-needed
    -lpthread
  )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_link_options(secondary_index_builtin PRIVATE
    -Wl,-platform_version,macos,11.0,11.0
    -framework
    CoreFoundation
  )
endif()
