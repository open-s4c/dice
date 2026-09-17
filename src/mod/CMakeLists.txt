# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

set(TSAN_C ${CMAKE_CURRENT_BINARY_DIR}/tsan.c)
set(TSAN_C_IN ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in)

add_custom_command(
  OUTPUT ${TSAN_C}
  COMMAND $<TARGET_FILE:tmplr> -P_tmpl ${TSAN_C_IN} > tsan.c.tmp
  COMMAND ${CMAKE_COMMAND} -E rename tsan.c.tmp tsan.c
  DEPENDS tmplr ${TSAN_C_IN}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(expand-tsan.c DEPENDS ${TSAN_C})
set_source_files_properties(${TSAN_C} PROPERTIES GENERATED TRUE)

set(OVERRIDE_TYPES "" "Debug")
if("${CMAKE_BUILD_TYPE}" IN_LIST OVERRIDE_TYPES)
  set(LINK_OVERRIDE on)
  message(STATUS "Linking modules with dice-override")
endif()

set(TSAN_MODULES tsan stacktrace)

# Compile options to modules can be set like this: set(OPTIONS_memcpy -O2)
set(OPTIONS_memcpy -fno-builtin-memcpy -fno-builtin-memset
                   -fno-builtin-memmmove)

set(TSAN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c)

file(GLOB SRCS *.c)
list(APPEND SRCS ${TSAN_SOURCE})

if(APPLE)
  list(REMOVE_ITEM SRCS ${CMAKE_CURRENT_SOURCE_DIR}/pthread_spinlock.c)
endif()

foreach(SRC ${SRCS})
  get_filename_component(MODULE ${SRC} NAME_WLE)
  set(TARGET dice-${MODULE})
  set(BUILD_SRC ${SRC})
  if("${MODULE}" STREQUAL "tsan")
    set(BUILD_SRC ${TSAN_C})
  endif()
  if(NOT ${DICE_MEMSET} AND ${TARGET} MATCHES "wrap_memset")
    continue()
  endif()

  add_library(${TARGET} SHARED ${BUILD_SRC})
  target_link_libraries(${TARGET} PRIVATE dice dice.h)
  target_compile_options(${TARGET} PRIVATE ${OPTIONS_${MODULE}})
  set_target_properties(${TARGET} PROPERTIES PREFIX "")
  if(APPLE)
    # Allow unresolved symbols that will be provided by the sanitizer/runtime at
    # preload time (Annotate*, pthread_*, etc).
    target_link_options(${TARGET} PRIVATE -undefined dynamic_lookup)
  endif()
  install(TARGETS ${TARGET} DESTINATION lib/dice)

  if(NOT ${TARGET} MATCHES "wrap_memset")
    if(${LINK_OVERRIDE})
      target_link_libraries(${TARGET} PRIVATE dice-override.o)
    endif()

    add_library(${TARGET}.o OBJECT ${BUILD_SRC})
    target_link_libraries(${TARGET}.o PRIVATE dice.h)
    target_compile_options(${TARGET}.o PRIVATE)
    target_compile_options(${TARGET}.o PRIVATE ${OPTIONS_${MODULE}})
    set(ENDINGS "" ".o")
  else()
    set(ENDINGS "")
  endif()

  foreach(ENDING ${ENDINGS})
    if("${MODULE}" STREQUAL "tsan")
      add_dependencies(${TARGET}${ENDING} expand-tsan.c)
    endif()
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

if(${DICE_MEMSET})
  target_link_libraries(dice-wrap_memset PRIVATE dice-memcpy)
endif()
target_link_libraries(dice-self PRIVATE pthread)
target_link_libraries(dice-self.o PUBLIC pthread)
if(APPLE)
  target_link_libraries(dice-cxa PUBLIC stdc++)
endif()
