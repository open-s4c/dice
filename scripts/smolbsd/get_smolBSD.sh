#!/bin/sh

set -e

git clone --depth 1 https://github.com/NetBSDfr/smolBSD.git smolBSD
(
	cd smolBSD
	curl -LO https://smolbsd.org/assets/netbsd-SMOL
	gmake setfetch SETS="base.tar.xz etc.tar.xz comp.tar.xz"
)
