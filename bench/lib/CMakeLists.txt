# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_library(dice-bundle SHARED)
target_link_libraries(
  dice-bundle
  PRIVATE dice.o
          dice-self.o
          dice-pthread_create.o
          dice-pthread_mutex.o
          dice-pthread_cond.o
          dice-pthread_rwlock.o
          dice-pthread_rwlock.o
          dice-tsan.o
          dice-stacktrace.o)

add_library(dice-bundle-box SHARED)
target_link_libraries(
  dice-bundle-box
  PRIVATE dice.o
          dice-box.o
          dice-self.o
          dice-pthread_create.o
          dice-pthread_mutex.o
          dice-pthread_cond.o
          dice-pthread_rwlock.o
          dice-pthread_rwlock.o
          dice-tsan.o
          dice-stacktrace.o)
