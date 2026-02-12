# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

if(${ENABLE_SANITIZER})
  return()
endif()

file(GLOB SRCS *.c *.cpp)
foreach(SRC ${SRCS})
  get_filename_component(TARGET ${SRC} NAME_WLE)

  add_executable(${TARGET} ${SRC})
  target_compile_options(${TARGET} PRIVATE -fsanitize=thread)
  target_link_options(${TARGET} PRIVATE ${LIBSAN_SHARED})
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_link_options(${TARGET} PRIVATE -fsanitize=thread)
  endif()
  if(${SRC} MATCHES ".*\.cpp")
    target_compile_options(${TARGET} PRIVATE -std=c++11)
  elseif(${SRC} MATCHES ".*\.c")
    target_compile_options(${TARGET} PRIVATE -Wno-incompatible-pointer-types)
  endif()

  target_link_libraries(${TARGET} PRIVATE mock_checker tsano pthread)

  add_test(
    NAME ${TARGET}-test
    COMMAND
      ${DICE_SCRIPT} -pthread -malloc -self -cxa -tsan #
      -preload $<TARGET_FILE:trace_checker> #
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} #
      COMMAND_EXPAND_LISTS)
endforeach()
