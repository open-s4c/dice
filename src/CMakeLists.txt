# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

if(NOT ${DICE_LOG_LEVEL} STREQUAL "")
  add_compile_definitions(LOG_LEVEL=${DICE_LOG_LEVEL})
endif()

# Absolutely all modules created in this subtree can land in shared libraries
# and have to be compiled with -fPIC.
add_compile_options(-fPIC)

add_subdirectory(dice)
add_subdirectory(mod)
