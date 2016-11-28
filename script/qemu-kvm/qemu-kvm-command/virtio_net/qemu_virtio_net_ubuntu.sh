# sudo ip link add name br0 type bridge
# sudo ip link set br0 up
# sudo ip addr add 192.168.1.101/24 dev br0
/home/zsy/Workspace/code/qemu/qemu-2.6.2/x86_64-softmmu/qemu-system-x86_64 \
        -s \
        -enable-kvm \
        -kernel /home/zsy/Data/nfs/ubuntu_linux/linux-4.4.0/arch/x86_64/boot/bzImage \
        -initrd /home/zsy/Data/qemu-kvm/initrd.img-4.4.0-38-generic \
        -hda /home/imgs/ubuntu1604.qcow2 \
        -net nic,model=virtio \
        -net tap,ifname=tap0,script=no,downscript=no,vhost=on \
        -m 512 \
        -nographic \
        -append "root=/dev/mapper/ubuntu--vg-root console=ttyS0" \
        -serial pty
# sudo ip link set tap0 master br0
# sudo ip link set tap0 up
