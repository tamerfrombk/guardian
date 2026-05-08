cmake_minimum_required(VERSION 3.25)
project(
  guardian
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ExternalProject)
find_package(Git REQUIRED)

function(make_symlink TARGET NAME)
  if(EXISTS "${NAME}")
    if(IS_SYMLINK "${NAME}")
      file(REMOVE "${NAME}")
      message(STATUS "Symlink '${NAME}' removed")
    else()
      message(WARNING "File '${NAME}' exists but is not a symlink")
    endif()
  endif()

  file(CREATE_LINK "${TARGET}" "${NAME}" SYMBOLIC)
  message(STATUS "Symlink created: '${NAME}' => '${TARGET}'")
endfunction()

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(test)

make_symlink("${CMAKE_BINARY_DIR}" "build")
make_symlink("${CMAKE_BINARY_DIR}/compile_commands.json"
             "compile_commands.json")
