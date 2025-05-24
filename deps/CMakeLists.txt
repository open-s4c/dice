# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_subdirectory(tsano)

if(NOT CMAKE_TOOLCHAIN_FILE)
  add_subdirectory(tmplr)
else()
  # If we are cross-compiling Dice, we still have to ensure tmplr is built with
  # the host compiler since it is used to generate some of Dice's source files.
  #
  # To keep using $<TARGET_FILE:tmplr> generator to refer to the tmplr binary,
  # we have to fool CMake. We first build a cross-compiled version of tmplr and
  # then overwrite it with a host-compiled one.

  # Step 1. build tmplr with cross toolchain
  add_executable(tmplr tmplr/tmplr.c)

  # Step 2. prepare tmplr with host default compiler as external project
  set(TMPLR_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/tmplr)
  set(TMPLR_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/tmplr-host)
  include(ExternalProject)
  ExternalProject_Add(
    tmplr-host
    SOURCE_DIR ${TMPLR_SRCDIR}
    BINARY_DIR ${TMPLR_BINDIR}
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
    INSTALL_COMMAND true)

  # Step 3. Create dependency to build tmplr-host version when tmplr is built.
  add_dependencies(tmplr tmplr-host)

  # Step 4. Replace tmplr with tmplr-host (POST_BUILD)
  add_custom_command(
    TARGET tmplr
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${TMPLR_BINDIR}/tmplr $<TARGET_FILE:tmplr>)
endif()
