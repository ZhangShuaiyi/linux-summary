DEV='/dev/sdb'
echo $DEV

blktrace -d $DEV -w 4 &
sleep 2
fio --direct=1 --ioengine=libaio --filename=$DEV --rw=write --name=write --size=80k --bs=4k --iodepth=4
