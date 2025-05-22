# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

configure_file(tsano tsano)
configure_file(dice dice)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/tsano ${CMAKE_CURRENT_BINARY_DIR}/dice
  DESTINATION bin
  PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE)
