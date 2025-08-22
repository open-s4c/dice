# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(self_unique_test self_unique_test.c)
target_link_libraries(self_unique_test PRIVATE dice.h)
target_link_libraries(self_unique_test PRIVATE mod-self.o dice.o)
add_test(NAME self-unique-test COMMAND self_unique_test)
