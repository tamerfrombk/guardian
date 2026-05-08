# doctest
# https://github.com/doctest/doctest/blob/master/doc/markdown/build-systems.md
ExternalProject_Add(
  doctest
  PREFIX ${CMAKE_BINARY_DIR}/doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  TIMEOUT 10
  UPDATE_COMMAND ${GIT_EXECUTABLE} pull
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR
    ${source_dir}/doctest
    CACHE INTERNAL "Path to include folder for doctest")

function(guardian_test_exe TEST_SOURCE)
  cmake_path(GET TEST_SOURCE STEM NAME)

  add_executable(${NAME} ${TEST_SOURCE})
  target_include_directories(
    ${NAME} PRIVATE ${DOCTEST_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(${NAME} PRIVATE libguardian)
  target_compile_options(
    ${NAME} PRIVATE -Wno-\#warnings -Wno-missing-designated-field-initializers)

  add_test(NAME ${NAME} COMMAND ${NAME})
  add_custom_target(run_${NAME} ALL COMMAND ${NAME})
  add_dependencies(${NAME} ${NAME} doctest)
endfunction()

guardian_test_exe(${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp)
