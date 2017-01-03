DEV='/dev/sdb'
echo $DEV

blktrace -d $DEV -w 2 &
sleep 1
fio --direct=1 --ioengine=libaio --filename=$DEV --rw=write --name=write --size=40k --bs=4k --iodepth=2
