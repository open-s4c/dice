#!/bin/bash
# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

set -e

# Depends on ripgrep (rg) and miller (mlr)

DIR=.
rg --no-heading                 \
   "define EVENT_.*[0-9]+" $DIR \
    | grep -v scripts           \
    | tr ":" " " | tr -s " "    \
    | cut -d" " -f1,3,4         \
    | mlr --ifs=" " --opprint   \
      label file,event,constant \
      then cut -o -f event,constant,file \
      then sort -n -f constant \
    | less
