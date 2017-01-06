DEV='/dev/sdb'
echo $DEV

blktrace -d $DEV -w 3 &
sleep 1
# fio --direct=1 --ioengine=libaio --filename=$DEV --rw=write --name=write --size=80k --bs=4k --iodepth=4
fio --direct=1 --ioengine=libaio --filename=$DEV --rw=write --name=write --size=4096k --bs=4k --iodepth=1
