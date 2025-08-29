#!/bin/sh

set -e

IMG=$(readlink -f $1)
DIR=$(readlink -f $2)

mkdir -p mnt
mount -o loop $IMG mnt

echo "=== Copy repository ==="
cp -r $DIR mnt/workspace

echo "=== Add test script ==="
cat > test.sh <<-EOF
#!/bin/sh
set -ex
rm -f tests.ok tests.log
ls
pwd
cmake -S /workspace -Bbuild || reboot
cmake --build build || reboot
ctest --test-dir build --output-on-failure > tests.log 2>&1 || reboot
touch tests.ok
reboot
EOF

chmod +x test.sh
mv test.sh mnt/

echo "=== Set rc to call test.sh ==="
cat > rc << EOF
#!/bin/sh
export HOME=/root
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/pkg/bin
umask 022
mount -o rw /
sh /test.sh
EOF

cp rc mnt/etc/rc

echo "=== Image ready ==="
umount mnt
