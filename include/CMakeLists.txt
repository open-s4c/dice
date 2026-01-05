# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

# dice.h is an interface library that can be linked with objects, libraries, and
# executables via target_link_libraries. That adds dice.h's include path to that
# target. In theory, CMake's way how to do that is using an INTERFACE library
# like this: add_library(dice.h INTERFACE)
#
# However, compile definitions, compile_options, etc are not propagated to the
# targets linking with it. Hence, we instead use an OBJECT library with an empty
# file instead. Should work the same, but it also propagets the compile
# definitions below.
add_library(dice.h OBJECT empty.c)
target_include_directories(
  dice.h INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

install(DIRECTORY dice DESTINATION include)

# Maximum event types
set(DICE_MAX_TYPES
    128
    CACHE STRING "Maximum number of supported events")
target_compile_definitions(dice.h PUBLIC MAX_TYPES=${DICE_MAX_TYPES})

# Maximum number of chains
set(DICE_MAX_CHAINS
    16
    CACHE STRING "Maximum number of chains")
target_compile_definitions(dice.h PUBLIC MAX_CHAINS=${DICE_MAX_CHAINS})

# Maximum slot number of builtin modules
set(DICE_MAX_BUILTIN_SLOTS
    16
    CACHE STRING "Maximum builtin slot number")
target_compile_definitions(dice.h
                           PUBLIC MAX_BUILTIN_SLOTS=${DICE_MAX_BUILTIN_SLOTS})

# LOG_LEVEL
set(DICE_LOG_LEVEL
    ""
    CACHE STRING "Log level log_* functions (FATAL|INFO|DEBUG)")

if(NOT ${DICE_LOG_LEVEL} STREQUAL "")
  target_compile_definitions(dice.h PUBLIC LOG_LEVEL=${DICE_LOG_LEVEL})
endif()

# MEMPOOL_SIZE
set(DICE_MEMPOOL_SIZE
    "1024 * 1024 * 200"
    CACHE STRING "Dice mempool size")
target_compile_definitions(dice.h PUBLIC MEMPOOL_SIZE=${DICE_MEMPOOL_SIZE})

# All modules created with dice can land in shared libraries and have to be
# compiled with -fPIC.
target_compile_options(dice.h PUBLIC -fPIC)
