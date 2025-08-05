#!/bin/sh
# depends on: git, make, curl, uuid-runtime, rsync, bsdtar (libarchive-tools)

set -e

if [ ! -d smolBSD ]; do
    git clone --depth 1 https://github.com/NetBSDfr/smolBSD.git smolBSD
fi

(
    cd smolBSD
    make setfetch SETS="base.tar.xz etc.tar.xz comp.tar.xz"
    curl -LO https://smolbsd.org/assets/netbsd-SMOL
)
