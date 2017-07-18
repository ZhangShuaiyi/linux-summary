#!/usr/bin/env python
import sys
import argparse
import subprocess

PREFIX = 'ctpid'

def get_cgroup_name(pid):
    return 'ctpid%s' % pid

def check_cgroup_exist(controller, name):
    cmd = 'lscgroup -g %s:%s' % (controller, name)
    out = subprocess.check_output(cmd, shell=True)
    return True if len(out) > 0 else False

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


def add_args(*args, **kwargs):
    def _decorator(func):
        if not hasattr(func, 'arguments'):
            func.arguments = []
        if (args, kwargs) not in func.arguments:
            func.arguments.insert(0, (args, kwargs))
        return func
    return _decorator

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
        cgcreate(controller, name)


@add_args('-m', '--mem', help='Max mem usage, eg:200M')
def do_cgroup_mem(args):
    """
    memory limit
    """
    print(args)

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
    print(args)

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
