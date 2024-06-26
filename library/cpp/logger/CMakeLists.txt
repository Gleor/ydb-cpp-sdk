add_library(library-cpp-logger)
target_link_libraries(library-cpp-logger PUBLIC
  yutil
  tools-enum_parser-enum_serialization_runtime
  library-cpp-json
  cpp-deprecated-atomic
)
target_sources(library-cpp-logger PRIVATE
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/backend.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/backend_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/composite.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/element.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/file.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/filter.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/filter_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/log.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/null.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/rotating_file.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/stream.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/sync_page_cache_file.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/system.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/thread.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/thread_creator.cpp
)
generate_enum_serilization(library-cpp-logger
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/priority.h
  INCLUDE_HEADERS
  library/cpp/logger/priority.h
)

add_global_library_for(library-cpp-logger.global library-cpp-logger)
target_link_libraries(library-cpp-logger.global PUBLIC
  yutil
  tools-enum_parser-enum_serialization_runtime
  library-cpp-json
  cpp-deprecated-atomic
)
target_sources(library-cpp-logger.global PRIVATE
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/composite_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/file_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/null_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/rotating_file_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/stream_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/sync_page_cache_file_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/system_creator.cpp
  ${CMAKE_SOURCE_DIR}/library/cpp/logger/uninitialized_creator.cpp
)


if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT HAVE_CUDA)
  target_link_libraries(library-cpp-logger PUBLIC
    
  )
  target_link_libraries(library-cpp-logger.global PUBLIC
    
  )
endif()
