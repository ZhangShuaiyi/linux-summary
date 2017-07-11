#!/usr/bin/env python
import sys
import libvirt
import libvirt_qemu
import json
import base64
from xml.dom import minidom

def checkDiskName(diskType, diskname):
    diskNodes = diskType.childNodes
    for diskNode in diskNodes:
        if diskNode.nodeName == 'source':
            for attr in diskNode.attributes.keys():
                if diskname in diskNode.attributes[attr].value:
                    return True
    return False

def getPCIFromNode(diskType):
    diskNodes = diskType.childNodes
    for diskNode in diskNodes:
        if diskNode.nodeName == 'address':
            # print([attr for attr in diskNode.attributes.keys()])
            return "%s:%s:%s.%s" % (diskNode.attributes['domain'].value[2:],
                                    diskNode.attributes['bus'].value[2:],
                                    diskNode.attributes['slot'].value[2:],
                                    diskNode.attributes['function'].value[2:])

def getPCIByImageNameFromDomain(dom, imageName):
    raw_xml = dom.XMLDesc(0)
    xml = minidom.parseString(raw_xml)
    diskTypes = xml.getElementsByTagName('disk')
    hasNameDiskTypes = filter(lambda diskType: checkDiskName(diskType, imageName), diskTypes)
    return(map(getPCIFromNode, hasNameDiskTypes))


def getPCI(domname, imageName):
    conn = libvirt.open("qemu:///system")
    dom = None
    domains = conn.listAllDomains(0)
    for domain in domains:
        if domname == domain.name() or domname == str(domain.ID()):
            dom = domain
            break
    else:
        print("Failed to find domain %s" % str(domname))
        conn.close()
        return
    pcis = getPCIByImageNameFromDomain(dom, imageName)
    print(pcis)
    conn.close()

def getDiskNameByPCI(dom, pci):
    # arg = '/dev/disk/by-path/virtio-pci-0000:' + pci
    arg = '/dev/disk/by-path/virtio-pci-' + pci
    cmd = '{"execute": "guest-exec", "arguments": {"path": "readlink", "arg": [\"%s\"], "capture-output": true}}' % arg
    # print(cmd)
    # cmd = '{"execute":"guest-exec", "arguments": {"path":"readlink", "arg": ["/dev/disk/by-path/virtio-pci-0000:00:07.0"], "capture-output": true}}'
    ret = libvirt_qemu.qemuAgentCommand(dom, cmd, -1, 0)
    # print(ret)
    ret_dict = json.loads(ret)
    pid = ret_dict["return"]["pid"]
    # print("pid:%d" % pid)
    cmd = '{"execute":"guest-exec-status", "arguments": {"pid": %d}}' % pid
    ret = libvirt_qemu.qemuAgentCommand(dom, cmd, -1, 0)
    # print(ret)
    ret_dict = json.loads(ret)
    out_data = ret_dict["return"]["out-data"]
    out_data_str = base64.b64decode(out_data)
    disk_name = out_data_str.split("/")[-1].split()[0]
    print(disk_name)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage %s <domain> <imageName>" % sys.argv[0])
        exit(1)
    conn = libvirt.open("qemu:///system")
    domName = sys.argv[1]
    imageName = sys.argv[2]
    dom = None
    domains = conn.listAllDomains(0)
    for domain in domains:
        if domName == domain.name() or domName == str(domain.ID()):
            dom = domain
            break
    else:
        print("Failed to find domain %s" % str(domName))
        conn.close()
        exit(1)
    pcis = getPCIByImageNameFromDomain(dom, imageName)
    map(lambda pci: getDiskNameByPCI(dom, pci), pcis)
    conn.close()
