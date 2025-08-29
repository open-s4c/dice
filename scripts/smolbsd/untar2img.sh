#!/bin/sh

set -e

DIR="/usr/pkg"
PKGS=""
TAR=bsdtar
OPTS="-o loop"
while [ $# -gt 0 ]; do
	case $1 in
	-C)
		shift
		DIR=$1
	;;
	-t)
		shift
		TAR=$1
	;;
	-o)
		shift
		OPTS=$1
	;;
	-i)
		shift
		IMG=$1
	;;
	*)
		PKGS="$PKGS $1"
	;;
	esac
	shift
done

if [ -z "$PKGS" ] || [ -z "$IMG" ]; then
	echo "usage: $0 [-C DIR] -i <IMAGE> <TGZ-FILE> ..."
	exit 1
fi

IMG=$(readlink -f $IMG)

mkdir -p mnt
mount $OPTS $IMG mnt
mkdir -p mnt$DIR

echo "== Target $IMG:$DIR =="
for pkg in $PKGS; do
	echo "=== Package $pkg ==="
	$TAR xf $pkg -C mnt$DIR
done

echo "== Image ready =="
umount mnt
