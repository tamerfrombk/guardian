add_executable(guardian ${CMAKE_CURRENT_SOURCE_DIR}/guardian.cpp)
target_link_libraries(guardian PRIVATE libguardian)
add_custom_target(
  run_guardian
  EXLUDE_FROM_ALL
  COMMAND guardian)
add_dependencies(guardian guardian libguardian)
