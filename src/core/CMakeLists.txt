# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_library(bingo-base SHARED rbtree.c idmap.c)
target_link_libraries(bingo-base PRIVATE pthread bingo.h)

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC bingo.h)
add_test(rbtree_test rbtree_test)

set(SRCS mempool.c switcher.c rbtree.c idmap.c)
add_library(bingo-core.o OBJECT ps_builtin.c ${SRCS})
target_link_libraries(bingo-core.o PRIVATE pthread bingo.h)

add_library(bingo-core-plugin.o OBJECT ps_plugin.c ${SRCS})
target_link_libraries(bingo-core-plugin.o PRIVATE pthread bingo.h)
