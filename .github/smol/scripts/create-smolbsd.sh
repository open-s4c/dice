#!/bin/sh
set -e

if [ $# -ne 2 ]; then
	echo "usage: $0 <SERVICE_DIR> <SMOLBSD_DIR>"
	echo
	echo "Clones smolBSD project into SMOLBSD_DIR, copies the service"
	echo "pointed by SERVICE_DIR, and builds the service image"
	echo
	exit 1
fi

SERVICE="$1"
SMOLDIR="$2"

OS=$(uname -s)
ARCH=$(uname -m)
if [ "$ARCH" = "x86_64" ]; then
	ARCH=amd64
fi

# select a MAKE
if [ -z "$MAKE" ]; then
	case "$OS" in
	NetBSD)
		MAKE=make
		;;
	*)
		MAKE=bmake
		;;
	esac
fi


# clone repo if it does not exit
if [ ! -d smolBSD ]; then
	git clone --depth 1 https://github.com/NetBSDfr/smolBSD.git $SMOLDIR
fi

# copy the new service over
cp -r $SERVICE $SMOLDIR/service/

# fetch dependencies and build image
if [ ! -f $SMOLDIR/images/build-$ARCH.img ]; then
	$MAKE -C $SMOLDIR fetchimg
fi
$MAKE -C $SMOLDIR SERVICE=$SERVICE build
