#!/bin/sh
set -e

if [ $# -ne 3 ]; then
	echo "usage: $0 <SMOLBSD_DIR> <SERVICE> <PAYLOAD_IMG>"
	exit 1
fi

SMOLDIR=$1
SMOLDIR=$(readlink -f $SMOLDIR)
SERVICE=$2
PAYLOAD_IMG=$3
PAYLOAD_IMG=$(readlink -f $PAYLOAD_IMG)

ARCH=$(uname -m)
if [ "$ARCH" = "x86_64" ]; then
	ARCH=amd64
fi

(
	cd $SMOLDIR
	./startnb.sh \
		-k kernels/netbsd-SMOL \
		-i images/$SERVICE-$ARCH.img \
		-l $PAYLOAD_IMG \
		-x "-no-reboot"
)
