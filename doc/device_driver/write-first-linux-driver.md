## linux设备驱动1--编写第一个驱动程序

### 1. 第一个linux驱动程序
编写ofd.c
```c
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>

static int __init ofd_init(void) {
    pr_debug("Our first driver registered\n");
    return 0;
}

static void __exit ofd_exit(void) {
    pr_debug("Our first driver unregistered\n");
}

module_init(ofd_init);
module_exit(ofd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Our first driver");
MODULE_VERSION("0.1");
```
+ module_init和module_exit是宏定义，当模块加载时执行ofd_init函数，当模块卸载时执行ofd_exit函数。
+ pr_debug可输出信息，当编译时打开DEBUG时，可使用dmesg查看输出。

### 2. 编译驱动程序
编写Makefile
```
obj-m := ofd.o
CFLAGS_ofd.o += -DDEBUG

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```
+ "obj-m := ofd.o": 编译成ofd.ko
+ "CFLAGS_ofd.o += -DDEBUG": 打开pr_debug输出
+ 使用make进行编译

### 3. 测试驱动程序
+ modinfo查看驱动信息
```sh
$ modinfo ofd.ko
filename:       /home/zsy/Workspace/code/linux-summary/src/device_driver/driver1/ofd.ko
version:        0.1
description:    Our first driver
author:         ZhangShuaiyi zhang_syi@163.com
license:        GPL
srcversion:     BF16071727CA41A5960BB1F
depends:        
vermagic:       4.4.0-41-generic SMP mod_unload modversions
```
+ insmod加载驱动模块，使用dmesg查看输出信息
```
$ sudo insmod ofd.ko
$ dmesg | tail
```
+ 加载驱动后在/sys/module/下出现ofd目录
```
$ ls /sys/module/ofd/
coresize  holders  initsize  initstate  notes  refcnt  sections  srcversion  taint  uevent  version
$ cat /sys/module/ofd/version 
0.1
```
+ 使用rmmod卸载驱动模块
```sh
$ sudo rmmod ofd 
$ dmesg | tail
```
