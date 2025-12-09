#!/bin/sh
# on linux require mtools: apt install mtools
#
set -e

if [ $# -ne 2 ]; then
	echo "usage: $0 <IMAGE_PATH> <UNPACK_DIR>"
	exit 1
fi

IMG=$1
DIR=$2
IMG=$(readlink -f $IMG)
DIR=$(readlink -f $DIR)

MTOOLSRC=$HOME/.mtoolsrc

# save current state of MTOOLSRC
if [ -f $MTOOLSRC ]; then
	cp $MTOOLSRC $MTOOLSRC.bak
fi
echo "drive x: file=\"$IMG\"" > $MTOOLSRC

mkdir -p $DIR
mcopy 'x:/*' $DIR/

# restore current state of MTOOLSRC
if [ -f $MTOOLSRC.bak ]; then
	mv $MTOOLSRC.bak $MTOOLSRC
fi
