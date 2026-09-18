# Copyright (C) 2026 Huawei Technologies Co., Ltd.
# SPDX-License-Identifier: 0BSD

include_guard(GLOBAL)
include(CMakeParseArguments)

# Create the generic generated Dice interposition test for one module.
#
# Usage:
#
# dice_add_interpose_test(
#   socket
#   HEADER "<dice/events/socket.h>"
#   FUNCTIONS socket socketpair bind ...
#   MAPPINGS /path/to/signatures.h.in
#   INCLUDE_DIRECTORIES /path/to/external/include
# )
#
# Optional:
#
#   LINK_LIBRARIES ...
#   COMPILE_OPTIONS ...
#   PRELOAD_TARGETS ...
#
function(dice_add_interpose_test MODULE)
  set(OPTIONS)

  set(ONE_VALUE_ARGS
      HEADER)

  set(MULTI_VALUE_ARGS
      FUNCTIONS
      MAPPINGS
      INCLUDE_DIRECTORIES
      LINK_LIBRARIES
      COMPILE_OPTIONS
      PRELOAD_TARGETS)

  cmake_parse_arguments(
    TEST
    "${OPTIONS}"
    "${ONE_VALUE_ARGS}"
    "${MULTI_VALUE_ARGS}"
    ${ARGN})

  if(TEST_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "dice_add_interpose_test(${MODULE}): "
      "unrecognized arguments: ${TEST_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT TEST_HEADER)
    message(
      FATAL_ERROR
      "dice_add_interpose_test(${MODULE}): HEADER is required")
  endif()

  if(NOT TEST_FUNCTIONS)
    message(
      FATAL_ERROR
      "dice_add_interpose_test(${MODULE}): FUNCTIONS is required")
  endif()

  if(NOT TARGET "dice-${MODULE}")
    message(
      FATAL_ERROR
      "dice_add_interpose_test(${MODULE}): "
      "target dice-${MODULE} does not exist")
  endif()

  if(NOT TARGET expand-tests)
    message(
      FATAL_ERROR
      "dice_add_interpose_test(${MODULE}) must be called after "
      "Dice test infrastructure has created expand-tests")
  endif()

  # This function lives in dice/cmake/.
  get_filename_component(
    DICE_TEST_ROOT
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../test"
    ABSOLUTE)

  set(DICE_INTERPOSE_TEST_ROOT
      "${DICE_TEST_ROOT}/interpose")

  set(TARGET "${MODULE}_test")
  set(TARGET_SRC "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c")

  if(TARGET "${TARGET}")
    message(
      FATAL_ERROR
      "dice_add_interpose_test(${MODULE}): "
      "target ${TARGET} already exists")
  endif()

  # The first-pass mapping file containing Dice's built-in functions.
  set(MAPPING_FILES
      "${DICE_INTERPOSE_TEST_ROOT}/interposed_.h.in")

  # External modules append their own function signature mappings.
  list(APPEND MAPPING_FILES ${TEST_MAPPINGS})

  # tmplr receives the function list as one escaped definition.
  string(REPLACE ";" "\\;" FUNCTIONS_ESCAPED "${TEST_FUNCTIONS}")

  # --------------------------------------------------------------------------
  # Generate test source
  # --------------------------------------------------------------------------

  add_custom_command(
    OUTPUT "${TARGET_SRC}"

    COMMAND
      $<TARGET_FILE:tmplr>
      -l 1024
      "-DINCL='${TEST_HEADER}'"
      "-DFOO=${FUNCTIONS_ESCAPED}"
      ${MAPPING_FILES}
      "${DICE_INTERPOSE_TEST_ROOT}/interpose_test.c.in"
      >
      "${TARGET}_.c.tmp"

    COMMAND
      ${CMAKE_COMMAND}
      -E rename
      "${TARGET}_.c.tmp"
      "${TARGET}_.c"

    COMMAND
      $<TARGET_FILE:tmplr>
      -l 1024
      -sP\\$$_
      "-DFOO=${FUNCTIONS_ESCAPED}"
      "${DICE_INTERPOSE_TEST_ROOT}/interposed__.h.in"
      "${TARGET}_.c"
      >
      "${TARGET}.c.tmp"

    COMMAND
      ${CMAKE_COMMAND}
      -E rename
      "${TARGET}.c.tmp"
      "${TARGET}.c"

    WORKING_DIRECTORY
      "${CMAKE_CURRENT_BINARY_DIR}"

    DEPENDS
      tmplr
      "${DICE_INTERPOSE_TEST_ROOT}/interpose_test.c.in"
      "${DICE_INTERPOSE_TEST_ROOT}/interposed_.h.in"
      "${DICE_INTERPOSE_TEST_ROOT}/interposed__.h.in"
      ${TEST_MAPPINGS}

    )

  add_custom_target(
    "expand-test-${MODULE}"
    DEPENDS "${TARGET_SRC}")

  set_source_files_properties(
    "${TARGET_SRC}"
    PROPERTIES GENERATED TRUE)

  add_dependencies(
    expand-tests
    "expand-test-${MODULE}")

  # --------------------------------------------------------------------------
  # Build generated test
  # --------------------------------------------------------------------------

  add_executable(
    "${TARGET}"
    "${TARGET_SRC}")

  add_dependencies(
    "${TARGET}"
    "expand-test-${MODULE}")

  target_link_libraries(
    "${TARGET}"
    PRIVATE
      dice.h
      dice
      tsano
      pthread
      ${TEST_LINK_LIBRARIES})

  # These were previously inherited from test/CMakeLists.txt because the
  # interpose tests lived below that directory. An external extension is a
  # sibling directory, so they must now be applied explicitly.
  target_include_directories(
    "${TARGET}"
    PRIVATE
      "${DICE_TEST_ROOT}/include"
      ${TEST_INCLUDE_DIRECTORIES})

  target_compile_options(
    "${TARGET}"
    PRIVATE
      -DVATOMIC_BUILTINS
      -fno-builtin-memcpy
      -fno-builtin-memset
      -fno-builtin-memmmove
      ${TEST_COMPILE_OPTIONS})

  # --------------------------------------------------------------------------
  # Preload configuration
  # --------------------------------------------------------------------------

  if(TEST_PRELOAD_TARGETS)
    set(PRELOAD_TARGETS ${TEST_PRELOAD_TARGETS})
  else()
    set(PRELOAD_TARGETS "dice-${MODULE}")
  endif()

  set(PRELOAD_FILES)

  foreach(PRELOAD_TARGET IN LISTS PRELOAD_TARGETS)
    if(NOT TARGET "${PRELOAD_TARGET}")
      message(
        FATAL_ERROR
        "dice_add_interpose_test(${MODULE}): "
        "preload target ${PRELOAD_TARGET} does not exist")
    endif()

    list(APPEND
         PRELOAD_FILES
         "$<TARGET_FILE:${PRELOAD_TARGET}>")
  endforeach()

  list(JOIN PRELOAD_FILES ":" PRELOAD)

  add_test(
    NAME "interpose-${TARGET}"
    COMMAND
      ${DICE_SCRIPT}
      -preload "${PRELOAD}"
      "$<TARGET_FILE:${TARGET}>")

  # --------------------------------------------------------------------------
  # Sanitizer configuration
  # --------------------------------------------------------------------------

  if(ENABLE_SANITIZER)
    target_compile_options(
      "${TARGET}"
      PRIVATE
        ${LIBSAN_C_FLAGS})

    target_link_options(
      "${TARGET}"
      PRIVATE
        ${LIBSAN_LD_FLAGS})

    target_link_libraries(
      "${TARGET}"
      PRIVATE
        ${LIBSAN_LINK})
  endif()
endfunction()
