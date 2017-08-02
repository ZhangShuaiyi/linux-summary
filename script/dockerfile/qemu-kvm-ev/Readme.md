+ build
```
docker build -t local/qemu-kvm-ev .
```
+ run
```
docker run -d --privileged --name myqemu --network=host -v /etc/localtime:/etc/localtime:ro -v /lib/modules:/lib/modules:ro -v /run/:/run/ -v /dev:/dev -v /sys/fs/cgroup:/sys/fs/cgroup -v mylibvirtd:/var/lib/libvirt -v myqemu:/etc/libvirt/qemu local/qemu-kvm-ev
```
