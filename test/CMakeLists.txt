# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_compile_options(-DVATOMIC_BUILTINS)
set(TSAN_mp on)
set(TSAN_simple-tsan on)
set(DICE_simple on)
set(DICE_intercept_event on)

file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
  get_filename_component(TEST ${SRC} NAME_WLE)
  set(TARGET ${TEST})
  add_executable(${TARGET} ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice.h)

  if(DICE_${TARGET})
    target_link_libraries(${TARGET} PRIVATE dice)
  endif()

  if(TSAN_${TARGET})
    target_compile_options(${TARGET} PRIVATE -fsanitize=thread)
    target_link_libraries(${TARGET} PRIVATE tsano)
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
      target_compile_options(${TARGET} PRIVATE -shared-libsan)
      target_link_options(${TARGET} PRIVATE -shared-libsan)
    endif()
  endif()
endforeach()

add_executable(simple-dice simple-tsan.c)
target_compile_options(simple-dice PRIVATE -fsanitize=thread)
target_link_libraries(simple-dice PRIVATE dice tsano pthread)

add_test(NAME pthread_create_exit COMMAND pthread_create_exit)
add_test(NAME pthread_create_join COMMAND pthread_create_join)
add_test(NAME pthread_create_detach COMMAND pthread_create_detach)
target_link_libraries(pthread_create_join PRIVATE mod-pthread_create dice)
target_link_libraries(pthread_create_exit PRIVATE mod-pthread_create dice)
target_link_libraries(pthread_create_detach PRIVATE mod-pthread_create dice)

# pthread must be last library to link with tests
foreach(SRC ${SRCS})
  get_filename_component(TARGET ${SRC} NAME_WLE)
  target_link_libraries(${TARGET} PRIVATE pthread)
endforeach()

add_custom_target(expand-tests)

add_subdirectory(interpose)
add_subdirectory(tsan)
