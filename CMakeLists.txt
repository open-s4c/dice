# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)
project(
  dice
  LANGUAGES C CXX ASM
  VERSION 0.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror)

# LTO setup
if(CMAKE_SYSTEM_NAME MATCHES "NetBSD" AND "${CMAKE_C_COMPILER_ID}" MATCHES
                                          "Clang")
  add_compile_options(-flto -ffat-lto-objects)
else()
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION on)
endif()

# POSIX extensions
if(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  add_compile_options(-D_XOPEN_SOURCE=700)
else()
  add_compile_options(-D_GNU_SOURCE)
endif()

find_package(libvsync 4.2 CONFIG)
if(NOT libvsync_DIR)
  set(CMAKE_FIND_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/libvsync)
  find_package(libvsync 4.2 CONFIG REQUIRED)
endif()
message(STATUS "Using libvsync_DIR: ${libvsync_DIR}")
link_libraries(libvsync::vsync)

include(GNUInstallDirs)
include(CheckSymbolExists)
include(cmake/export.cmake)

add_subdirectory(scripts)
add_subdirectory(deps)
add_subdirectory(include)
add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
  set(TESTS_DEFAULT on)
else()
  set(TESTS_DEFAULT off)
endif()
option(DICE_TESTS "Enable Dice tests" ${TESTS_DEFAULT})
if(${DICE_TESTS})
  enable_testing()
  add_subdirectory(test)
  add_subdirectory(bench/micro)
  add_subdirectory(bench/lib)
endif()

install(TARGETS dice dice.h EXPORT ${PROJECT_TARGETS})
