# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

cmake_minimum_required(VERSION 3.20)
project(
  tsano
  LANGUAGES C CXX
  VERSION 1.2)

if(PROJECT_IS_TOP_LEVEL)
  set(DEV_DEFAULT on)
else()
  set(DEV_DEFAULT off)
endif()

set(TSANO_C ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c)
set(TSANO_C_IN ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in)

option(TSANO_DEV "Enable tsano template expansion" ${DEV_DEFAULT})
if(${TSANO_DEV})
  # depends on tmplr: https://github.com/open-s4c/tmplr
  include(cmake/tmplr.cmake)

  add_custom_command(
    OUTPUT ${TSANO_C}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${TMPLR_PROGRAM} -FBITS="8\;16\;32\;64" ${TSANO_C_IN} > ${TSANO_C}
    DEPENDS ${TSANO_C_IN})

  add_custom_target(update-tsano DEPENDS ${TSANO_C})
endif()

add_library(tsano SHARED ${TSANO_C} syscall.c annotate.c fiber.c)
target_compile_options(tsano PRIVATE -Wall -Werror -Wextra)

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
