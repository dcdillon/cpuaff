---
title: Home
layout: home
---
### Short Description

cpuaff is a C++ library that abstracts CPU affinity settings for multiple platforms.  It is a header-only library on some platforms.  Other platforms are supported using [hwloc](http://www.open-mpi.org/projects/hwloc/).  The project aims to fully support all platforms as header-only eventually.

For a more detailed description of cpuaff, click [here](details.html)

To see a list of supported platforms click [here](supported_platforms.html).

### Releases

The latest source release is [cpuaff-1.0.6](releases/cpuaff-1.0.6.tar.gz).

The latest Ubuntu packages are available from [ppa:dcdillon/cpuaff](https://launchpad.net/~dcdillon/+archive/ubuntu/cpuaff).

To get other releases, go to the [downloads](downloads.html) page.

For installation instructions, click [here](installation.html)

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
