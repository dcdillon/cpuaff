---
title: Installation
layout: default
---
### Install from Source

To install a stable release:

```
tar -xzf cpuaff-x.x.x.tar.gz
cd cpuaff-x.x.x
./configure
make
make install
```

To install from a repository snapshot:

```
./bootstrap.sh
./configure
make
make install
```

### Install from PPA

```
sudo add-apt-repository ppa:dcdillon/cpuaff
sudo apt-get update
sudo apt-get install libcpuaff-dev
```
    
### Install Manually

Since `cpuaff` is header only, all you need to do is copy the include files to
a directory from which you will use them.  For example:

```
tar -xzf cpuaff-x.x.x.tar.gz
cp -r include/cpuaff /path/to/include/directory
```

Note: If you are on a platform that requires `hwloc` the `hwloc` headers must
be available at compile time as well.  Additionally you will need to link to
the `hwloc` library.