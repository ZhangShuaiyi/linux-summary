#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "Shyi"

static dev_t first_dev_t = 0;
static struct cdev ofcd_cdev;
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
    return 0;
}

static void __exit ofcd_exit(void) {
    cdev_del(&ofcd_cdev);
    unregister_chrdev_region(first_dev_t, 1);
    pr_debug("Our first character driver unregistered\n");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Our first character driver with operations");
MODULE_VERSION("0.2");