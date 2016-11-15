/**
 * Test kernel THIS_MODULE and __this_module.
 * List module by list_for_each_entry(mod, THIS_MODULE->list.prev, list).
 * Use mod->target_list get module's depends.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>

static int __init ztest_init(void) {
    int i = 1;
    struct module *mod;
    struct module_use *use;

    pr_info("zmodule init %p\n", ztest_init);
    pr_info("THIS_MODULE:%p __this_module:%p module name:%s init:%p\n",
            THIS_MODULE, &__this_module, THIS_MODULE->name, THIS_MODULE->init);

    /**
     * struct module {
     *     ...
     *     struct list_head list;
     *     ...
     *     int (*init)(void);
     *     ...
     * }
     */
    list_for_each_entry(mod, THIS_MODULE->list.prev, list) {
        pr_info("mod name:%s depends::", mod->name);
        /* get module depends by target_list */
        list_for_each_entry(use, &mod->target_list, target_list) {
            struct module *i = use->target;
            pr_info(" %s", i->name);
        }
        pr_info("\n");
        i++;
    }
    pr_info("module num:%d\n", i);
    return 0;
}

static void __exit ztest_exit(void) {
    pr_info("zmodule exit\n");
}

module_init(ztest_init);
module_exit(ztest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");