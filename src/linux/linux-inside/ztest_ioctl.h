#ifndef ZTEST_IOCTL_H
#define ZTEST_IOCTL_H
#include <linux/ioctl.h>

#define ZTESTIO 0x66

#define ZTEST_LIST_PROCESS  _IO(ZTESTIO, 0x00)
#define ZTEST_LIST_FILES    _IO(ZTESTIO, 0x01)
#define ZTEST_GET_THREAD    _IO(ZTESTIO, 0x02)

#endif