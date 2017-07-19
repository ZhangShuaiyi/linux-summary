## cpu资源

### cpu.cfs_quota_us
+ cpu.cfs_quota_us可用来限制cpu资源，该值和cpu.cfs_period_us有关系，默认cpu.cfs_period_us为100000，
cpu.cfs_quota_us值为-1，表示不限制cpu时间。如果想限制cpu使用率为50%，则可以设置cpu.cfs_quota_us为50000。
+ cpu.cfs_quota_us的值可以大于cpu.cfs_period_us，在多核系统中，设置最高cpu使用率为150%，则可以设置
cpu.cfs_quota_us的值为150000。
+ 使用cgclassify命令移动的是spid，ps -T -p <pid>可查看到该进程的所有线程对应的spid。将进程的所有线程的spid
都移动到该cpu cgroup中可限制对应进程的总cpu使用情况，通过top -H -p <pid>命令查看进程的各个线程的cpu使用率。
将多个线程spid设置到该cpu cgroup的task中后，这几个线程总共的最高cpu使用率为设置的cpu.cfs_quota_us值。

## memory资源
参考[Memory Resource Controller](https://www.kernel.org/doc/Documentation/cgroup-v1/memory.txt)

### memory.limit_in_bytes和memory.memsw.limit_in_bytes
```
 memory.limit_in_bytes          # set/show limit of memory usage
 memory.memsw.limit_in_bytes    # set/show limit of memory+Swap usage
```
+ memory.limit_in_bytes限制物理内存使用量，memory.memsw.limit_in_bytes限制物理内存+Swap空间的总量，若想限制进程
的内存使用总量为100M，可设置memory.limit_in_bytes和memory.memsw.limit_in_bytes的值为100M，超出时会触发oom

## block io

### device number
blkio限速时需要知道指定块设备的device number，如/dev/sda设备device number为8:0，可通过一下方法查看:
+ ls -l /dev/sda
```
brw-rw----. 1 root disk 8, 0 Jul 19 09:40 /dev/sda
```
+ stat -c '%t:%T' /dev/sda
```
8:0
```
+ 通过stat函数获取属性后，st_rdev为device number，通过major和minor函数获取MAJOR和MINOR值。


### 限速
参考[Linux cgroup资源隔离各个击破之 - io隔离](https://yq.aliyun.com/articles/54458?spm=5176.8067842.tagmain.29.jmJCdH)
+ blkio.throttle.write_bps_device和blkio.throttle.read_bps_device限制块设备的读写bps
```
cgset -r blkio.throttle.read_bps_device='8:0 10485760' <cgroup path>
```
+ blkio.throttle.write_iops_device和blkio.throttle.read_iops_device限制块设备的读写iops

