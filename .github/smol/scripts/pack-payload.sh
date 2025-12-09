#!/bin/sh
# on linux require mtools: apt install mtools
#
set -e

if [ $# -lt 2 ]; then
	echo "usage: $0 <IMAGE_PATH> <FILE> [FILE]..."
	exit 1
fi

IMG=$1
IMG=$(readlink -f $IMG)
shift

MTOOLSRC=$HOME/.mtoolsrc

# prepare empty image
dd if=/dev/zero of=$IMG bs=1M count=16
mkfs.fat -F 16 $IMG
IMG=$(readlink -f $IMG)


# save current state of MTOOLSRC
if [ -f $MTOOLSRC ]; then
	cp $MTOOLSRC $MTOOLSRC.bak
fi
echo "drive x: file=\"$IMG\"" > $MTOOLSRC

for f in $@; do
	echo "copying $f"
	mcopy $f x:/
done

# restore current state of MTOOLSRC
if [ -f $MTOOLSRC.bak ]; then
	mv $MTOOLSRC.bak $MTOOLSRC
fi
