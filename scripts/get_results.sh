#!/bin/sh
# depends on: git, make, curl, uuid-runtime, rsync, bsdtar (libarchive-tools)

set -e

DATA=$1
DATA=$(readlink -f $DATA)
NAME=dice-smol
SMOL=$DATA/smolBSD
IMG=$NAME.img
SUDO=sudo

(
    CUR=$(pwd)
    cd $SMOL
    $SUDO mount -o loop $IMG mnt

    echo "=== Copy log file ==="
    $SUDO cp mnt/tests.log $CUR

    cat $CUR/tests.log
    echo "=== Was successful? ==="
    success=no
    if [ -f mnt/tests.ok ]; then
        success=yes
    fi
    $SUDO umount mnt

    if [ $success = yes ]; then
        echo YES
        return 0
    else
        echo NO
        return 1
    fi
)
