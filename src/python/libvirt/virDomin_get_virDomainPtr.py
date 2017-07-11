#!/usr/bin/env python
import sys
import libvirt
import ctypes

class virDomainPtrObject(ctypes.Struc)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: %s <domName>' % sys.argv[0])
        exit(1)
    conn = libvirt.open('qemu:///system')
    dom = conn.lookupByName(sys.argv[1])
    # virDomainPtr object
    ptr = dom.c_pointer()
    print('virDomainPtr object from dom.c_pointer: %s' % hex(ptr))

    # get virDomainPtr object by ctypes
    

