## linux设备驱动3--字符设备文件操作

### 1. 文件操作
+ 声明file_operations
+ 使用cdev_init初始化cdev
+ 使用cdev_add将cdev添加到VFS

### 2. 自动创建设备文件
+ 使用class_create创建class，创建后可以在/sys/class/下看到该class
+ 使用device_create创建设备文件
+ 在模块卸载时使用device_destroy和class_destroy删除自动创建的设备文件

### 3. 驱动代码
```c
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#ifdef CREATE_NODE
#include <linux/device.h>
#endif

#define DEVICE_NAME "Shyi"

static dev_t first_dev_t = 0;
static struct cdev ofcd_cdev;

#ifdef CREATE_NODE
static struct class *ofcd_class;
#endif

static int number_opens = 0;
static char messages[256] = {0};

static int my_open(struct inode *inodep, struct file *filep) {
    number_opens++;
    pr_debug("ofcd opens (%d) times\n", number_opens);
    return 0;
}

static int my_close(struct inode *inodep, struct file *filep) {
    pr_debug("ofcd closed successfully\n");
    return 0;
}

static ssize_t my_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    pr_debug("ofcd read len:%lu offset:%llu\n", len, *offset);
    if (copy_to_user(buffer, messages, len) != 0) {
        pr_err("ofcd copy_to_user failed\n");
        return -EFAULT;
    }
    return len;
}

static ssize_t my_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    pr_debug("ofcd write len:%lu offset:%llu\n", len, *offset);
    if (copy_from_user(messages, buffer, len) != 0) {
        pr_err("ofcd copy_from_user failed\n");
        return -EFAULT;
    }
    return len;
}

static struct file_operations ofcd_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static int __init ofcd_init(void) {
    pr_debug("Our first character driver registered\n");
    if (alloc_chrdev_region(&first_dev_t, 0, 1, DEVICE_NAME)!=0) {
        pr_err("alloc_chrdev_region failed\n");
        return -1;
    }
    pr_debug("first_dev_t:0x%x\n", first_dev_t);
    pr_debug("<Major, Minor>: <%d, %d>\n", MAJOR(first_dev_t), MINOR(first_dev_t));

    cdev_init(&ofcd_cdev, &ofcd_fops);
    if (cdev_add(&ofcd_cdev, first_dev_t, 1)!=0) {
        pr_err("ofcd cdev_add failed\n");
        return -1;
    }
#ifdef CREATE_NODE
    ofcd_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (!ofcd_class) {
        pr_err("ofcd class_create failed\n");
        return -1;
    }
    if (!device_create(ofcd_class, NULL, first_dev_t, NULL, "ofcd%d", MINOR(first_dev_t))) {
        pr_err("ofcd device_create failed\n");
        return -1;
    }
#endif
    return 0;
}

static void __exit ofcd_exit(void) {
    cdev_del(&ofcd_cdev);
#ifdef CREATE_NODE
    device_destroy(ofcd_class, first_dev_t);
    class_destroy(ofcd_class);
#endif
    unregister_chrdev_region(first_dev_t, 1);
    pr_debug("Our first character driver unregistered\n");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Our first character driver with operations");
MODULE_VERSION("0.2");
```
Makefile文件
```
obj-m := ofcd.o
CFLAGS_ofcd.o += -DDEBUG
CFLAGS_ofcd.o += -DCREATE_NODE

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

### 4. 测试
```sh
$ sudo insmod ofcd.ko 
$ ls -l /dev/ofcd0 
crw------- 1 root root 242, 0 10月 22 16:40 /dev/ofcd0
$ ls -l /sys/class/Shyi/
total 0
lrwxrwxrwx 1 root root 0 10月 22 16:41 ofcd0 -> ../../devices/virtual/Shyi/ofcd0
```