#!/bin/sh
# Copyright (C) 2025 Huawei Technologies Co., Ltd.
# SPDX-License-Identifier: 0BSD

set -e

SCRIPTS_DIR=$(readlink -f $(dirname $0))
. $SCRIPTS_DIR/benchmark-utils.sh

OUTPUT_DIR=results
WIKI_DIR=dice.wiki
MAKE=make
if which mlr > /dev/null 2>&1; then
	HAS_MILLER=yes
fi

while [ $# -gt 0 ]; do
	case $1 in
	-d)
		shift
		OUTPUT_DIR="$1"
		;;
	-w)
		shift
		WIKI_DIR=$1
		;;
	*)
		echo "invalid option"
		exit 1
		;;
	esac
	shift
done

output() {
    echo "$@" >> $SUMMARY
}

to_output() {
    cat <&0 >> $SUMMARY
}

process_bench() {
	benchfile=$1
	bench=$(basename $benchfile .csv)
	output
	if in_list $bench micro; then
		output "## Microbenchmarks"
		output
		if [ -z "$HAS_MILLER" ]; then
			output '```'
			cat $benchfile | to_output
			output '```'
		else
			cat $benchfile \
			| mlr --icsv --omd cat | to_output
		fi
	else
		output "## Benchmark $bench"
		output
		if [ -z "$HAS_MILLER" ]; then
			output '```'
			cat $benchfile | to_output
			output '```'
		else
			cat $benchfile \
			| sed -e 's/ $//' \
			| mlr --icsv --ifs=' ' --omd cat | to_output
		fi
	fi
}

for name in $(find $OUTPUT_DIR -name 'info.md'); do
	dir=$(dirname $name)
	dat=$(basename $dir)
	tag=$(basename $(dirname $dir))
	host=$(basename $(dirname $(dirname $dir)))

	SUMMARY=$WIKI_DIR/bench/$host.$tag.$dat.md
	rm -f $SUMMARY

	cat $name | to_output
	output # empty lines

	for benchfile in $(ls $dir/*.csv); do
		process_bench $benchfile
	done
done
