gdbserver \
              :2345 \
              /home/zsy/Workspace/code/qemu/qemu-2.6.2/x86_64-softmmu/qemu-system-x86_64 \
              -nographic -machine pc-i440fx-2.6,accel=kvm,usb=off \
              -cpu Nehalem -m 2048 \
              -kernel /home/zsy/Data/nfs/ubuntu_linux/linux-4.4.0/arch/x86_64/boot/bzImage \
              -initrd /home/zsy/Data/qemu-kvm/initrd.img-4.4.0-38-generic \
              -hda /home/imgs/ubuntu1604.qcow2 \
              -device virtio-serial-pci,id=virtio-serial0,ioeventfd=off \
              -chardev socket,id=charchannel0,path=/tmp/serial,server,nowait \
              -device virtserialport,bus=virtio-serial0.0,nr=1,chardev=charchannel0,id=channel0,name=com.redhat.spice.0 \
              -append "root=/dev/mapper/ubuntu--vg-root console=ttyS0"
