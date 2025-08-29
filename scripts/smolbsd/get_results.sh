#!/bin/sh

set -e

IMG=$(readlink -f $1)

mkdir -p mnt
mount -o loop $IMG mnt

ls mnt

echo "=== Copy log file ==="
cp mnt/tests.log .

cat tests.log
echo "=== Was successful? ==="
success=no
if [ -f mnt/tests.ok ]; then
	success=yes
fi
umount mnt

if [ $success = yes ]; then
	echo YES
	return 0
else
	echo NO
	return 1
fi
