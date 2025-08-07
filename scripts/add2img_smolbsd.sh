#!/bin/sh
# depends on: git, make, curl, uuid-runtime, rsync, bsdtar (libarchive-tools)

set -e

DATA=$1
DATA=$(readlink -f $DATA)
NAME=dice-smol
SMOL=$DATA/smolBSD
MEM=2048
IMG=$NAME.img
SUDO=sudo

(
    CUR=$(pwd)
    cd $SMOL
    $SUDO mount -o loop $IMG mnt

    echo "=== Copy repository ==="
    $SUDO cp -r $CUR mnt/dice

    echo "=== Add test script ==="
    cat > test.sh << EOF
#!/bin/sh
set -ex
rm -f tests.ok tests.log
ls
pwd
cmake -S dice -Bbuild || reboot
cmake --build build || reboot
ctest --test-dir build --output-on-failure > tests.log 2>&1 || reboot
touch tests.ok
reboot
EOF
    chmod +x test.sh
    $SUDO mv test.sh mnt/

    echo "=== Set rc to call test.sh ==="
    cat > rc << EOF
#!/bin/sh
export HOME=/root
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/pkg/bin
umask 022
mount -o rw /
sh /test.sh
EOF
    $SUDO cp rc mnt/etc/rc

    echo "=== Image ready ==="
    $SUDO umount mnt
)
