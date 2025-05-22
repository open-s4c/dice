# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_custom_command(
  OUTPUT tsano.c
  COMMAND env $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in >
          tsano.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in)
add_custom_target(tsano.c DEPENDS tsano.c)

add_library(tsano SHARED tsano.c)
install(TARGETS tsano DESTINATION lib/bingo)
