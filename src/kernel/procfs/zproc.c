#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define DIR "zproc"
#define MM_NAME "mminfo"
#define MSG_LEN 16

static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *mminfo = NULL;

static long pid = 0;

static int mminfo_show(struct seq_file *m, void *v) {
    struct task_struct *task = NULL;
    pr_debug("<zproc> mminfo_show pid:%ld\n", pid);
    if (!pid) {
        seq_printf(m, "pid:%ld\n", pid);
        return 0;
    }

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        seq_printf(m, "pid:%ld find task error\n", pid);
        return 0;
    }
    seq_printf(m, "pid:%ld comm:%s\n", pid, task->comm);
    return 0;
}

static int mminfo_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, mminfo_show, NULL);
}

static ssize_t mminfo_write(struct file *filep, const char __user *buf, size_t count, loff_t *pos) {
    int len = 0;
    char msg[MSG_LEN] = {0};

    len = MSG_LEN > count ? count : MSG_LEN;
    if (copy_from_user(msg, buf, len)) {
        return -EFAULT;
    }
    pr_debug("<zproc> mminfo_write:%s\n", msg);
    if (kstrtol(msg, 10, &pid)) {
        pr_warn("<zproc> kstrtol error, msg:%s\n", msg);
    }
    return count;
}

static const struct file_operations mminfo_proc_fops = {
    .open = mminfo_proc_open,
    .read = seq_read,
    .write = mminfo_write,
};

static int __init zproc_init(void) {
    proc_dir = proc_mkdir(DIR, NULL);
    mminfo = proc_create(MM_NAME, 0, proc_dir, &mminfo_proc_fops);
    pr_info("<zproc> test proc create\n");
    return 0;
}

static void __exit zproc_exit(void) {
    proc_remove(mminfo);
    proc_remove(proc_dir);
    pr_info("<zproc> test proc remove\n");
}

module_init(zproc_init);
module_exit(zproc_exit);
MODULE_LICENSE("GPL");
