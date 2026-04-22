# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_subdirectory(tsano)

set(TMPLR_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/tmplr)
set(TMPLR_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/tmplr)
set(TMPLR_VERSION_H ${TMPLR_BINDIR}/version.h)

file(MAKE_DIRECTORY ${TMPLR_BINDIR})

add_custom_command(
  OUTPUT ${TMPLR_VERSION_H}
  COMMAND /bin/sh -c
          "${TMPLR_SRCDIR}/.versionize.sh -r ${TMPLR_SRCDIR}/version.h.in > ${TMPLR_VERSION_H}"
  DEPENDS ${TMPLR_SRCDIR}/.versionize.sh ${TMPLR_SRCDIR}/version.h.in
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

  # Step 2. prepare tmplr with host default compiler as external project
  set(TMPLR_HOST_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/tmplr-host)
  include(ExternalProject)
  ExternalProject_Add(
    tmplr-host
    SOURCE_DIR ${TMPLR_SRCDIR}
    BINARY_DIR ${TMPLR_HOST_BINDIR}
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
    INSTALL_COMMAND true)

  # Step 3. Create dependency to build tmplr-host version when tmplr is built.
  add_dependencies(tmplr tmplr-host)

  # Step 4. Replace tmplr with tmplr-host (POST_BUILD)
  add_custom_command(
    TARGET tmplr
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${TMPLR_HOST_BINDIR}/tmplr
            $<TARGET_FILE:tmplr>)
endif()
