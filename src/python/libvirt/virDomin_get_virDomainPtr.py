#!/usr/bin/env python
import sys
import libvirt
import ctypes

class virDomainPtrObject(ctypes.Structure):
    _fields_ = [
        ("ob_refcnt", ctypes.c_size_t),
        ("ob_type", ctypes.c_void_p),
        ("ob_ptr", ctypes.c_void_p)
    ]

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: %s <domName>' % sys.argv[0])
        exit(1)
    print('domName:%s' % sys.argv[1])
    conn = libvirt.open('qemu:///system')
    dom = conn.lookupByName(sys.argv[1])
    # virDomainPtr object
    ptr = dom.c_pointer()
    print('virDomainPtr object from dom.c_pointer: %s' % hex(ptr))

    # get virDomainPtr object by ctypes
    obj = virDomainPtrObject.from_address(id(dom._o))
    ptr2 = obj.ob_ptr
    print('virDomainPtr object by ctypes: %s' % hex(ptr2))

    

