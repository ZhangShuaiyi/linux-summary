FROM fedora:latest
ENV container docker
RUN dnf install -y bash-completion
RUN dnf install -y python
RUN dnf install -y vim
RUN dnf install -y git
RUN dnf install -y global
RUN dnf install -y astyle
RUN git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
RUN mkdir /root/.pip
RUN mkdir /root/.vim/plugin
RUN cp /usr/share/gtags/gtags.vim /root/.vim/plugin/
COPY vimrc /root/.vimrc
COPY pip.conf /root/.pip/pip.conf
RUN vim +PluginInstall +qall
ENV LANG C.UTF-8
VOLUME ["/data"]
CMD ["/usr/sbin/init"]
