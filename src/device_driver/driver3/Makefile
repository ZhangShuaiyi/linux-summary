obj-m := ofcd.o
CFLAGS_ofcd.o += -DDEBUG
CFLAGS_ofcd.o += -DCREATE_NODE

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean