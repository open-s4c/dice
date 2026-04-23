#!/bin/sh

set -e
TESTS=$(ls *.in)

for t in $TESTS; do
	./runtest.sh "$t"
done
