#!/usr/bin/env python
import subprocess

def get_domain_node(target, domName):
    cmd = 'ssh root@%s ps aux | grep %s' % (target, domName)
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        if 'qemu-kvm' in line:
            # print(line)
            pid = line.split()[1]
            # print(pid)
            cmd = 'ssh root@%s numastat -c -p %s' % (target, pid)
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
            for numastat1 in p.stdout.readlines():
                if 'Total ' in numastat1:
                    # print(numastat1)
                    rets = numastat1.split()
                    node0 = rets[1]
                    node1 = rets[2]
                    print(target, domName, node0, node1)
                    node = 0 if int(node0) > int(node1) else 1
                    return node

if __name__ == '__main__':
    # targets = ['172.18.211.20', '172.18.211.21', '172.18.211.9', '172.18.211.10', '172.18.211.11',
    #            '172.18.211.12', '172.18.211.22', '172.18.211.15', '172.18.211.17', '172.18.211.18']
    targets = ['172.18.211.22',
               '172.18.211.20',
               '172.18.211.21',
               '172.18.211.27',
               '172.18.211.9',
               '172.18.211.18',
               '172.18.211.17',
               '172.18.211.15',
               '172.18.211.12',
               '172.18.211.11',
               '172.18.211.10']
    numainfos = ['0-7,16-23', '8-15,24-31']
    for target in targets:
        # cmd = 'ssh root@%s virsh list' % target
        cmd = 'virsh -c qemu+ssh://%s/system list' % target
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
        # print(p.stdout.readlines())
        node = 0
        for line in p.stdout:
            if 'instance' in line:
                # print(line)
                domName = line.split()[1]
                # print(domName, target)
                node = get_domain_node(target, domName)
                print(domName, target, node)
                for i in range(15):
                    cmd = 'virsh -c qemu+ssh://%s/system vcpupin %s %d %s' % (target, domName, i, numainfos[node])
                    subprocess.call(cmd, shell=True)
                cmd = 'virsh -c qemu+ssh://%s/system emulatorpin %s %s' % (target, domName, numainfos[node])
                subprocess.call(cmd, shell=True)
                cmd = 'virsh -c qemu+ssh://%s/system numatune %s --mode preferred --nodeset \'%d\'' % (target, domName, node)
                subprocess.call(cmd, shell=True)


