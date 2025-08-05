#!/bin/sh
# depends on: git, make, cc

# install pkgsrc to get NetBSD dependencies
git clone --depth 1 https://github.com/NetBSD/pkgsrc.git pkgsrc

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
    export PKG_PATH="http://cdn.netbsd.org/pub/pkgsrc/packages/NetBSD/amd64/10.0_2025Q2/All"

    # Get cmake and dependencies
    mkdir -p downloads
    cd downloads
    echo "$PKG_PATH/cmake-3.31.7nb2.tgz" > urls

    # we force pkg_add with -f since the platform does not match
    touch urls.prev
    while true; do
        if diff urls urls.prev; then
            break
        fi
    	cp urls urls.prev
        for url in $(cat urls | grep -v /.tgz); do
            curl -LOC - $url
        done
    	for f in $(ls *.tgz); do
              pkg_add -nf $f 2>&1 \
              | grep ignored \
              | cut -d' ' -f4 \
              | tr -d '>' | tr "=" "-" \
              | xargs -n 1 printf "$PKG_PATH/%s.tgz\n" >> urls.tmp
        done
        cat urls.tmp | sort | uniq | tee urls
    done
)
