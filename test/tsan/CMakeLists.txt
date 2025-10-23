# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

if(${ENABLE_SANITIZER})
  return()
endif()

add_custom_command(
  OUTPUT tsan_test.c
  COMMAND $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c.in >
          ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c)
add_custom_target(tsan_test-expand DEPENDS tsan_test.c)
add_dependencies(expand-tests tsan_test-expand)

# compile command
add_executable(tsan_test ${CMAKE_CURRENT_SOURCE_DIR}/tsan_test.c)
target_link_libraries(tsan_test PRIVATE dice-tsan_testing.o
                                        dice-stacktrace_testing.o)
target_link_libraries(tsan_test PRIVATE dice.h dice pthread)

# target_compile_options(dice PRIVATE -fsanitize=address)
add_test(NAME tsan_test COMMAND ${DICE_SCRIPT} ./tsan_test)
