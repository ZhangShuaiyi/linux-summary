#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVICE_NAME "hello"

static dev_t foo_dev_t = 0;
static struct cdev *foo_cdev;
static struct class *foo_class;
static int numberOpens = 0;
static char message[256] = {0};

static int dev_open(struct inode *inodep, struct file *filep) {
    numberOpens++;
    pr_debug("Device has been opened %d times\n", numberOpens);
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_debug("Device successfully closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int ret = 0;
    ret = copy_to_user(buffer, message, len);
    if (ret) {
        pr_err("Failed to read\n");
        return -EFAULT;
    }
    return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(message, buffer, len)) {
        pr_err("Failed to write\n");
        return -EFAULT;
    }
    return len;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init hello_init(void) {
    int result;
    result = alloc_chrdev_region(&foo_dev_t, 0, 1, DEVICE_NAME);
    if (result<0) {
        pr_err("Failed to alloc chrdev region\n");
        return result;
    }
    pr_debug("The major number:%d\n", MAJOR(foo_dev_t));
    foo_cdev = cdev_alloc();
    if (!foo_cdev) {
        pr_err("Failed to cdev_alloc\n");
        return -EFAULT;
    }
    cdev_init(foo_cdev, &fops);
    result = cdev_add(foo_cdev, foo_dev_t, 1);
    if (result<0) {
        pr_err("Failed to cdev_add\n");
        return result;
    }
    foo_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (!foo_class) {
        pr_err("Failed to class_create\n");
        return -EFAULT;
    }
    if (!device_create(foo_class, NULL, foo_dev_t, NULL, "hello%d", MINOR(foo_dev_t))) {
        pr_err("Failed to create device\n");
        return -EFAULT;
    }
    return 0;
}

static void __exit hello_exit(void) {
    cdev_del(foo_cdev);
    unregister_chrdev_region(foo_dev_t, 1);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi");
MODULE_DESCRIPTION("A simple linux char device driver");
MODULE_VERSION("0.1");