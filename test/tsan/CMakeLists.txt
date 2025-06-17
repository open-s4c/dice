add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c
  COMMAND $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c.in >
          ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c)

# compile command
add_executable(tsan_test tsan_test.c)
target_link_libraries(tsan_test PRIVATE dice.h dice mod-tsan_testing.o
                                        mod-stacktrace_testing.o pthread)
add_test(NAME tsan_test COMMAND tsan_test)
