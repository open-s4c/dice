#!/bin/sh
# depends on: git, make, curl, uuid-runtime, rsync, bsdtar (libarchive-tools)

set -e

DATA=$1
DATA=$(readlink -f $DATA)
NAME=dice-smol
SMOL=$DATA/smolBSD
PKGS=$DATA/pkgsrc/downloads
SETS="base.tar.xz etc.tar.xz comp.tar.xz"
MEM=2048
IMG=$NAME.img
SUDO=sudo

(
    cd $SMOL
    rm -rf rootfs
    mkdir -p rootfs

    echo "=== Install sets ==="
	for s in $SETS; do
        echo "===> $s"
		tar xfp sets/amd64/$s -C rootfs/
	done

    echo "=== Install packages ==="
    mkdir -p rootfs/usr/pkg
    for f in $(ls $PKGS/*.tgz); do
        echo "===> $f"
        tar xfp $f -C rootfs/usr/pkg 2> /dev/null
    done
)

(
    cd $SMOL
    if [ -d mnt ]; then
        $SUDO umount mnt 2> /dev/null || true
        rm -rf mnt
    fi
    mkdir mnt
    echo "=== Make image ==="
    $SUDO ./mkimg.sh -r rootfs -m $MEM -i $IMG
    $SUDO chown $(id -u) $IMG
)
