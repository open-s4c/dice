# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

add_custom_target(
  expand-tsan.c
  COMMAND $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in >
          ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in)
add_dependencies(expand-tsan.c tmplr)

set(TSAN_MODULES tsan stacktrace)

# Compile options to modules can be set like this: set(OPTIONS_memcpy -O2)

file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
  get_filename_component(MODULE ${SRC} NAME_WLE)
  set(TARGET dice-${MODULE})

  add_library(${TARGET} SHARED ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice dice.h)
  target_compile_options(${TARGET} PRIVATE ${OPTIONS_${MODULE}})
  set_target_properties(${TARGET} PROPERTIES PREFIX "")
  install(TARGETS ${TARGET} DESTINATION lib/dice)

  if(NOT ${TARGET} MATCHES "wrap_memset")
    target_link_libraries(${TARGET} PRIVATE dice-override.o)

    add_library(${TARGET}.o OBJECT ${SRC})
    target_link_libraries(${TARGET}.o PRIVATE dice.h)
    target_compile_options(${TARGET}.o PRIVATE)
    target_compile_options(${TARGET}.o PRIVATE ${OPTIONS_${MODULE}})
    set(ENDINGS "" ".o")
  else()
    set(ENDINGS "")
  endif()

  foreach(ENDING ${ENDINGS})
    if(NOT "${MODULE}" IN_LIST TSAN_MODULES)
      if(NOT "${DICE_SANITIZER}" STREQUAL "")
        target_compile_options(${TARGET}${ENDING}
                               PUBLIC -fsanitize=${DICE_SANITIZER})
      endif()
    endif()
    if(${DICE_COVERAGE})
      target_compile_options(${TARGET}${ENDING} PRIVATE --coverage)
      target_link_options(${TARGET}${ENDING} PUBLIC --coverage)
    endif()
  endforeach()
endforeach()

target_link_libraries(dice-self PRIVATE pthread)
target_link_libraries(dice-self.o PUBLIC pthread)
