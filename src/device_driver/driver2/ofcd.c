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