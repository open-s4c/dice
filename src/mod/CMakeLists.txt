# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_custom_target(
  expand-tsan.c
  COMMAND $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in >
          ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in)
add_dependencies(expand-tsan.c tmplr)

set(TSAN_MODULES tsan stacktrace)

file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
  get_filename_component(MODULE ${SRC} NAME_WLE)
  set(TARGET dice-${MODULE})

  if(${TARGET} STREQUAL "dice-memcpy" AND NOT ${DICE_INTERPOSE_MEMCPY})
    continue()
  endif()

  add_library(${TARGET} SHARED ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice dice.h)
  set_target_properties(${TARGET} PROPERTIES PREFIX "")
  install(TARGETS ${TARGET} DESTINATION lib/dice)

  add_library(${TARGET}.o OBJECT ${SRC})
  target_link_libraries(${TARGET}.o PRIVATE dice.h)

  add_library(${TARGET}_testing.o OBJECT ${SRC})
  target_link_libraries(${TARGET}_testing.o PRIVATE dice.h)
  target_compile_options(${TARGET}_testing.o PRIVATE -DDICE_TEST_INTERPOSE)

  if(NOT "${MODULE}" IN_LIST TSAN_MODULES)
    if(NOT "${DICE_SANITIZER}" STREQUAL "")
      target_compile_options(${TARGET} PUBLIC -fsanitize=${DICE_SANITIZER})
      target_compile_options(${TARGET}.o PRIVATE -fsanitize=${DICE_SANITIZER})
      target_compile_options(${TARGET}_testing.o
                             PRIVATE -fsanitize=${DICE_SANITIZER})
    endif()
  endif()
endforeach()

target_link_libraries(dice-self PRIVATE pthread)
target_link_libraries(dice-self.o PUBLIC pthread)
