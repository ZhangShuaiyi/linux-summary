#!/usr/bin/env python
import subprocess

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
    cacheindex2 = ['0,16', '0,16', '1,17', '1,17', '2,18', '2,18', '3,19', '3,19', '4,20', '4,20', '5,21', '5,21', '6,22', '6,22', '7,23', '7,23', '8,24', '8,24', '9,25', '9,25', '10,26', '10,26', '11,27', '11,27', '12,28', '12,28', '13,29', '13,29', '14,30', '14,30', '15,31', '15,31']
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
                print(domName, target)
                for i in range(15):
                    cmd = 'virsh -c qemu+ssh://%s/system vcpupin %s %d %s' % (target, domName, i, cacheindex2[node*16 + i])
                    subprocess.call(cmd, shell=True)
                cmd = 'virsh -c qemu+ssh://%s/system emulatorpin %s %s' % (target, domName, numainfos[node])
                subprocess.call(cmd, shell=True)
                # cmd = 'virsh -c qemu+ssh://%s/system numatune %s --nodeset \'%d\'' % (target, domName, node)
                # cmd = 'virsh -c qemu+ssh://%s/system numatune %s --mode preferred --nodeset \'%d\'' % (target, domName, node)
                # subprocess.call(cmd, shell=True)
                node += 1

