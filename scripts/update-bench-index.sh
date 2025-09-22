#!/bin/sh
# Copyright (C) 2025 Huawei Technologies Co., Ltd.
# SPDX-License-Identifier: 0BSD

set -e

# assume starting from project root and dice.wiki is cloned there
WIKI_DIR=dice.wiki

# or if one argument is passed, use that as directory
if [ ! -z "$1" ]; then
	WIKI_DIR="$1"
fi

BENCH_DIR=$WIKI_DIR/bench

# create a simple header text for the benchmark README
cat <<-'EOF' | tee $WIKI_DIR/Home.md
# Dice Benchmarks

On the right sidebar is a list of benchmark results ran on
different hosts.  To benchmarks Dice on your machine, checkout
`dice.wiki`, run `scripts/benchmark.sh`, and update this files with
`scripts/update-bench-index.sh`.


EOF

# iterate over the different hosts and dates and create lists of links
hosts=$(ls $BENCH_DIR/bench-*.md | cut -d- -f2 | sort | uniq)
for host in $hosts; do
    dates=$(ls $BENCH_DIR/bench-$host-*.md | cut -d- -f3- | cut -d. -f1 | sort | uniq)
    echo "### Host $host"
    for d in $dates; do
        variants=$(ls $BENCH_DIR/*-$host-*$d*.md)
        variants=$(basename $variants)
        variants=$(echo $variants | cut -d. -f2 | sort | uniq)
        for v in $variants; do
            if [ "$v" = "md" ]; then
                echo "- [$d](bench-$host-$d)"
            else
                echo "- [$d $v](bench-$host-$d.$v)"
            fi
        done
    done
    echo
done | tee $WIKI_DIR/_Sidebar.md
