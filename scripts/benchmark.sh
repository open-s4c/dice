#!/bin/sh
# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
#
set -e

OPTION_CLEAN=no
OPTION_CONFIG=no
OPTION_BUILD=no
OPTION_RUN_MICRO=yes
OPTION_SUM_MICRO=yes
OPTION_RUN_LEVELDB=yes
OPTION_SUM_LEVELDB=yes

OPTION_FORCE_RUN=no
OPTION_FORCE_SUM=no

OPTION_PULL_WIKI=no
OPTION_MVTO_WIKI=yes
WIKI_URL="ssh://git@github.com/open-s4c/dice.wiki.git"
MAKE=make

enabled() {
    if [ -z "$1" ] || [ "$1" != "yes" ]
    then return 1
    else return 0
    fi
}

# CONFIG AND BUILD DICE

if enabled $OPTION_CLEAN; then
    echo rm -rf build
fi

if enabled $OPTION_CONFIG; then
    cmake -S . -Bbuild -DCMAKE_BUILD_TYPE=Release
fi

if enabled $OPTION_BUILD; then
    cmake --build build
fi

# RUN BENCHMARKS
F=
if enabled $OPTION_FORCE_RUN; then
    F="FORCE=1"
fi

if enabled $OPTION_RUN_MICRO; then
    $MAKE -sC bench/micro run $F
fi

if enabled $OPTION_RUN_LEVELDB; then
    $MAKE -sC bench/leveldb run $F
fi

# SUMMARY

DATE=$(date "+%Y-%m-%d")
HOST=$(hostname -s)
SUMMARY=bench-$HOST-$DATE.md
F=
if enabled $OPTION_FORCE_SUM; then
    F="FORCE=1"
fi

rm -f $SUMMARY

output() {
    echo "$@" >> $SUMMARY
}

output "## Environment"
output
output "- Host: $HOST"
output "- Date: $DATE"
output "- Tag:  $(git rev-parse --short HEAD) ($(git branch --show-current))"
output
output '```'
output "$(uname -a | fmt -w 70)"
output '```'

if enabled $OPTION_SUM_MICRO; then
    $MAKE -sC bench/micro process $F

    output
    output "## Microbenchmarks"
    cat bench/micro/work/results.csv | mlr --icsv --omd cat >> $SUMMARY
fi

if enabled $OPTION_SUM_LEVELDB; then
    $MAKE -sC bench/leveldb process $F

    output
    output "## LevelDB"
    cat bench/leveldb/work/results.csv \
    | mlr --icsv --ifs=\; --omd cat >> $SUMMARY
fi

# PULL WIKI

if enabled $OPTION_PULL_WIKI; then
    if [ ! -d dice.wiki ]; then
        git clone $WIKI_URL
    fi

    if [ -d dice.wiki ]; then
        (cd dice.wiki && git pull)
    fi
fi

# MOVE REPORT TO WIKI

if enabled $OPTION_MVTO_WIKI; then
    mv $SUMMARY dice.wiki/bench
fi

