#!/usr/bin/env python
import argparse

PREFIX = 'ctpid'

def cgroup_cpu(args):
    print(args)

def cgroup_mem(args):
    print(args)

def cgroup_blkio(args):
    print(args)

def cgroup_net(args):
    print(args)

def cgroup_clear(args):
    print(args)

def get_argparser():
    parser = argparse.ArgumentParser()
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('-C', '--clear', action='store_true', default=False, help='Clear pid target cgroup')
    parent_parser.add_argument('-p', '--pid', required=True, help='Process pid to limit')
    subparsers = parser.add_subparsers()

    parser_cpu = subparsers.add_parser('cpu', parents=[parent_parser], help='cpu limit')
    parser_cpu.add_argument('-u', '--usage', type=int, help='Max cpu usage')
    parser_cpu.set_defaults(func=cgroup_cpu)

    parser_mem = subparsers.add_parser('mem', parents=[parent_parser], help='memory limit')
    parser_mem.add_argument('-m', '--mem', help='Max mem usage, eg:200M')
    parser_mem.set_defaults(func=cgroup_mem)

    parser_blkio = subparsers.add_parser('blkio', parents=[parent_parser], help='block device io limit')
    parser_blkio.add_argument('-d', '--dev', help='block device to limit, eg:8:1')
    parser_blkio.add_argument('-w', '--write', help='Max write')
    parser_blkio.add_argument('-r', '--read', help='Max read')
    parser_blkio.set_defaults(func=cgroup_blkio)

    parser_net = subparsers.add_parser('net', parents=[parent_parser], help='net io limit')
    parser_net.add_argument('-d', '--dev', help='network interface to limit, eg:eth0')
    parser_net.set_defaults(func=cgroup_net)


    return parser

if __name__ == '__main__':
    parser = get_argparser()
    args = parser.parse_args()
    args.func(args)
