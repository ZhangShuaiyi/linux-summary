DEV='/dev/sdb'
echo $DEV

blktrace -d $DEV -w 3 &
sleep 1
# dd if=/dev/zero of=$DEV bs=4k count=10
dd if=/dev/zero of=$DEV bs=4k
