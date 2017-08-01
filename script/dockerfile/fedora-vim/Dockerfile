FROM fedora:latest
ENV container docker
RUN dnf install -y bash-completion
RUN dnf install -y python
RUN dnf install -y vim
RUN dnf install -y git
RUN dnf install -y global
RUN dnf install -y astyle
RUN dnf install -y automake gcc gcc-c++ kernel-devel cmake
RUN dnf install -y python-devel python3-devel
RUN git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
RUN mkdir /root/.pip
RUN mkdir /root/.vim/plugin
RUN cp /usr/share/gtags/gtags.vim /root/.vim/plugin/
COPY vimrc /root/.vimrc
COPY pip.conf /root/.pip/pip.conf
COPY ycm_extra_conf.py /root/.vim/.ycm_extra_conf.py
RUN pip install yapf
RUN vim +PluginInstall +qall
RUN cd ~/.vim/bundle/YouCompleteMe && ./install.py --clang-completer
RUN cd /usr/lib64 && ln -s libtinfo.so.6 libtinfo.so.5
ENV LANG C.UTF-8
VOLUME ["/data"]
CMD ["/usr/sbin/init"]
