/**
 * Use udp_table list udp sockets.
 *   udp_table.mask: number of sockets
 *   sk_null_for_each_rcu: go through udp_hslot
 *   sk->sk_num: socket local port
 *   sk->sk_daddr,sk->sk_rcv_saddr: ip address
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/udp.h>

static int __init zudp_init(void) {
    struct udp_hslot *hslot = NULL;
    struct sock *sk;
    struct hlist_nulls_node *node;
    int i = 0;
    pr_debug("zudp_init\n");

    pr_info("udp_table mask:0x%x log:0x%x\n", udp_table.mask, udp_table.log);
    for (i = 0; i < udp_table.mask; i++) {
        hslot = &udp_table.hash[i];
        if (hslot->count > 0) {
            pr_info("hslot[%d] count:%d\n", i, hslot->count);
            sk_nulls_for_each_rcu(sk, node, &hslot->head) {
                /* pr_info("port:%u\n", sk->sk_dport); */
                pr_info("destination port:%u local port:%u daddr:%pI4 saddr:%pI4\n",
                        sk->sk_dport, sk->sk_num, &sk->sk_daddr, &sk->sk_rcv_saddr);
            }
        }
    }
    return 0;
}

static void __exit zudp_exit(void) {
    pr_debug("zudp_exit\n");
}

module_init(zudp_init);
module_exit(zudp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("List udp sockets");
MODULE_VERSION("0.1");
