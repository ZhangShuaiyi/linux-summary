#ifndef _TEST_ZPAGE_H
#define _TEST_ZPAGE_H

#include <linux/types.h>
#include <linux/ioctl.h>

struct zpage_region {
    __u32 pid;
    __u64 addr;
};

#define ZPAGE_GET_PID_ADDR _IOW('z', 0x10, struct zpage_region)

#endif