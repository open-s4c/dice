# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_subdirectory(tsano)

set(TMPLR_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/tmplr)
set(TMPLR_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/tmplr)
set(TMPLR_VERSION_H ${TMPLR_BINDIR}/version.h)
set(TMPLR_VERSION_IN ${TMPLR_BINDIR}/version.h.in)
set(TMPLR_VERSIONIZE_CMAKE ${CMAKE_CURRENT_BINARY_DIR}/tmplr-versionize.cmake)

file(MAKE_DIRECTORY ${TMPLR_BINDIR})
file(WRITE ${TMPLR_VERSION_IN}
     "#ifndef TMPLR_VERSION_H\n#define TMPLR_VERSION_H\n#define TMPLR_VERSION \"dice-__VERSION__\"\n#endif\n")
file(WRITE ${TMPLR_VERSIONIZE_CMAKE}
     "execute_process(\n"
     "  COMMAND \"${TMPLR_SRCDIR}/.versionize.sh\" -r \"\${INPUT}\"\n"
     "  OUTPUT_FILE \"\${OUTPUT}\"\n"
     "  COMMAND_ERROR_IS_FATAL ANY)\n")

add_custom_command(
  OUTPUT ${TMPLR_VERSION_H}
  COMMAND ${CMAKE_COMMAND} -DINPUT=${TMPLR_VERSION_IN} -DOUTPUT=${TMPLR_VERSION_H}
          -P ${TMPLR_VERSIONIZE_CMAKE}
  DEPENDS ${TMPLR_SRCDIR}/.versionize.sh ${TMPLR_VERSION_IN}
  WORKING_DIRECTORY ${TMPLR_SRCDIR}
  VERBATIM)

function(dice_configure_tmplr_target)
  target_include_directories(tmplr PRIVATE ${TMPLR_SRCDIR}/include
                                            ${TMPLR_BINDIR})
  set_target_properties(
    tmplr
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endfunction()

if(NOT CMAKE_TOOLCHAIN_FILE)
  add_executable(tmplr ${TMPLR_SRCDIR}/tmplr.c ${TMPLR_VERSION_H})
  dice_configure_tmplr_target()
else()
  # If we are cross-compiling Dice, we still have to ensure tmplr is built with
  # the host compiler since it is used to generate some of Dice's source files.
  #
  # To keep using $<TARGET_FILE:tmplr> generator to refer to the tmplr binary,
  # we have to fool CMake. We first build a cross-compiled version of tmplr and
  # then overwrite it with a host-compiled one.

  # Step 1. build tmplr with cross toolchain
  add_executable(tmplr ${TMPLR_SRCDIR}/tmplr.c ${TMPLR_VERSION_H})
  dice_configure_tmplr_target()

  # Step 2. build tmplr with the host compiler in a separate directory.
  set(TMPLR_HOST_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/tmplr-host)
  set(TMPLR_HOST_VERSION_H ${TMPLR_HOST_BINDIR}/version.h)
  set(TMPLR_HOST_VERSION_IN ${TMPLR_HOST_BINDIR}/version.h.in)
  find_program(TMPLR_HOST_CC NAMES cc clang gcc REQUIRED)

  add_custom_command(
    OUTPUT ${TMPLR_HOST_BINDIR}/tmplr
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TMPLR_HOST_BINDIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${TMPLR_VERSION_IN} ${TMPLR_HOST_VERSION_IN}
    COMMAND
      ${CMAKE_COMMAND} -DINPUT=${TMPLR_HOST_VERSION_IN}
      -DOUTPUT=${TMPLR_HOST_VERSION_H} -P ${TMPLR_VERSIONIZE_CMAKE}
    COMMAND
      ${TMPLR_HOST_CC} -std=c99 -O3 -Wall -Wextra -Wpedantic -Wshadow -Werror
      -I${TMPLR_SRCDIR}/include -I${TMPLR_HOST_BINDIR} -o
      ${TMPLR_HOST_BINDIR}/tmplr ${TMPLR_SRCDIR}/tmplr.c
    DEPENDS ${TMPLR_SRCDIR}/tmplr.c ${TMPLR_SRCDIR}/.versionize.sh
            ${TMPLR_VERSION_IN})
  add_custom_target(tmplr-host DEPENDS ${TMPLR_HOST_BINDIR}/tmplr)

  # Step 3. Create dependency to build tmplr-host version when tmplr is built.
  add_dependencies(tmplr tmplr-host)

  # Step 4. Replace tmplr with tmplr-host (POST_BUILD)
  add_custom_command(
    TARGET tmplr
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${TMPLR_HOST_BINDIR}/tmplr
            $<TARGET_FILE:tmplr>)
endif()
