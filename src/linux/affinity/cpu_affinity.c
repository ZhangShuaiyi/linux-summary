#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define DIR "cpu_affinity"
#define GET "get"
#define MSGLEN 32

static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *proc_get = NULL;

static long pid = 0;

static int proc_get_show(struct seq_file *m, void *v) {
    struct task_struct *task = NULL;
    pr_debug("<cpu_affinity> proc_get_show pid:%ld\n", pid);
    if (!pid) {
        seq_printf(m, "pid:%ld\n", pid);
        return 0;
    }

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        seq_printf(m, "pid:%ld task is NULL\n", pid);
        return 0;
    }
    seq_printf(m, "pid:%ld nr_cpus_allowed:%d cpus_allowed:0x%lx\n",
               pid, task->nr_cpus_allowed, *task->cpus_allowed.bits);
    return 0;
}

static int proc_get_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_get_show, NULL);
}

static ssize_t proc_get_write(struct file *filep, const char __user *buf, size_t count, loff_t *pos) {
    int len = 0;
    char msg[MSGLEN] = {0};

    len = MSGLEN > count ? count : MSGLEN;
    if (copy_from_user(msg, buf, len)) {
        return -EFAULT;
    }
    pr_debug("<cpu_affinity> proc_get_write:%s\n", msg);
    if (kstrtol(msg, 10, &pid)) {
        pr_warn("<cpu_affinity> kstrtol error msg:%s\n", msg);
    }
    return count;
}

static const struct file_operations proc_get_fops = {
    .open = proc_get_open,
    .read = seq_read,
    .write = proc_get_write,
};

static int __init affinity_init(void) {
    proc_dir = proc_mkdir(DIR, NULL);
    proc_get = proc_create(GET, 0666, proc_dir, &proc_get_fops);
    pr_info("<cpu_affinity> loaded\n");
    return 0;
}

static void __exit affinity_exit(void) {
    proc_remove(proc_get);
    proc_remove(proc_dir);
    pr_info("<cpu_affinity> remove\n");
}

module_init(affinity_init);
module_exit(affinity_exit);
MODULE_LICENSE("GPL");

