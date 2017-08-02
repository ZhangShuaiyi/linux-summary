FROM centos:7
ENV container docker
RUN yum install -y bash-completion
RUN yum install -y centos-release-qemu-ev
RUN yum install -y qemu-kvm-ev libvirt && yum clean all
COPY libvirtd.conf /etc/libvirt/libvirtd.conf
COPY qemu.conf /etc/libvirt/qemu.conf
CMD ["/bin/bash", "-c", "chmod 660 /dev/kvm && chown root:qemu /dev/kvm && libvirtd --listen"]
