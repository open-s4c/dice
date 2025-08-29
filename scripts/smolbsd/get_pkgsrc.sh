#!/bin/sh
# depends on: git, make, cc

set -e

BASE="http://cdn.netbsd.org/pub/pkgsrc/packages/NetBSD"
ARCH=amd64
VERSION="10.0_2025Q2"
PACKAGES="$@"

# install pkgsrc to get NetBSD dependencies
if [ ! -d pkgsrc ]; then
    git clone --depth 1 https://github.com/NetBSD/pkgsrc.git
fi

# bootstrap
PKGBUILD=$(pwd)/pkgsrc/build
unset PKG_PATH
if [ ! -f $PKGBUILD/sbin/pkg_add ]; then
(
	cd pkgsrc/bootstrap
	rm -rf work
	./bootstrap --unprivileged --prefix $PKGBUILD
)
fi

# download packages
export PKG_PATH=$BASE/$ARCH/$VERSION
if [ ! -z "$PACKAGES" ]; then
(
	cd pkgsrc
	export PATH="$PKGBUILD/sbin:$PKGBUILD/bin:$PATH"

	# get current list of files
	if [ ! -f SHA512 ]; then
		curl -LOC - "$PKG_PATH/SHA512.bz2" > /dev/null
		bunzip2 SHA512.bz2
	fi
	if [ ! -f package.path ]; then
		echo "=== Extract package path from SHA512 ==="
		cat SHA512 | sed 's/SHA512 (\(.*\)) =.*/\1/g' > package.path
	fi

	# create package lists
	for pkg in $PACKAGES; do
		echo $pkg
	done > package.list
	echo "=== Initial package list ==="
	cat package.list
	rm -f package.prev
	touch package.prev

	# start processing package list
	mkdir -p downloads
	while true; do

		# if list hasn't change in last iteration, we are done
		if diff package.list package.prev; then
		    break
		fi > /dev/null

		# list has changed, new packages must be installed.
		cp package.list package.prev

		# download all packages in package.list
		for pkg in $(cat package.list); do
			if [ -f downloads/$pkg.tgz ]; then
				continue
			fi
			pkgver=$(cat package.path | grep All/$pkg | head -n1)
			url=$PKG_PATH/$pkgver
			echo "=== Downloading $pkgver ==="
			curl -LC- -o downloads/$pkg.tgz $url 2> /dev/null
		done

		# install all packages and update dependencies in package.list
		for f in $(ls downloads/*.tgz); do
			echo "=== Fake installing $f ==="
			# force pkg_add with -f because platform does not match
			pkg_add -nf $f 2>&1 \
			| tee package.last_install \
			| grep ignored \
			| cut -d' ' -f4 \
			| cut -d'>' -f1 >> package.tmp
		done
		echo "=== Updated package list ==="
		cat package.tmp | sort | uniq | tee package.list
	done
)
fi
