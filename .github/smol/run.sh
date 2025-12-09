#!/bin/sh
# This script runs in the cicd-ARCH.img called by /etc/rc.
# It assumes the payload image (containing this file too) is mounted at /mnt
# It will first untar the project and then run it
set -e

fail() {
	exit 1
}

echo "extract code..."
mkdir -p /project
tar -xf /mnt/project.tar -C /project || fail
(
	cd /project
	rm -f /mnt/tests.ok
	ls -R .
	echo "configure and build ..."
	cmake -S. -Bbuild || fail
	cmake --build build || fail
	echo "test..."
	ctest --test-dir build --output-on-failure > /mnt/tests.log 2>&1 || fail
	touch /mnt/tests.ok
)
