#!/bin/sh
# Copyright (C) 2025 Huawei Technologies Co., Ltd.
# SPDX-License-Identifier: 0BSD

set -e

SCRIPTS_DIR=$(readlink -f $(dirname $0))
. $SCRIPTS_DIR/benchmark-utils.sh

BENCHMARKS="micro leveldb raytracing scratchapixel"

OPTION_FORCE_RUN=no
OPTION_FORCE_SUM=no

MAKE=make
if which mlr > /dev/null 2>&1; then
	HAS_MILLER=yes
fi
DATE=$(date "+%Y-%m-%d")
TIME=$(date "+%H%M")
HOST=$(hostname -s)
OUTPUT_DIR=results
NAME=default

while [ $# -gt 0 ]; do
	case $1 in
	-b)
		shift
		BENCHMARKS="$1"
		;;
	-n)
		shift
		NAME="$1"
		;;
	-d)
		shift
		OUTPUT_DIR="$1"
		;;
	-H)
		shift
		HOST=$1
		;;
	*)
		echo "invalid option"
		exit 1
		;;
	esac
	shift
done

# ENSURE build/ exists
if [ ! -f build/src/dice/libdice.so ]; then
	echo "Please build project first"
	exit 1
fi

# RUN BENCHMARKS
FR=
if enabled $OPTION_FORCE_RUN; then
    FR="FORCE=1"
fi
FP=
if enabled $OPTION_FORCE_SUM; then
    FP="FORCE=1"
fi

for bench in $BENCHMARKS; do
    $MAKE -sC bench/$bench run $FR
    $MAKE -sC bench/$bench process $FP
done

# COLLECT RESULTS
OUTPUT_DIR=$OUTPUT_DIR/$HOST/$NAME/$DATE
mkdir -p $OUTPUT_DIR

for bench in $BENCHMARKS; do
	cp bench/$bench/work/results.csv $OUTPUT_DIR/$bench.csv
done

# SAVE SOME INFORMATION
INFO=$OUTPUT_DIR/info.md
rm -rf $INFO

output() {
    echo "$@" >> $INFO
}

COMP=$(find build/CMakeFiles -name CMakeCCompiler.cmake | head -n 1)
# get value out of lines such as set(CMAKE_C_COMPILER_ID "GNU")
get_val() {
	cat $COMP | grep "$1 " | cut -d' ' -f2 | cut -d')' -f1 | tr -d '"'
}

output "## Environment and Configuration"
output
output "- Host:     $HOST"
output "- Arch:     $(uname -p)"
output "- OS:       $(uname) $(uname -r)"
output "- Git tag:  $(git rev-parse --short HEAD)"
output "- Branch:   $(git branch --show-current)"
output "- Date:     $DATE"
output "- Compiler: $(get_val C_COMPILER_ID) $(get_val C_COMPILER_VERSION)"
