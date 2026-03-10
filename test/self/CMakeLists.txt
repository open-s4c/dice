# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_executable(self_unique_test self_unique_test.c)
target_link_libraries(self_unique_test PRIVATE dice-self.o dice dice.h)
add_test(NAME self-unique-test COMMAND ${DICE_SCRIPT} ./self_unique_test)

add_executable(self_wait_test self_wait_test.c)
target_link_libraries(self_wait_test PRIVATE dice-self.o dice-pthread_create.o dice dice.h)
add_test(NAME self-wait-test COMMAND ${DICE_SCRIPT} ./self_wait_test)
set_tests_properties(self-wait-test PROPERTIES TIMEOUT 5)
