obj-m := zlist.o
CFLAGS_zlist.o += -DDEBUG

obj-m += zlist2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean