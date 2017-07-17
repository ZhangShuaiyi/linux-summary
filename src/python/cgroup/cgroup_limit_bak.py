#!/usr/bin/env python
import subprocess
import argparse
from random import randint

if __name__ == '__main__':
    major_id = '1368'
    minor_id = randint(0, 0xffff)
    minor_id = '%04x' % minor_id
    groups = []
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--cpu', type=int, help='cpu max usage')
    parser.add_argument('-d', '--dev', help='network device')
    parser.add_argument('-n', '--net', help='network limit eg:5M')
    parser.add_argument('-m', '--mem', help='memory limit eg:100M')
    parser.add_argument('-p', '--pid', required=True, help='pid to limit')
    args = parser.parse_args()
    # print(args)
    if args.net and not args.dev:
        print('Use network limit -d option needed')
        exit(1)
    if args.cpu:
        cpu_group = 'cpu_%s' % args.cpu
        subprocess.call('cgcreate -g cpu:%s' % cpu_group, shell=True)
        groups.append(cpu_group)
        subprocess.call('cgset -r cpu.cfs_quota_us=%s %s' % (args.cpu * 1000, cpu_group), shell=True)
        subprocess.call('cgclassify -g cpu:%s %s' % (cpu_group, args.pid), shell=True)

    if args.mem:
        mem_group = 'mem_%s' % args.mem
        subprocess.call('cgcreate -g memory:%s' % mem_group, shell=True)
        groups.append(mem_group)
        subprocess.call('cgset -r memory.limit_in_bytes=%s %s' % (args.mem.upper(), mem_group), shell=True)
        subprocess.call('cgset -r memory.swappiness=0 %s' % mem_group, shell=True)
        subprocess.call('cgclassify -g memory:%s %s' % (mem_group, args.pid), shell=True)

    if args.net:
        net_group = 'net_%s' % args.net
        subprocess.call('cgcreate -g net_cls:%s' % net_group, shell=True)
        groups.append(net_group)
        classid = '0x%s%s' % (major_id, minor_id)
        subprocess.call('cgset -r net_cls.classid=%s %s' % (classid, net_group), shell=True)
        subprocess.call('cgclassify -g net_cls:%s %s' % (net_group, args.pid), shell=True)
        # tc command
        subprocess.call('tc qdisc add dev %s root handle %s: htb' % (args.dev, major_id), shell=True)
        subprocess.call('tc class add dev %s parent %s: classid %s:%s htb rate %sbps burst %sb' %
                        (args.dev, major_id, major_id, minor_id, args.net, args.net), shell=True)
        subprocess.call('tc filter add dev %s parent %s: protocol all prio 1 handle 1: cgroup' % (args.dev, major_id), shell=True)
