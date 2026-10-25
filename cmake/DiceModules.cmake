# Copyright (C) 2026 Huawei Technologies Co., Ltd.
# SPDX-License-Identifier: 0BSD

include_guard(GLOBAL)
include(CMakeParseArguments)

set(_DICE_OVERRIDE_TYPES "" "Debug")

if("${CMAKE_BUILD_TYPE}" IN_LIST _DICE_OVERRIDE_TYPES)
  set(DICE_LINK_OVERRIDE ON)
  message(STATUS "Linking modules with dice-override")
else()
  set(DICE_LINK_OVERRIDE OFF)
endif()

unset(_DICE_OVERRIDE_TYPES)


# dice_add_interpose_module(
#   <module>
#   SOURCES <source> [<source>...]
#   [INCLUDE_DIRECTORIES <dir> [<dir>...]]
#   [COMPILE_OPTIONS <option> [<option>...]]
#   [DEPENDENCIES <target> [<target>...]]
#   [NO_OBJECT]
#   [NO_SANITIZER]
# )
#
# Creates:
#
#   dice-<module>       SHARED library
#   dice-<module>.o     OBJECT library, unless NO_OBJECT is specified
function(dice_add_interpose_module MODULE)
  set(OPTIONS
      NO_OBJECT
      NO_SANITIZER)

  set(ONE_VALUE_ARGS)

  set(MULTI_VALUE_ARGS
      SOURCES
      INCLUDE_DIRECTORIES
      COMPILE_OPTIONS
      DEPENDENCIES)

  cmake_parse_arguments(
    MOD
    "${OPTIONS}"
    "${ONE_VALUE_ARGS}"
    "${MULTI_VALUE_ARGS}"
    ${ARGN})

  if(MOD_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "dice_add_interpose_module(${MODULE}): "
      "unrecognized arguments: ${MOD_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT MOD_SOURCES)
    message(
      FATAL_ERROR
      "dice_add_interpose_module(${MODULE}): SOURCES is required")
  endif()

  set(TARGET "dice-${MODULE}")

  if(TARGET ${TARGET})
    message(
      FATAL_ERROR
      "dice_add_interpose_module(${MODULE}): "
      "target ${TARGET} already exists")
  endif()

  # --------------------------------------------------------------------------
  # Shared/preloadable module
  # --------------------------------------------------------------------------

  add_library(${TARGET} SHARED ${MOD_SOURCES})

  target_link_libraries(
    ${TARGET}
    PRIVATE
      dice
      dice.h)

  if(MOD_INCLUDE_DIRECTORIES)
    target_include_directories(
      ${TARGET}
      PRIVATE
        ${MOD_INCLUDE_DIRECTORIES})
  endif()

  if(MOD_COMPILE_OPTIONS)
    target_compile_options(
      ${TARGET}
      PRIVATE
        ${MOD_COMPILE_OPTIONS})
  endif()

  set_target_properties(
    ${TARGET}
    PROPERTIES
      PREFIX "")

  if(APPLE)
    target_link_options(
      ${TARGET}
      PRIVATE
        -undefined dynamic_lookup)
  endif()

  if(MOD_DEPENDENCIES)
    add_dependencies(
      ${TARGET}
      ${MOD_DEPENDENCIES})
  endif()

  install(
    TARGETS ${TARGET}
    DESTINATION lib/dice)

  set(MODULE_TARGETS ${TARGET})

  # --------------------------------------------------------------------------
  # Object version
  # --------------------------------------------------------------------------

  if(NOT MOD_NO_OBJECT)
    if(DICE_LINK_OVERRIDE)
      target_link_libraries(
        ${TARGET}
        PRIVATE
          dice-override.o)
    endif()

    set(OBJECT_TARGET "${TARGET}.o")

    add_library(
      ${OBJECT_TARGET}
      OBJECT
        ${MOD_SOURCES})

    target_link_libraries(
      ${OBJECT_TARGET}
      PRIVATE
        dice.h)

    if(MOD_INCLUDE_DIRECTORIES)
      target_include_directories(
        ${OBJECT_TARGET}
        PRIVATE
          ${MOD_INCLUDE_DIRECTORIES})
    endif()

    if(MOD_COMPILE_OPTIONS)
      target_compile_options(
        ${OBJECT_TARGET}
        PRIVATE
          ${MOD_COMPILE_OPTIONS})
    endif()

    if(MOD_DEPENDENCIES)
      add_dependencies(
        ${OBJECT_TARGET}
        ${MOD_DEPENDENCIES})
    endif()

    list(APPEND MODULE_TARGETS ${OBJECT_TARGET})
  endif()

  # --------------------------------------------------------------------------
  # Sanitizer / coverage
  # --------------------------------------------------------------------------

  foreach(MODULE_TARGET IN LISTS MODULE_TARGETS)
    if(NOT MOD_NO_SANITIZER)
      if(NOT "${DICE_SANITIZER}" STREQUAL "")
        target_compile_options(
          ${MODULE_TARGET}
          PUBLIC
            -fsanitize=${DICE_SANITIZER})
      endif()
    endif()

    if(DICE_COVERAGE)
      target_compile_options(
        ${MODULE_TARGET}
        PRIVATE
          --coverage)

      target_link_options(
        ${MODULE_TARGET}
        PUBLIC
          --coverage)
    endif()
  endforeach()
endfunction()
