# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_library(dice-base SHARED rbtree.c idmap.c)
target_link_libraries(dice-base PRIVATE pthread dice.h)

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC dice.h)
add_test(rbtree_test rbtree_test)

set(SRCS mempool.c switcher.c rbtree.c idmap.c)
add_library(dice-core.o OBJECT ps_builtin.c ${SRCS})
target_link_libraries(dice-core.o PRIVATE pthread dice.h)

add_library(dice-core-plugin.o OBJECT ps_plugin.c ${SRCS})
target_link_libraries(dice-core-plugin.o PRIVATE pthread dice.h)
