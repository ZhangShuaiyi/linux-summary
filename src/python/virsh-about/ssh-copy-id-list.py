#!/usr/bin/env python
import subprocess

if __name__ == '__main__':
    targets = ['172.18.211.20', '172.18.211.21', '172.18.211.9', '172.18.211.10', '172.18.211.11',
           '172.18.211.12', '172.18.211.22', '172.18.211.15', '172.18.211.17', '172.18.211.18']
    for target in targets:
        cmd = 'ssh-copy-id root@%s' % target
        print(cmd)
        subprocess.call(cmd, shell=True)
