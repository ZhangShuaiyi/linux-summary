## linux设备驱动2--字符设备驱动

### 1. linux主次设备号(Major and minor numbers)
应用程序和设备文件通过设备文件的名称进行联系，设备文件和设备驱动通过设备文件的编号进行联系，设备文件编号是Major and minor numbers。
+ dev_t在include/linux/types.h中声明，包括了major和minor设备号
+ MAJOR(dev_t dev)在linux/kdev_t.h中声明，获取设备dev的major number。
+ MINOR(dev_t dev)在linux/kdev_t.h中声明，获取设备dev的minor number。
+ alloc_chrdev_region在linux/fs.h中声明，实现在fs/char_dev.c中，用来申请字符设备。

### 2. 注册字符设备
```c
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define DEVICE_NAME "Shyi"

static dev_t first_dev_t = 0;

static int __init ofcd_init(void) {
    pr_debug("Our first character driver registered\n");
    if (alloc_chrdev_region(&first_dev_t, 0, 1, DEVICE_NAME)!=0) {
        pr_err("alloc_chrdev_region failed\n");
        return -1;
    }
    pr_debug("first_dev_t:0x%x\n", first_dev_t);
    pr_debug("<Major, Minor>: <%d, %d>\n", MAJOR(first_dev_t), MINOR(first_dev_t));
    return 0;
}

static void __exit ofcd_exit(void) {
    unregister_chrdev_region(first_dev_t, 1);
    pr_debug("Our first character driver unregistered\n");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Our first character driver");
MODULE_VERSION("0.1");
```

### 3. 测试
+ 查看主次设备号，模块加载后可查看/proc/devices
+ 使用mknod创建节点
```sh
$ cat /proc/devices  | grep Shyi
242 Shyi
$ sudo mknod /dev/ofcd0 c 242 0
$ ls -l /dev/ofcd0 
crw-r--r-- 1 root root 242, 0 10月 22 14:33 /dev/ofcd0
$ cat /dev/ofcd0 
cat: /dev/ofcd0: No such device or address
```
+ 删除node
```
$ sudo rmmod ofcd 
$ sudo rm /dev/ofcd0
```

参考
+ [Device Drivers, Part 4: Linux Character Drivers](http://opensourceforu.com/2011/02/linux-character-drivers/)