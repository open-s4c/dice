# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_compile_options(-DVATOMIC_BUILTINS)
set(TSAN_mp on)
set(TSAN_simple-tsan on)
set(DICE_simple on)
set(DICE_capture_event on)

file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
  get_filename_component(TEST ${SRC} NAME_WLE)
  set(TARGET ${TEST})
  add_executable(${TARGET} ${SRC})
  target_link_libraries(${TARGET} PRIVATE pthread)

  if(DICE_${TARGET})
    target_link_libraries(${TARGET} PRIVATE dice)
  endif()

  if(TSAN_${TARGET})
    target_compile_options(${TARGET} PRIVATE -fsanitize=thread)
    target_link_options(${TARGET} PRIVATE -fsanitize=thread)
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
      target_compile_options(${TARGET} PRIVATE -shared-libsan)
      target_link_options(${TARGET} PRIVATE -shared-libsan)
    endif()
  endif()
endforeach()

add_executable(simple-dice simple-tsan.c)
target_compile_options(simple-dice PRIVATE -fsanitize=thread)
target_link_libraries(simple-dice PRIVATE dice tsano pthread)

# target_link_libraries(${TARGET} PRIVATE dice tsano)
