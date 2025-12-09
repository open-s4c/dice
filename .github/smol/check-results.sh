#!/bin/sh
# We use uppercase file names here because of the fat filesystem

set -e

DIR=$(readlink -f $1)

(
	cd $DIR
	echo "=== Log file ==="
	cat TESTS.LOG

	echo "=== Was successful? ==="
	success=no
	if [ -f TESTS.OK ]; then
		success=yes
	fi
	if [ $success = yes ]; then
		echo YES
		return 0
	else
		echo NO
		return 1
	fi
)
