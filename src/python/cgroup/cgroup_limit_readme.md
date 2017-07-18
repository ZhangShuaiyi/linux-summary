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
