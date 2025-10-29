# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

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

  add_library(${TARGET} SHARED ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice dice.h)
  set_target_properties(${TARGET} PROPERTIES PREFIX "")
  if(APPLE)
    # Allow unresolved symbols that will be provided by the sanitizer/runtime at
    # preload time (Annotate*, pthread_*, etc).
    target_link_options(${TARGET} PRIVATE -undefined dynamic_lookup)
  endif()
  install(TARGETS ${TARGET} DESTINATION lib/dice)

  add_library(${TARGET}.o OBJECT ${SRC})
  target_link_libraries(${TARGET}.o PRIVATE dice.h)

  if(NOT "${MODULE}" IN_LIST TSAN_MODULES)
    if(NOT "${DICE_SANITIZER}" STREQUAL "")
      target_compile_options(${TARGET} PUBLIC -fsanitize=${DICE_SANITIZER})
      target_compile_options(${TARGET}.o PRIVATE -fsanitize=${DICE_SANITIZER})
    endif()
  endif()

  if(${DICE_COVERAGE})
    foreach(ENDING "" ".o")
      target_compile_options(${TARGET}${ENDING} PRIVATE --coverage)
      target_link_options(${TARGET}${ENDING} PUBLIC --coverage)
    endforeach()
  endif()
endforeach()

target_link_libraries(dice-self PRIVATE pthread)
target_link_libraries(dice-self.o PUBLIC pthread)
target_link_libraries(dice-cxa PUBLIC stdc++)
