#!/usr/bin/env python
import libvirt
from ctypes import *

libvirt_qemu = cdll.LoadLibrary('/lib64/libvirt-qemu.so')
libvirt_qemu.virDomainQemuAgentCommand.argtypes = [c_void_p, c_char_p, c_int, c_uint]
libvirt_qemu.virDomainQemuAgentCommand.restype = c_char_p

def qga_command(dom, cmd):
    ptr = dom.c_pointer()
    timeout = 1000
    flags = 0
    ret = libvirt_qemu.virDomainQemuAgentCommand(ptr, cmd, timeout, flags)
    return ret

if __name__ == '__main__':
    import sys
    # print(sys.argv)
    if len(sys.argv) < 3:
        print('Usage: %s <domName> <qga_cmd>' % sys.argv[0])
        exit(1)
    conn = libvirt.open('qemu:///system')
    dom = conn.lookupByName(sys.argv[1])
    ret = qga_command(dom, sys.argv[2])
    print(ret)
