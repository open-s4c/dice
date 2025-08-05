#!/bin/sh
# depends on: git, make, cc

set -e

# install pkgsrc to get NetBSD dependencies
if [ ! -d pkgsrc ]; then
    git clone --depth 1 https://github.com/NetBSD/pkgsrc.git
fi

# pkgsrc pkgsrc
PKGBUILD=$(pwd)/pkgsrc/build
unset PKG_PATH

# bootstrap
(
    cd pkgsrc/bootstrap
    ./bootstrap --unprivileged --prefix $PKGBUILD
)

# download packages
(
    cd pkgsrc
    export PATH="$PKG/sbin:$PKG/bin:$PATH"
    export PKG_PATH="http://cdn.netbsd.org/pub/pkgsrc/packages/NetBSD/amd64/10.0_2025Q2"

    # get current list of files
    if [ ! -f SHA512 ]; then
        curl -LOC - "$PKG_PATH/SHA512.bz2" > /dev/null
        bunzip2 SHA512.bz2
    fi
    cat SHA512 | sed 's/SHA512 (\(.*\)) =.*/\1/g' > package.path

    # Get cmake and dependencies
    mkdir -p downloads
    echo cmake > package.list

	echo "=== Initial package list ==="
	cat package.list

    # we force pkg_add with -f since the platform does not match
    rm -f package.prev
    touch package.prev
    while true; do
        if diff package.list package.prev > /dev/null; then
            break
        fi
    	cp package.list package.prev

    	# download all packages in package.list
        for pkg in $(cat package.list); do
            if [ -f downloads/$pkg.tgz ]; then
                continue
            fi
            url=$PKG_PATH/$(cat package.path | grep All/$pkg | head -n1)
            echo "=== Downloading $pkg ==="
            curl -LC- -o downloads/$pkg.tgz $url 2> /dev/null
        done

    	# dry install all packages and update dependencies in package.list
    	for f in $(ls downloads/*.tgz); do
              echo "=== Fake installing $f ==="
              pkg_add -nf $f 2>&1 | tee package.last_install \
              | grep ignored \
              | cut -d' ' -f4 \
              | cut -d'>' -f1 >> package.tmp
            #bash
    	done
    	echo "=== Updated package list ==="
        cat package.tmp | sort | uniq | tee package.list
    done
)
