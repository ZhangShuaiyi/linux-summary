#!/usr/bin/env python
import subprocess

def cat_ssh(target):
    cmd = 'ssh root@%s cat /sys/kernel/mm/ksm/pages_shared' % target
    subprocess.call(cmd, shell=True)
    cmd = 'ssh root@%s cat /sys/kernel/mm/ksm/pages_sharing' % target
    subprocess.call(cmd, shell=True)

def shutdown_all(target):
    cmd = 'virsh -c qemu+ssh://%s/system list' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        if 'instance' in line:
            domName = line.split()[1]
            cmd = 'virsh -c qemu+ssh://%s/system shutdown %s' % (target, domName)
            print(cmd)
            subprocess.call(cmd, shell=True)

def list_all(target):
    cmd = 'virsh -c qemu+ssh://%s/system list' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        print(line)

def install_numa(target):
    cmd = 'ssh root@%s yum install -y numactl' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        print(line)

def list_numa_bak(target):
    cmd = 'ssh root@%s numastat -c qemu-kvm' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        print(line)

def list_numa(target):
    cmd = 'ssh root@%s pgrep qemu-kvm' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    pids = p.stdout.readlines()
    pids = [pid.split()[0] for pid in pids]
    print(pids)

def list_numatune(target):
    cmd = 'virsh -c qemu+ssh://%s/system list' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        if 'instance' in line:
            domName = line.split()[1]
            print(domName, target)
            cmd = 'virsh -c qemu+ssh://%s/system numatune %s' % (target, domName)
            subprocess.call(cmd, shell = True)

def reset_vcpupin(target):
    cmd = 'virsh -c qemu+ssh://%s/system list' % target
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    for line in p.stdout:
        if 'instance' in line:
            domName = line.split()[1]
            print(domName, target)
            for i in range(15):
                cmd = 'virsh -c qemu+ssh://%s/system vcpupin %s %d 0-31' % (target, domName, i)
                subprocess.call(cmd, shell=True)
            cmd = 'virsh -c qemu+ssh://%s/system emulatorpin %s 0-31' % (target, domName)
            subprocess.call(cmd, shell=True)

def set_ksm_thp(target):
    in_commands = [
        'systemctl stop ksm.service',
        'systemctl stop ksmtuned.service',
        'echo 0 > /sys/kernel/mm/ksm/pages_shared',
        'echo 0 > /sys/kernel/mm/ksm/pages_sharing',
        'echo 0 > /sys/kernel/mm/ksm/merge_across_nodes',
        'echo always > /sys/kernel/mm/transparent_hugepage/enabled',
        'echo never > /sys/kernel/mm/transparent_hugepage/defrag',
        'echo 0 > /sys/kernel/mm/transparent_hugepage/khugepaged/defrag'
    ]
    print(target)
    for in_com in in_commands:
        cmd = 'ssh root@%s "%s"' % (target, in_com)
        print(cmd)
        subprocess.call(cmd, shell=True)

if __name__ == '__main__':
    # targets = ['172.18.211.20', '172.18.211.21', '172.18.211.9', '172.18.211.10', '172.18.211.11',
    #        '172.18.211.12', '172.18.211.22', '172.18.211.15', '172.18.211.17', '172.18.211.18']
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
    for target in targets:
        # cat_ssh(target)
        # shutdown_all(target)
        # list_all(target)
        # list_numa(target)
        # reset_vcpupin(target)
        # list_numatune(target)
        set_ksm_thp(target)
