# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

if(NOT ${DICE_LOG_LEVEL} STREQUAL "")
  add_compile_definitions(LOG_LEVEL=${DICE_LOG_LEVEL})
endif()

add_subdirectory(dice)
add_subdirectory(mod)
