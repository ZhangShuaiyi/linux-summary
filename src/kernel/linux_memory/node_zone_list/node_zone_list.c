#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mmzone.h>

static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *info = NULL;

static int info_show(struct seq_file *m, void *v) {
    int nid = first_online_node;
    pg_data_t *pgdat = NULL;
    struct zone *zone, *node_zones;
    unsigned long flags;

    while (nid != MAX_NUMNODES) {
        pgdat = NODE_DATA(nid);
        seq_printf(m, "node_id:%d nr_zones:%d node_start_pfn:0x%lx node_present_pages:0x%lx\n",
                   pgdat->node_id, pgdat->nr_zones, pgdat->node_start_pfn, pgdat->node_present_pages);
        node_zones = pgdat->node_zones;
        for (zone = node_zones; zone - node_zones < MAX_NR_ZONES; ++zone) {
            if (!populated_zone(zone)) {
                continue;
            }
            spin_lock_irqsave(&zone->lock, flags);
            seq_printf(m, "\t zone: %8s zone_start_pfn:0x%lx present_pages:0x%lx\n",
                       zone->name, zone->zone_start_pfn, zone->present_pages);
            spin_unlock_irqrestore(&zone->lock, flags);
        }
        nid = next_online_node(pgdat->node_id);
    }
    return 0;
}

static int info_open(struct inode *inodep, struct file *file) {
    return single_open(file, info_show, NULL);
}

static const struct file_operations info_fops = {
    .open = info_open,
    .read = seq_read,
};

static int __init node_zone_list_init(void) {
    proc_dir = proc_mkdir("node_zone_list", NULL);
    info = proc_create("info", 0444, proc_dir, &info_fops);
    pr_info("<node_zone_list> insmod\n");
    return 0;
}

static void __exit node_zone_list_exit(void) {
    proc_remove(info);
    proc_remove(proc_dir);
    pr_info("<node_zone_list> remove\n");
}

module_init(node_zone_list_init);
module_exit(node_zone_list_exit);
MODULE_LICENSE("GPL");
