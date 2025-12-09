#!/bin/sh
# This script runs in the cicd-ARCH.img called by /etc/rc.
# It assumes the payload image (containing this file too) is mounted at /mnt
# It will first untar the example build it and then run it
set -e

fail() {
	exit 1
}

echo "extract code..."
tar -xf /mnt/example.tar -C / || fail
(
	cd /example
	echo "configure and build ..."
	cmake -S. -Bbuild || fail
	cmake --build build || fail
	echo "test..."
	ctest --test-dir build --output-on-failure 2>&1 | tee /mnt/tests.log || fail
	touch /mnt/tests.ok
)
