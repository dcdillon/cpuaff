---
title: Home
layout: default
theme: jekyll-theme-slate
---

### News

##### Version 1.0.6 Released

After quite some time, I have decided to roll a new release of cpuaff.
It has a couple of small bugfixes, one big bugfix, and some reorganization
and better test code.  Everything else is pretty much the same and things
have more or less just worked.  If anyone sees any issues or has new
feature requests please post them in issues at
[github](https://github.com/dcdillon/cpuaff).

Also please note that I have changed the PPA where the Ubuntu packages live.
They now live at [ppa:dcdillon/cpuaff](https://launchpad.net/~dcdillon/+archive/ubuntu/cpuaff)

* Fixed `affinity_manager::get_cpus_by_socket_and_core()` which now actually
  does what it says it does (instead of returning nothng ever).
* Fixed a bug in the `set_affinity()` family of functions when using hwloc.
  It was actually working, but was returning a failure even when it suceeded.
* Reorganized the repo to be a lot cleaner.
* Added packages to the PPA for Ubuntu Zesty and Artful (they should appear soon).
* Upgraded to the latest version of [Catch](https://github.com/philsquared/catch)

Released [cpuaff-1.0.6](releases/cpuaff-1.0.6.tar.gz)

2017-10-28 18:24:00

### Short Description

cpuaff is a C++ library that abstracts CPU affinity settings for multiple platforms.  It is a header-only library on some platforms.  Other platforms are supported using [hwloc](http://www.open-mpi.org/projects/hwloc/).  The project aims to fully support all platforms as header-only eventually.

For a more detailed description of cpuaff, click [here](details.html)

To see a list of supported platforms click [here](supported_platforms.html).

### Releases

The latest source release is [cpuaff-1.0.2](releases/cpuaff-1.0.2.tar.gz).

The latest Ubuntu pacakges are available from [ppa:dcdillon/ppa](https://launchpad.net/~dcdillon/+archive/ubuntu/ppa).

To get other releases, go to the [downloads](downloads.html) page.

For installation instructions, click [here](installation.html)

To get a snapshot of the repository, click one of the download links on this page.

### Simple Example

``` cpp
#include <cpuaff/cpuaff.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    cpuaff::affinity_manager manager;

    if (manager.has_cpus())
    {
        cpuaff::cpu_set cpus;
        manager.get_affinity(cpus);

        std::cout << "Initial Affinity:" << std::endl;

        cpuaff::cpu_set::iterator i = cpus.begin();
        cpuaff::cpu_set::iterator iend = cpus.end();

        for (; i != iend; ++i)
        {
            std::cout << "  " << (*i) << std::endl;
        }

        std::cout << std::endl;

        // set the affinity to all the processing units on
        // the first core
        cpuaff::cpu_set core_0;
        manager.get_cpus_by_core(core_0, 0);

        manager.set_affinity(core_0);
        manager.get_affinity(cpus);

        std::cout << "Affinity After Calling set_affinity():"
                  << std::endl;
        i = cpus.begin();
        iend = cpus.end();

        for (; i != iend; ++i)
        {
            std::cout << "  " << (*i) << std::endl;
        }

        return 0;
    }

    std::cerr << "cpuaff: unable to load cpus." << std::endl;
    return -1;
}
```

### Pronunciation

Due to popular demand, we now have a guide on pronunciation.  cpuaff is pronounced "spoof" because all other pronunciations I have come up with sound like garbled French.

### Licensing

cpuaff is distributed under the [New BSD](http://opensource.org/licenses/BSD-3-Clause) (or BSD 3-Clause) license.
