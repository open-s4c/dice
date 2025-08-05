#!/bin/sh
# depends on: git, make, curl, uuid-runtime, rsync, bsdtar (libarchive-tools)

set -e

git clone --depth 1 https://github.com/NetBSDfr/smolBSD.git smolBSD

(
    cd smolBSD
    make setfetch SETS="base.tar.xz etc.tar.xz comp.tar.xz"

    curl -LO https://smolbsd.org/assets/netbsd-SMOL
)
