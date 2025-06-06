# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

set(SRCS mempool.c switcher.c rbtree.c)

add_library(dice SHARED ps_plugin.c ${SRCS})
target_link_libraries(dice PUBLIC dice.h)
target_link_libraries(dice PRIVATE pthread)
install(TARGETS dice DESTINATION lib)

add_library(dice.o OBJECT ps_builtin.c ${SRCS})
target_link_libraries(dice.o PRIVATE pthread dice.h)

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC dice.h)

add_test(NAME rbtree_test COMMAND rbtree_test)
