#ifndef _TEST_ZPAGE_H
#define _TEST_ZPAGE_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define MMAP_LEN 4096

struct zpage_region {
    __u32 pid;
    __u64 addr;
};

#define ZPAGE_GET_PID_ADDR _IOW('z', 0x10, struct zpage_region)
#define ZPAGE_GET_MM_INFO  _IO('z', 0x11)
#define ZPAGE_GET_VMA_INFO _IO('z', 0x12)

#endif