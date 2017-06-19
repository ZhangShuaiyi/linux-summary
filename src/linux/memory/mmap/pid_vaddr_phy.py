#!/usr/bin/env python
import os
import sys
import struct

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage:%s <pid> <vaddr>" % sys.argv[0])
        exit(1)
    pth = '/proc/%s/pagemap' % sys.argv[1]
    vaddr = int(sys.argv[2], 16)
    with open(pth, 'rb') as f:
        offset = vaddr / os.sysconf("SC_PAGE_SIZE") * 8
        f.seek(offset)
        d = f.read(8)
        phy = struct.unpack("L", d)
        print("%s vaddr:0x%x phy:0x%x" % (sys.argv[1], vaddr, phy[0]))
