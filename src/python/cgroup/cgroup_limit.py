#!/usr/bin/env python
#coding=utf-8
import sys
import os
import re
import argparse
import subprocess

PREFIX = 'ctpid'

def get_spids_by_pid(pid):
    spids = []
    task_path = '/proc/%s/task' % pid
    if os.path.isdir(task_path):
        spids = os.listdir(task_path)
    else:
        spids = [pid]
    return spids

def get_cgroup_name(pid):
    return 'ctpid%s' % pid

def check_cgroup_exist(controller, name):
    cmd = 'lscgroup -g %s:%s' % (controller, name)
    out = subprocess.check_output(cmd, shell=True)
    return True if len(out) > 0 else False

def lscgroup_filter(name=None):
    cmd = 'lscgroup'
    out = subprocess.check_output(cmd, shell=True)
    out = out.splitlines()
    if name is None:
        return out
    return filter(lambda x:name in x, out)

def cgcreate(controller, name):
    if check_cgroup_exist(controller, name):
        cgdelete(controller, name)
    cmd = 'cgcreate -g %s:%s' % (controller, name)
    subprocess.call(cmd, shell=True)

def cgdelete_name(group_name):
    cmd = 'cgdelete -r -g %s' % group_name
    subprocess.call(cmd, shell=True)

def cgdelete(controller, name):
    cgdelete_name('%s:%s' % (controller, name))

def cgget_key(key, name):
    cmd = 'cgget -r %s %s' % (key, name)
    popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    popen.wait()
    stderr = popen.stderr.readlines()
    stdout = popen.stdout.readlines()
    assert len(stderr) == 0, stderr
    return stdout

def cgset(key, value, name):
    cgget_key(key, name)
    cmd = 'cgset -r %s=%s %s' % (key, value, name)
    subprocess.call(cmd, shell=True)

def cgclassify(controller, name, pid):
    cmd = 'cgclassify -g %s:%s %s' % (controller, name, pid)
    subprocess.call(cmd, shell=True)

def add_args(*args, **kwargs):
    def _decorator(func):
        if not hasattr(func, 'arguments'):
            func.arguments = []
        if (args, kwargs) not in func.arguments:
            func.arguments.insert(0, (args, kwargs))
        return func
    return _decorator

@add_args('-s', '--spid', action='store_true', default=False, help='set for spid')
@add_args('-u', '--usage', type=int, help='Max cpu usage')
def do_cgroup_cpu(args):
    """
    cpu limit
    """
    controller = 'cpu'
    name = get_cgroup_name(args.pid)
    if args.clear:
        cgdelete(controller, name)
        return
    if args.usage:
        # 使用cpu.cfs_quota_us设置cpu时间，设置为-1时为不限制，通过和cpu.cfs_period_us比值计算比例
        # 限制对象为线程
        cgcreate(controller, name)
        usage = args.usage * 1000
        cgset('cpu.cfs_quota_us', usage, name)
        if args.spid:
            cgclassify(controller, name, args.pid)
        else:
            spids = get_spids_by_pid(args.pid)
            for spid in spids:
                cgclassify(controller, name, spid)


@add_args('-m', '--mem', help='Max mem usage, eg:200M')
def do_cgroup_mem(args):
    """
    memory limit
    """
    controller = 'memory'
    name = get_cgroup_name(args.pid)
    if args.clear:
        cgdelete(controller, name)
        return
    if args.mem:
        cgcreate(controller, name)
        mem = args.mem
        assert re.match('^[0-9]+[kKmMgG]?$', mem) is not None, '-m <bytes to limit>, can use K,M,G'
        #
        cgset('memory.limit_in_bytes', mem, name)
        cgset('memory.memsw.limit_in_bytes', mem, name)
        cgclassify(controller, name, args.pid)


@add_args('-d', '--dev', help='block device to limit, eg:8:1')
@add_args('-w', '--write', help='Max write')
@add_args('-r', '--read', help='Max read')
def do_cgroup_blkio(args):
    """
    block device io limit
    """
    print(args)

@add_args('-d', '--dev', help='network interface to limit, eg:eth0')
def do_cgroup_net(args):
    """
    net io limit
    """
    print(args)

@add_args('-A', '--all', action='store_true', default=False, help='Clear all cgroup')
@add_args('-p', '--pid', help='Process pid to clear')
def do_cgroup_clear(args):
    """
    clear pid cgroups
    """
    assert args.all or args.pid is not None, '-A or -p <pid> must use one'
    filter_name = 'ctpid%s' % ('' if args.all else args.pid)
    group_names = lscgroup_filter(filter_name)
    for group_name in group_names:
        if 'cpu,cpuacct' in group_name:
            group_name = group_name.replace(',cpuacct', '')
        cgdelete_name(group_name)

def get_argparser():
    current_module = sys.modules[__name__]
    parser = argparse.ArgumentParser()
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('-C', '--clear', action='store_true', default=False, help='Clear cgroup')
    parent_parser.add_argument('-p', '--pid', required=True, help='Process pid')
    subparsers = parser.add_subparsers()
    for attr in (a for a in dir(current_module) if a.startswith('do_cgroup_')):
        command = attr[10:].replace('_', '-')
        callback = getattr(current_module, attr)
        action_help = callback.__doc__ or ''
        arguments =getattr(callback, 'arguments', [])
        if attr != 'do_cgroup_clear':
            subparser = subparsers.add_parser(command, parents=[parent_parser], help=action_help)
        else:
            subparser = subparsers.add_parser(command, help=action_help)
        for (args, kwargs) in arguments:
            subparser.add_argument(*args, **kwargs)
        subparser.set_defaults(func=callback)
    return parser


if __name__ == '__main__':
    parser = get_argparser()
    args = parser.parse_args()
    args.func(args)
