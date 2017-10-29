---
title: Installation
layout: slate
---
### Installation from Debian Package

To install a Debian package:

    sudo dpkg -i deb/libcpuaff-dev_x.x.x-x_all.deb

### Installation from Source

To install a stable release:

    tar -xzf cpuaff-x.x.x.tar.gz
    cd cpuaff-x.x.x
    ./configure
    make
    make install

To install from a repository snapshot:

    ./bootstrap.sh
    ./configure
    make
    make install

