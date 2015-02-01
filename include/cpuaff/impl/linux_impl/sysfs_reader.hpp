/* Copyright (c) 2015, Daniel C. Dillon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#include "set_reader.hpp"
#include "../../cpu_spec.hpp"
#include <set>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <dirent.h>

namespace cpuaff
{
namespace impl
{
namespace linux_impl
{
namespace sysfs_reader
{
struct pu
{
    int32_t node;
    int32_t socket;
    int32_t core;
    int32_t native;
};

inline bool read_list(std::set< int32_t > &set, const std::string &_file)
{
    set.clear();
    std::ifstream infile(_file.c_str());
    
    if (infile.good())
    {
        std::string line;
        std::getline(infile, line);
    
        set_reader::read_int_set(set, line);
        infile.close();
    }
    
    return !!set.size();
}

inline bool read_nodes(std::set< int32_t > &nodes)
{
    return read_list(nodes, "/sys/devices/system/node/has_cpu");
}

inline bool read_cpus(std::set< int32_t > &cpus, int32_t node)
{
    std::ostringstream buf;
    buf << "/sys/devices/system/node/node" << node << "/cpulist";
    return read_list(cpus, buf.str());
}

inline bool read_cpus(std::set< int32_t > &cpus)
{
    return read_list(cpus, "/sys/devices/system/cpu/possible");
}

inline int32_t read_socket(int32_t cpu)
{
    std::ostringstream buf;
    std::set< int32_t > sockets;
    
    buf << "/sys/devices/system/cpu/cpu" << cpu << "/topology/physical_package_id";
    
    read_list(sockets, buf.str());
    
    if (sockets.size() == 1)
    {
        return *sockets.begin();
    }
    else
    {
        return -1;
    }
}

inline int32_t read_socket(int32_t node, int32_t cpu)
{
    std::ostringstream buf;
    std::set< int32_t > sockets;

    buf << "/sys/devices/system/node/node" << node << "/cpu" << cpu
        << "/topology/physical_package_id";

    read_list(sockets, buf.str());

    if (sockets.size() == 1)
    {
        return *sockets.begin();
    }
    else
    {
        return -1;
    }
}

inline int32_t read_core(int32_t cpu)
{
    std::ostringstream buf;
    std::set< int32_t > cores;
    
    buf << "/sys/devices/system/cpu/cpu" << cpu << "/topology/core_id";
    
    read_list(cores, buf.str());
    
    if (cores.size() == 1)
    {
        return *cores.begin();
    }
    else
    {
        return -1;
    }
}

inline int32_t read_core(int32_t node, int32_t cpu)
{
    std::ostringstream buf;
    std::set< int32_t > cores;

    buf << "/sys/devices/system/node/node" << node << "/cpu" << cpu
        << "/topology/core_id";

    read_list(cores, buf.str());

    if (cores.size() == 1)
    {
        return *cores.begin();
    }
    else
    {
        return -1;
    }
}

inline bool read_cpu(std::vector< pu > &pus, int32_t cpu)
{
    int32_t socket = read_socket(cpu);
    int32_t core = read_core(cpu);
    
    pu u;
    u.node = -1;
    u.native = cpu;
    u.socket = socket;
    u.core = core;
    pus.push_back(u);
    return true;
}

inline bool read_cpu(std::vector< pu > &pus, int32_t node, int32_t cpu)
{
    int32_t socket = read_socket(node, cpu);
    int32_t core = read_core(node, cpu);

    if (socket >= 0 && core >= 0)
    {
        pu u;
        u.node = node;
        u.native = cpu;
        u.socket = socket;
        u.core = core;
        pus.push_back(u);
        return true;
    }

    return false;
}

inline bool read_node(std::vector< pu > &pus, int32_t node)
{
    std::set< int32_t > cpus;
    read_cpus(cpus, node);

    std::set< int32_t >::iterator i = cpus.begin();
    std::set< int32_t >::iterator iend = cpus.end();

    for (; i != iend; ++i)
    {
        read_cpu(pus, node, *i);
    }

    return !!pus.size();
}

inline bool load_cpus(std::vector< pu > &pus)
{
    pus.clear();

    std::set< int32_t > nodes;

    if (read_nodes(nodes))
    {
        std::set< int32_t >::iterator i = nodes.begin();
        std::set< int32_t >::iterator iend = nodes.end();
    
        for (; i != iend; ++i)
        {
            read_node(pus, *i);
        }
    }
    else
    {
        // we don't have nodes to read, so let's just set node to
        // -1 for everything and read the cpus
        std::set< int32_t > cpus;
        read_cpus(cpus);
        
        std::set< int32_t >::iterator i = cpus.begin();
        std::set< int32_t >::iterator iend = cpus.end();
        
        for ( ; i != iend; ++i)
        {
            read_cpu(pus, *i);
        }
    }

    return !!pus.size();
}
};

}  // namespace linux_impl
}  // namespace impl
}  // namespace cpuaff