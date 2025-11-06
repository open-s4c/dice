#!/bin/sh
set -e

MAKE=bmake


ROOT=$(readlink -f $(dirname "$0")/..)
echo "ROOT: $ROOT"

# TODO: ensure we are in git repo

TAG1=v1.1.0
TAG2=v1.2.1

# TODO: ensure we have two versions as input
# TODO: ensure they are in the current repo
echo "TAG1: $TAG1"
echo "TAG2: $TAG2"

# TODO: create temporary regression directory (also flag)
WORK=regression
mkdir -p $WORK

if ! test -d "$WORK/$TAG1"; then
	git clone --branch $TAG1 $ROOT $WORK/$TAG1
fi

if ! test -d "$WORK/$TAG2"; then
	git clone --branch $TAG2 $ROOT $WORK/$TAG2
fi

OPTS=""
OPTS="$OPTS -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
OPTS="$OPTS -DDICE_LTO=on"
OPTS="$OPTS -DCMAKE_BUILD_TYPE=Release"

for tag in $TAG1 $TAG2; do
	cmake -S$WORK/$tag -B$WORK/$tag/build $OPTS
done

for tag in $TAG1 $TAG2; do
	cmake --build $WORK/$tag/build -j
done

for tag in $TAG1 $TAG2; do
	$MAKE -C $WORK/$tag/bench/micro build run process
	cp $WORK/$tag/bench/micro/work/results.csv results-$tag.csv
done

