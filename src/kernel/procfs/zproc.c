#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <asm/uaccess.h>
#include <linux/mmzone.h>

#define DIR "zproc"
#define MM_NAME "mminfo"
#define PHYS_VALUE "physvalue"
#define BUDDY_NAME "node0buddy"
#define MSG_LEN 32

static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *mminfo = NULL;
static struct proc_dir_entry *phys_value = NULL;
static struct proc_dir_entry *buddy_proc = NULL;

static long pid = 0;
static long phys = 0;

static int mminfo_show(struct seq_file *m, void *v) {
    struct task_struct *task = NULL;
    struct mm_struct *mm = NULL;
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
    mm = task->active_mm;
    seq_printf(m, "map_count:%d pgd:%p pgd pa:0x%llx pgd value:0x%lx\n",
               mm->map_count, mm->pgd, virt_to_phys(mm->pgd), *(unsigned long *)mm->pgd);
    seq_printf(m, "start_brk:0x%lx brk:0x%lx start_stack:0x%lx\n",
               mm->start_brk, mm->brk, mm->start_stack);
    seq_printf(m, "total_vm:0x%lx data_vm:0x%lx\n", mm->total_vm, mm->data_vm);
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

static int phys_value_show(struct seq_file *m, void *v) {
    struct page *pg = NULL;
    void *from;
    unsigned long page_num, page_off;

    page_num = (unsigned long)phys / PAGE_SIZE;
    page_off = (unsigned long)phys % PAGE_SIZE;

    pg = pfn_to_page(page_num);
    if (!pg) {
        seq_printf(m, "0x%lx pfn_to_page error\n", phys);
        return 0;
    }
    from = kmap(pg) + page_off;
    seq_printf(m, "0x%lx:0x%lx\n", phys, *(unsigned long *)from);
    kunmap(pg);
    return 0;
}

static int phys_value_open(struct inode *inodep, struct file *filep) {
    return single_open(filep, phys_value_show, NULL);
}

static ssize_t phys_value_write(struct file *filep, const char __user *buf, size_t count, loff_t *pos) {
    int len = 0;
    char msg[MSG_LEN] = {0};

    len = MSG_LEN > count ? count : MSG_LEN;
    if (copy_from_user(msg, buf, len)) {
        return -EFAULT;
    }
    if (kstrtol(msg, 16, &phys)) {
        pr_warn("<zproc> phys_value_write %s error\n", msg);
    }
    return count;
}

static const struct file_operations phys_proc_fops = {
    .open = phys_value_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .write = phys_value_write,
};

static int node0_show(struct seq_file *m, void *v) {
    pg_data_t *pgdat = NULL;
    struct zone *zone, *node_zones;
    int order;
    unsigned long flags;

    /* pgdat = first_online_pgdat(); */
    pgdat = NODE_DATA(first_online_node);
    node_zones = pgdat->node_zones;
    for (zone = node_zones; zone - node_zones < MAX_NR_ZONES; ++zone) {
        if (!populated_zone(zone)) {
            continue;
        }
        spin_lock_irqsave(&zone->lock, flags);
        seq_printf(m, "Node %d, zone %8s ", pgdat->node_id, zone->name);
        for (order = 0; order < MAX_ORDER; ++order) {
            seq_printf(m, "%6lu ", zone->free_area[order].nr_free);
        }
        seq_putc(m, '\n');
        spin_unlock_irqrestore(&zone->lock, flags);
    }
    return 0;
}

static int buddy_proc_open(struct inode *inodep, struct file *filep) {
    return single_open(filep, node0_show, NULL);
}

static const struct file_operations buddy_proc_fops = {
    .open = buddy_proc_open,
    .read = seq_read,
};

static int __init zproc_init(void) {
    proc_dir = proc_mkdir(DIR, NULL);
    mminfo = proc_create(MM_NAME, 0666, proc_dir, &mminfo_proc_fops);
    phys_value = proc_create(PHYS_VALUE, 0666, proc_dir, &phys_proc_fops);
    buddy_proc = proc_create(BUDDY_NAME, 0444, proc_dir, &buddy_proc_fops);
    pr_info("<zproc> test proc create\n");
    return 0;
}

static void __exit zproc_exit(void) {
    proc_remove(mminfo);
    proc_remove(phys_value);
    proc_remove(buddy_proc);
    proc_remove(proc_dir);
    pr_info("<zproc> test proc remove\n");
}

module_init(zproc_init);
module_exit(zproc_exit);
MODULE_LICENSE("GPL");
