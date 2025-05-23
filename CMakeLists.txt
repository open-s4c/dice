# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)
project(
  tsano
  LANGUAGES C CXX
  VERSION 1.0)

# tmplr must be installed to update tsano.c: https://github.com/open-s4c/tmplr

add_custom_command(
  OUTPUT tsano.c
  COMMAND env tmplr ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in >
          ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in)
add_custom_target(tsano.c DEPENDS tsano.c)

add_library(tsano SHARED ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c)

install(TARGETS tsano DESTINATION lib)
install(
  FILES tsano
  DESTINATION bin
  PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE)
