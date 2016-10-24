obj-m := ztest.o
CFLAGS_ztest.o += -DDEBUG

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean