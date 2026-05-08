file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp")

add_library(libguardian INTERFACE)
target_sources(
  libguardian INTERFACE FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                        FILES ${HEADER_FILES})
target_compile_options(libguardian INTERFACE -Wall -Wextra -Wpedantic)
target_include_directories(libguardian INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

foreach(HEADER_FILE ${HEADER_FILES})
  message(STATUS "Found header file: ${HEADER_FILE}")
  set_source_files_properties(${HEADER_FILE} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

set_target_properties(libguardian PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)
