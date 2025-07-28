# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_library(dice-bundle SHARED)
target_link_libraries(
  dice-bundle
  PRIVATE dice.o
          mod-self.o
          mod-pthread_create.o
          mod-pthread_mutex.o
          mod-pthread_cond.o
          mod-pthread_rwlock.o
          mod-pthread_rwlock.o
          mod-tsan.o
          mod-stacktrace.o)

add_library(dice-bundle-box SHARED)
target_link_libraries(
  dice-bundle-box
  PRIVATE dice.o
          dice-box.o
          mod-self.o
          mod-pthread_create.o
          mod-pthread_mutex.o
          mod-pthread_cond.o
          mod-pthread_rwlock.o
          mod-pthread_rwlock.o
          mod-tsan.o
          mod-stacktrace.o)
