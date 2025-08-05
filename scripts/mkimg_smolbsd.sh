#!/bin/sh
# depends on: git, make, curl, uuid-runtime, rsync, bsdtar (libarchive-tools)

set -e

DATA=$1
DATA=$(readlink -f $DATA)
NAME=dice-smol
SMOL=$DATA/smolBSD
PKGS=$DATA/pkgsrc/downloads
SETS="base.tar.xz etc.tar.xz comp.tar.xz"
MEM=2048
IMG=$NAME.img
SUDO=sudo

(
    cd $SMOL
    if [ -d mnt ]; then
        $SUDO umount mnt 2> /dev/null || true
        $SUDO rm -rf mnt
    fi
    $SUDO mkdir mnt
    #if [ ! -f $IMG ]; then
        echo "=== Make initial image ==="
        $SUDO ./mkimg.sh -x "$SETS" -m $MEM -i $IMG
    #fi
)

(
    cd $SMOL
    $SUDO umount mnt 2> /dev/null || true
    $SUDO mkdir -p mnt
    $SUDO mount -o loop $IMG mnt
    $SUDO mkdir -p mnt/usr/pkg
    for f in $(ls $PKGS/*.tgz); do
        echo "=== Install package $f ==="
        $SUDO tar xf $f -C mnt/usr/pkg 2> /dev/null
    done
    $SUDO umount mnt
)

(
    echo "=== Copy repository ==="
    CUR=$(pwd)
    cd $SMOL
    $SUDO umount mnt 2> /dev/null || true
    $SUDO mkdir -p mnt
    $SUDO mount -o loop $IMG mnt
    $SUDO mkdir -p mnt/workspace
    $SUDO cp -r $CUR mnt/workspace/
    $SUDO umount mnt
)

(
    echo "=== Add test script ==="
    CUR=$(pwd)
    cd $SMOL
    $SUDO umount mnt 2> /dev/null || true
    $SUDO mkdir -p mnt
    $SUDO mount -o loop $IMG mnt
    cat | $SUDO tee mnt/run-test.sh <<- EOF
        #!/bin/sh
        set -e
        cd workspace/dice
        /usr/pkg/bin/cmake -S. -Bbuild
        make -C build
        make -C build test
        shutdown -h now
EOF
    $SUDO chmod +x mnt/run-test.sh
    #echo "/run-test.sh" >> $SUDO tee mnt/.cshrc
    $SUDO umount mnt
)

