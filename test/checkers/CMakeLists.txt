# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_library(malloc_free_check SHARED malloc_free_check.c)
target_link_libraries(malloc_free_check dice)
