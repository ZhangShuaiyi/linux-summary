#!/usr/bin/env python
import subprocess

if __name__ == '__main__':
    index_list = []
    for i in range(4):
        cmd = 'cat /sys/devices/system/cpu/cpu%d/cache/index2/shared_cpu_list' % i
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        ret = p.stdout.readline()
        index_list.append(ret.split()[0])
    print(index_list)
