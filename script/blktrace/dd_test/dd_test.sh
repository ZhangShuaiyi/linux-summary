DEV='/dev/sdb'
echo $DEV

blktrace -d $DEV -w 4 &
sleep 2
dd if=/dev/zero of=$DEV bs=4k count=10
