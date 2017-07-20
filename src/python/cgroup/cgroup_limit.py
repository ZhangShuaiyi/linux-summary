#!/usr/bin/env python
#coding=utf-8
import sys
import os
import re
import argparse
import subprocess

def unit_to_bytes(data):
    assert re.match('^[0-9]+[kKmMgG]?$', data) is not None, 'can use K,M,G'
    us = {
        'k': 1024,
        'K': 1024,
        'm': 1024 * 1024,
        'M': 1024 * 1024,
        'g': 1024 * 1024 * 1024,
        'G': 1024 * 1024 * 1024
    }
    v = re.search('[0-9]+', data).group()
    v = int(v)
    r = re.search('[kKmMgG]', data)
    if r is not None:
        u = r.group()
        v *= us[u]
    return v

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

def tc_qdisc_show(dev):
    cmd = 'tc qdisc show dev %s' % dev
    out = subprocess.check_output(cmd, shell=True)
    return out

def tc_class_del(pid, dev, tc_major):
    cmd = 'tc class show dev %s classid %s:%s' % (dev, tc_major, hex(pid))
    out = subprocess.check_output(cmd, shell=True)
    if len(out) > 0:
        cmd = 'tc class del dev %s classid %s:%s' % (dev, tc_major, hex(pid))
        subprocess.call(cmd, shell=True)

def tc_qdisc_get_qdisc_and_major(dev):
    out = tc_qdisc_show(dev)
    m = re.search('qdisc ([a-z_]+) ([0-9]+): ([a-z]+)', out)
    assert m is not None
    qdisc = m.groups()[0]
    major = m.groups()[1]
    return (qdisc, major)

def tc_qdisc_add(dev):
    tc_major = '368'
    qdisc, major = tc_qdisc_get_qdisc_and_major(dev)
    if qdisc != 'htb':
        cmd = 'tc qdisc add dev %s root handle %s: htb' % (dev, tc_major)
        subprocess.call(cmd, shell=True)
    else:
        tc_major = major
    return tc_major

def tc_qdisc_do_del(pid = None):
    tc_major = '368'
    cmd = 'tc qdisc show'
    out = subprocess.check_output(cmd, shell=True)
    lines = out.splitlines()
    for line in lines:
        r = re.search('qdisc ([a-z]+) ([0-9]+): dev ([a-z0-9]+)', line)
        if r is not None:
            qdisc = r.groups()[0]
            major = r.groups()[1]
            dev = r.groups()[2]
            if pid is None:
                if qdisc == 'htb' and major == tc_major:
                    cmd = 'tc qdisc del dev %s root' % dev
                    subprocess.call(cmd, shell=True)
            elif qdisc == 'htb':
                tc_class_del(pid, dev, major)


def tc_class_add(pid, dev, value, tc_major):
    tc_class_del(pid, dev, tc_major)
    cmd = 'tc class add dev %s parent %s: classid %s:%s htb rate %sbps burst %sbit cburst %sbit' % \
          (dev, tc_major, tc_major, hex(pid), value, value, value)
    subprocess.call(cmd, shell=True)

def tc_filter_add(dev, tc_major):
    cmd = 'tc filter show dev %s parent %s:' % (dev, tc_major)
    out = subprocess.check_output(cmd, shell=True)
    if 'cgroup' in out:
        return
    cmd = 'tc filter add dev %s parent %s: protocol ip prio 1 handle 1: cgroup' % (dev, tc_major)
    subprocess.call(cmd, shell=True)

def tc_qdisc_class_filter_create(pid, dev, value):
    tc_major = tc_qdisc_add(dev)
    tc_class_add(pid, dev, value, tc_major)
    tc_filter_add(dev, tc_major)
    return tc_major

def net_cls_tc_create(pid, dev, value):
    tc_major = tc_qdisc_class_filter_create(pid, dev, value)
    controller = 'net_cls'
    name = get_cgroup_name(pid)
    cgcreate(controller, name)
    minor = '%04x' % pid
    classid = '0x%s%s' % (tc_major, minor)
    cgset('net_cls.classid', classid, name)
    cgclassify(controller, name, pid)


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
        # 使用memory.limit_in_bytes和memory.memsw.limit_in_bytes限制内存使用
        cgset('memory.limit_in_bytes', mem, name)
        cgset('memory.memsw.limit_in_bytes', mem, name)
        cgclassify(controller, name, args.pid)


@add_args('-d', '--dev', help='block device to limit, eg:/dev/sda')
@add_args('--wbps', help='Max write bps: eg:--wbps 10M')
@add_args('--rbps', help='Max read bps: eg:--wbps 10M')
@add_args('--wiops', type=int, help='Max write iops')
@add_args('--riops', type=int, help='Max read iops')
def do_cgroup_blkio(args):
    """
    block device io limit
    """
    controller = 'blkio'
    name = get_cgroup_name(args.pid)
    if args.clear:
        cgdelete(controller, name)
        return
    assert args.dev is not None, 'Need --dev <block device>'
    st = os.lstat(args.dev)
    major = os.major(st.st_rdev)
    minor = os.minor(st.st_rdev)
    def wbps_func(bps):
        value = unit_to_bytes(bps)
        value = '\"%d:%d %d\"' % (major, minor, value)
        cgset('blkio.throttle.write_bps_device', value, name)
    def rbps_func(bps):
        value = unit_to_bytes(bps)
        value = '\"%d:%d %d\"' % (major, minor, value)
        print(value)
        cgset('blkio.throttle.read_bps_device', value, name)
    def wiops_func(iops):
        value = '\"%d:%d %d\"' % (major, minor, iops)
        cgset('blkio.throttle.write_iops_device', value, name)
    def riops_func(iops):
        value = '\"%d:%d %d\"' % (major, minor, iops)
        cgset('blkio.throttle.read_iops_device', value, name)
    funcs = {
        'wbps': wbps_func,
        'rbps': rbps_func,
        'wiops': wiops_func,
        'riops': riops_func
    }
    cgcreate(controller, name)
    for k, v in vars(args).items():
        if v is not None and funcs.get(k, None) is not None:
            funcs.get(k, None)(v)
    cgclassify(controller, name, args.pid)

@add_args('-d', '--dev', required=True, help='network interface to limit, eg:eth0')
@add_args('-s', '--speed', help='Max speed of bps, eg:10M')
def do_cgroup_net(args):
    """
    net io limit
    """
    controller = 'net_cls'
    name = get_cgroup_name(args.pid)
    if args.clear:
        (qdisc, major) = tc_qdisc_get_qdisc_and_major(args.dev)
        if qdisc == 'htb':
            tc_class_del(args.pid, args.dev, major)
        cgdelete(controller, name)
        return
    if args.speed is not None:
        assert re.match('^[0-9]+[kKmMgG]?$', args.speed) is not None, '-s <bps to limit>, can use K,M,G'
        net_cls_tc_create(args.pid, args.dev, args.speed)


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
        elif 'net_cls,net_prio' in group_name:
            group_name = group_name.replace(',net_prio', '')
        cgdelete_name(group_name)
    if args.all:
        tc_qdisc_do_del()
        return
    if args.pid:
        tc_qdisc_do_del(args.pid)

def get_argparser():
    current_module = sys.modules[__name__]
    parser = argparse.ArgumentParser()
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('-C', '--clear', action='store_true', default=False, help='Clear cgroup')
    parent_parser.add_argument('-p', '--pid', type=int, required=True, help='Process pid')
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
