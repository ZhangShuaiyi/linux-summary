# fio --direct=1 --ioengine=libaio --filename=/dev/sdb --rw=write --name=write --size=40k --bs=8k --iodepth=4
fio --direct=1 --ioengine=libaio --filename=/dev/sdb --rw=write --name=write --size=40k --bs=4k --iodepth=2
