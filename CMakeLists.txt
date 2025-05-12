# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)
project(
  bingo
  LANGUAGES C CXX ASM
  VERSION 0.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION on)

add_compile_options(-Wall -Wextra -Werror -g3)

if(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  add_compile_options(-D_XOPEN_SOURCE=700)
else()
  add_compile_options(-D_GNU_SOURCE)
endif()

find_package(libvsync 4.2 CONFIG REQUIRED)
link_libraries(libvsync::vsync)

include(GNUInstallDirs)
include(CheckSymbolExists)
include(cmake/export.cmake)

enable_testing()
add_subdirectory(scripts)
add_subdirectory(deps/tmplr)
add_subdirectory(include)
add_subdirectory(tsano)
add_subdirectory(core)
add_subdirectory(mod)
add_subdirectory(fastch)
add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(bench/micro)

# install(TARGETS bingo bingo.h bingo-core EXPORT ${PROJECT_TARGETS})
install(TARGETS bingo.h EXPORT ${PROJECT_TARGETS})
