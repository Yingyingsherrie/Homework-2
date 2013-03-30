#!/bin/sh

# bi-directional test

echo -n "bi-directional test ... "
rm -f my_*.txt >/dev/null 2>&1
(echo a; echo b; echo c) | ./chat a b | grep -v CONNECTED > my_a2b.txt &
(echo c; echo d; echo e) | ./chat b a | grep -v CONNECTED > my_b2a.txt &
wait
if ! (diff my_a2b.txt out_a2b.txt && diff my_b2a.txt out_b2a.txt); then
    echo failed
    exit 1
fi
rm -f my_?2?.txt
rm -f a2b b2a
echo passed

# uni-directional test with random data

echo -n "large random input test ... "
dd if=/dev/urandom bs=256k count=80 of=ii.txt 2>/dev/null
./chat b a < /dev/null > oo.txt &
cat ii.txt | ./chat a b > /dev/null
wait
if ! (cmp -l ii.txt oo.txt); then
    echo failed
    exit 1
fi
rm -f ii.txt oo.txt
echo passed

rm -f a2b b2a

exit 0
