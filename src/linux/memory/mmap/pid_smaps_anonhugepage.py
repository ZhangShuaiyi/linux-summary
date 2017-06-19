#!/usr/bin/env python
import glob
import argparse
import os

def get_sum(smaps_list):
    hugepages_sum = 0
    for smaps in smaps_list:
        with open(smaps, 'r') as f:
            for line in f:
                if 'AnonHugePages' in line:
                    num = int(line.split()[1])
                    hugepages_sum += num
    print("%d kB" % hugepages_sum)

def list_hugepages(smaps_list):
    hugepages_sum = 0
    for smaps in smaps_list:
        with open(smaps, 'r') as f:
            lines = f.readlines()
            for i, line in enumerate(lines):
                if 'AnonHugePages' in line:
                    num = int(line.split()[1])
                    if num > 0:
                        hugepages_sum += num
                        print(smaps)
                        cmdline = os.path.join(os.path.dirname(smaps), 'cmdline')
                        with open(cmdline, 'r') as fi:
                            print(fi.read())
                        print(lines[i - 10])
                        print(line)
                        print("")
    print("Total %d kB" % hugepages_sum)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', '--list', action='store_true')
    args = parser.parse_args()
    smaps_list = glob.glob('/proc/*/smaps')
    if args.list:
        list_hugepages(smaps_list)
    else:
        get_sum(smaps_list)

