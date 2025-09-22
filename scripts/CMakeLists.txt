# Copyright (C) 2025 Huawei Technologies Co., Ltd.
# SPDX-License-Identifier: 0BSD

configure_file(dice dice)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/dice
  DESTINATION bin
  PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE)

# for tests
set(DICE_SCRIPT
    ${PROJECT_SOURCE_DIR}/scripts/dice
    PARENT_SCOPE)
