/home/zsy/Workspace/qemu-kvm/qemu/x86_64-softmmu/qemu-system-x86_64 \
        -enable-kvm \
        -s \
        -hda /home/imgs/test.qcow2 \
        -device vhost-vsock-pci,id=vhost-vsock-pci0,guest-cid=3 \
        -device virtio-net-pci,netdev=net0,ioeventfd=on \
        -netdev tap,id=net0,ifname=tap0,script=no,downscript=no,vhost=on \
        -m 512 \
        -monitor stdio
