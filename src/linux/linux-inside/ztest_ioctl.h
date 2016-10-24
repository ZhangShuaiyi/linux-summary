#ifndef ZTEST_IOCTL_H
#define ZTEST_IOCTL_H
#include <linux/ioctl.h>

#define ZTESTIO 0x66

#define ZTEST_LIST_PROCESS  _IO(ZTESTIO, 0)

#endif