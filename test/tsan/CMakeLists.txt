# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

if(${ENABLE_SANITIZER})
  return()
endif()

set(TSAN_TEST_C ${CMAKE_CURRENT_BINARY_DIR}/tsan_test.c)

add_custom_command(
  OUTPUT ${TSAN_TEST_C}
  COMMAND $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c.in >
          tsan_test.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(tsan_test-expand DEPENDS ${TSAN_TEST_C})
add_dependencies(expand-tests tsan_test-expand)

# compile command
add_executable(tsan_test ${TSAN_TEST_C})
target_link_libraries(tsan_test PRIVATE dice-tsan.o dice-stacktrace.o)
target_link_libraries(tsan_test PRIVATE dice.h dice pthread)

# target_compile_options(dice PRIVATE -fsanitize=address)
add_test(NAME tsan_test COMMAND ${DICE_SCRIPT} ./tsan_test)
