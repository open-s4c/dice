# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

cmake_minimum_required(VERSION 3.20)
project(
  dice
  LANGUAGES C CXX ASM
  VERSION 0.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror)

option(DICE_INTERPOSE_MEMCPY "Build dice-memcpy (memcpy, memset, memmove)" off)

# LTO setup
option(DICE_LTO "Build Dice with LTO" on)
if(${DICE_LTO})
  if(CMAKE_SYSTEM_NAME MATCHES "NetBSD" AND "${CMAKE_C_COMPILER_ID}" MATCHES
                                            "Clang")
    add_compile_options(-flto -ffat-lto-objects)
  else()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION on)
  endif()
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

# Coverage
option(DICE_COVERAGE "Enable test coverage" off)

# TSAN on Clang requires the following flag
set(LIBSAN_SHARED)
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(LIBSAN_SHARED -shared-libsan)
endif()

# Sanitizers for tests
set(DICE_SANITIZER
    ""
    CACHE STRING "Compiler sanitizers=thread,address,undefined")
if(NOT "${DICE_SANITIZER}" STREQUAL "")
  set(ENABLE_SANITIZER on)
  set(LIBSAN_thread tsan)
  set(LIBSAN_address asan)
  set(LIBSAN_undefined ubsan)

  message(STATUS "Enable sanitizer: ${DICE_SANITIZER}")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION off)
  set(LIBSAN_C_FLAGS -fsanitize=${DICE_SANITIZER})
  set(LIBSAN_CXX_FLAGS -fsanitize=${DICE_SANITIZER})
  set(LIBSAN_LD_FLAGS ${LIBSAN_SHARED})
  set(LIBSAN_LINK ${LIBSAN_${DICE_SANITIZER}})
endif()

# LOG_LEVEL
set(DICE_LOG_LEVEL
    ""
    CACHE STRING "Log level log_* functions (FATAL|INFO|DEBUG)")

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
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()

option(DICE_BENCHMARKS "Enable Dice benchmarks" ${TESTS_DEFAULT})
if(${DICE_BENCHMARKS})
  add_subdirectory(bench/micro)
  add_subdirectory(bench/lib)
endif()

install(TARGETS dice dice.h EXPORT ${PROJECT_TARGETS})
