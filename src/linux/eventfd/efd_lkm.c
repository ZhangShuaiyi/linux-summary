#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/eventfd.h>

static int pid;
static int efd;

int init_module(void) {
    struct task_struct *userspace_task = NULL;
    struct file *efd_file = NULL;
    struct eventfd_ctx *efd_ctx = NULL;

    pr_debug("Received from usersapce pid:%d efd:%d\n", pid, efd);
    userspace_task = pid_task(find_vpid(pid), PIDTYPE_PID);
    pr_debug("Resoveld pointer to the usersapce program's task struct:%p\n", userspace_task);

    pr_debug("Resoveld pointer to the usersapce program's file struct:%p\n", userspace_task->files);

    rcu_read_lock();
    efd_file = fcheck_files(userspace_task->files, efd);
    rcu_read_unlock();

    pr_debug("Resoveld pointer to the usersapce program's eventfs's file struct:%p\n", efd_file);

    efd_ctx = eventfd_ctx_fileget(efd_file);
    if (!efd_ctx) {
        pr_err("eventfd_ctx_fileget() error\n");
        return -1;
    }

    eventfd_signal(efd_ctx, 123);
    eventfd_ctx_put(efd_ctx);
    return 0;
}

void cleanup_module(void) {
    pr_debug("Module exit!\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi");
MODULE_VERSION("0.1");
module_param(pid, int, 0);
module_param(efd, int, 0);
