# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_subdirectory(tsano)
add_subdirectory(tmplr)

# If we are cross-compiling Dice, we still have to build tmplr with the host
# toolchain and then we fool CMake to use that tmplr instead of the
# cross-compiled one.

set(TMPLR_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/tmplr)
set(TMPLR_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/tmplr)
set(TMPLR_HOST ${CMAKE_CURRENT_BINARY_DIR}/tmplr-host)

# We first build tmplr with the host toolchain
include(ExternalProject)
ExternalProject_Add(
  tmplr-host
  SOURCE_DIR ${TMPLR_SRCDIR}
  BINARY_DIR ${TMPLR_HOST}
  CMAKE_ARGS
  INSTALL_COMMAND true)

# After the cross-compiled tmplr is built, we run the "replace-tmplr" target,
# which then copies the host-compiled tmplr over.
add_custom_target(replace-tmplr)
add_custom_command(
  TARGET replace-tmplr
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${TMPLR_HOST}/tmplr ${TMPLR_BINDIR})

# Create dependencies
add_dependencies(replace-tmplr tmplr-host)
add_dependencies(tmplr replace-tmplr)
