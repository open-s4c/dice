#!/bin/sh
# depends on: bsdtar (libarchive-tools), qemu-system

set -e

DATA=$1
DATA=$(readlink -f $DATA)
NAME=dice-smol
IMG=$NAME.img
SMOL=$DATA/smolBSD
MEM=2048
SUDO=

# run test case
(
    cd $SMOL
    $SUDO ./startnb.sh -k netbsd-SMOL -i $IMG -m $MEM -x "-no-reboot"
)

# extract results

