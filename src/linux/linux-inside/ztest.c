#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/fdtable.h>

#include "ztest_ioctl.h"

#define FILE_NAME_LEN 256
#define MINOR_CNT 1
#define DEVICE_NAME "ztest"

static dev_t ztest_dev_t;
static struct cdev ztest_cdev;
static struct class *ztest_class;

static int my_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static int my_close(struct inode *inodep, struct file *filep) {
    return 0;
}

static long my_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    struct task_struct *task;
    struct files_struct *current_files;
    struct fdtable *file_table;
    struct path file_path;
    int i = 0;
    char *cwd;
    char buf[FILE_NAME_LEN] = {0};

    switch (cmd) {
    case ZTEST_LIST_PROCESS:
        for_each_process(task)
            pr_info("%s [%d] state:%lu\n", task->comm, task->pid, task->state);
        break;
    case ZTEST_LIST_FILES:
        if (arg == 0) {
            task = current;
        } else {
            task = pid_task(find_vpid(arg), PIDTYPE_PID);
        }
        current_files = task->files;
        file_table = files_fdtable(current_files);
        pr_debug("%s [%d] max_fds:%u\n", task->comm, task->pid, file_table->max_fds);
#if 1
        for (i=0; i<file_table->max_fds; i++) {
            if (!file_table->fd[i]) {
                continue;
            }
            file_path = file_table->fd[i]->f_path;
            cwd = d_path(&file_path, buf, FILE_NAME_LEN);
            pr_info("fd %d %s\n", i, cwd);
        }
#else
        while (file_table->fd[i] != NULL) {
            file_path = file_table->fd[i]->f_path;
            cwd = d_path(&file_path, buf, FILE_NAME_LEN);
            pr_info("fd %d %s\n", i, cwd);
            i++;
        }
#endif
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

static struct file_operations ztest_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};

static int __init ztest_init(void) {
    pr_debug("ztest registered\n");

    if (alloc_chrdev_region(&ztest_dev_t, 0, MINOR_CNT, DEVICE_NAME) != 0) {
        pr_err("ztest alloc_chrdev_region failed\n");
        return -1;
    }
    pr_debug("ztest <major, minor>: <%d, %d>\n", MAJOR(ztest_dev_t), MINOR(ztest_dev_t));

    cdev_init(&ztest_cdev, &ztest_fops);
    if (cdev_add(&ztest_cdev, ztest_dev_t, MINOR_CNT) != 0) {
        pr_err("ztest cdev_add failed\n");
        return -1;
    }

    ztest_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (!ztest_class) {
        pr_err("ztest class_create failed\n");
        return -1;
    }
    if (!device_create(ztest_class, NULL, ztest_dev_t, NULL, "ztest%d", MINOR(ztest_dev_t))) {
        pr_err("ztest device_create failed\n");
        return -1;
    }
    return 0;
}

static void __exit ztest_exit(void) {
    cdev_del(&ztest_cdev);
    device_destroy(ztest_class, ztest_dev_t);
    class_destroy(ztest_class);
    unregister_chrdev_region(ztest_dev_t, MINOR_CNT);
    pr_debug("ztest unregistered\n");
}

module_init(ztest_init);
module_exit(ztest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Linux list info driver");
MODULE_VERSION("0.1");
