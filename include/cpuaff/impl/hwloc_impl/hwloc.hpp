/* Copyright (c) 2015-2017, Daniel C. Dillon
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

#include "../../cpu_spec.hpp"
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <vector>

#include <hwloc.h>
#include <iostream>
namespace cpuaff
{
namespace impl
{
namespace hwloc_impl
{
typedef unsigned int cpu_identifier_type;

class cpu_identifier_wrapper
{
   public:
    inline cpu_identifier_wrapper() : id_(0) {}

    inline cpu_identifier_wrapper(const cpu_identifier_type &id) : id_(id) {}

   public:
    const inline cpu_identifier_type &get() const { return id_; }
    inline bool operator<(const cpu_identifier_wrapper &rhs) const
    {
        return id_ < rhs.id_;
    }

   private:
    cpu_identifier_type id_;
};

class topology
{
   public:
    static inline topology &instance()
    {
        static topology instance;
        return instance;
    }

   private:
    inline topology()
    {
        hwloc_topology_init(&topology_);
        hwloc_topology_load(topology_);
    }

    inline topology(const topology &rhs) {}

    inline ~topology() { hwloc_topology_destroy(topology_); }

    inline void operator=(const topology &rhs) {}

   public:
    hwloc_topology_t &get() { return topology_; }

   private:
    hwloc_topology_t topology_;
};

struct cpu_info
{
    cpu_spec spec;
    cpu_identifier_wrapper id;
    numa_type numa;

    inline cpu_info(const cpu_spec &s,
                    const cpu_identifier_type &i,
                    const numa_type &n)
        : spec(s), id(i), numa(n)
    {
    }
};

class topology_reader
{
   public:
    inline topology_reader()
        : numa_id_(0), socket_id_(0), core_id_(0), processing_unit_id_(0)
    {
    }

    inline void read()
    {
        read(hwloc_get_root_obj(topology::instance().get()), 0);
    }

    inline std::vector< cpu_info > &cpus() { return cpus_; }

   private:
    inline void read(hwloc_obj_t obj, int depth)
    {
        if (obj->type == HWLOC_OBJ_NODE)
        {
            numa_id_ = obj->logical_index;
        }
        else if (obj->type == HWLOC_OBJ_SOCKET)
        {
            socket_id_ = obj->logical_index;
            core_id_ = -1;
        }
        else if (obj->type == HWLOC_OBJ_CORE)
        {
            ++core_id_;
            processing_unit_id_ = 0;
        }
        else if (obj->type == HWLOC_OBJ_PU)
        {
            cpus_.push_back(
                cpu_info(cpu_spec(socket_type(socket_id_), core_type(core_id_),
                                  processing_unit_type(processing_unit_id_++)),
                         hwloc_bitmap_first(obj->cpuset), numa_type(numa_id_)));
        }

        for (unsigned int i = 0; i < obj->arity; ++i)
        {
            read(obj->children[i], depth + 1);
        }
    }

   private:
    unsigned int numa_id_;
    unsigned int socket_id_;
    unsigned int core_id_;
    unsigned int processing_unit_id_;
    std::vector< cpu_info > cpus_;
};

typedef std::vector< cpu_info > cpu_loader_vector_type;

struct cpu_loader
{
    inline bool operator()(cpu_loader_vector_type &v)
    {
        topology_reader reader;
        reader.read();

        std::vector< cpu_info >::iterator i = reader.cpus().begin();
        std::vector< cpu_info >::iterator iend = reader.cpus().end();

        for (; i != iend; ++i)
        {
            v.push_back(*i);
        }

        return !!v.size();
    }
};

struct get_affinity
{
    inline bool operator()(std::set< cpu_identifier_wrapper > &cpus)
    {
        bool retval = false;
        hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc();

        if (0 == hwloc_get_cpubind(topology::instance().get(), cpu_set,
                                   HWLOC_CPUBIND_THREAD))
        {
            unsigned int id;
            hwloc_bitmap_foreach_begin(id, cpu_set)
                cpus.insert(cpu_identifier_wrapper(id));
            hwloc_bitmap_foreach_end();

            retval = true;
        }

        hwloc_bitmap_free(cpu_set);

        return retval;
    }
};

struct set_affinity
{
    inline bool operator()(const std::set< cpu_identifier_wrapper > &cpus)
    {
        hwloc_cpuset_t cpu_set = hwloc_bitmap_alloc();
        hwloc_bitmap_zero(cpu_set);

        typename std::set< cpu_identifier_wrapper >::const_iterator i =
            cpus.begin();
        typename std::set< cpu_identifier_wrapper >::const_iterator iend =
            cpus.end();

        for (; i != iend; ++i)
        {
            hwloc_bitmap_set(cpu_set, i->get());
        }

        bool retval = (0 == hwloc_set_cpubind(topology::instance().get(),
                                              cpu_set, HWLOC_CPUBIND_THREAD));
        hwloc_bitmap_free(cpu_set);
    }
};

#if defined(CPUAFF_PCI_SUPPORTED)

typedef std::string pci_address_type;

class pci_address_wrapper
{
   public:
    inline pci_address_wrapper() {}
    inline pci_address_wrapper(const pci_address_type &address)
        : address_(address)
    {
    }

   public:
    const inline pci_address_type &get() const { return address_; }
    inline bool operator<(const pci_address_wrapper &rhs) const
    {
        return address_ < rhs.address_;
    }

   private:
    pci_address_type address_;
};

struct pci_device_info
{
    pci_device_spec spec;
    numa_type numa;
    pci_address_type address;
    std::string description;

    inline pci_device_info(const pci_device_spec &s,
                           const numa_type &n,
                           const pci_address_type &a,
                           const std::string &d)
        : spec(s), numa(n), address(a), description(d)
    {
    }
};

typedef std::vector< pci_device_info > pci_loader_vector_type;

struct pci_loader
{
    inline bool operator()(pci_loader_vector_type &v) { return false; }
}

#endif

struct traits
{
    typedef hwloc_impl::cpu_identifier_type cpu_identifier_type;
    typedef cpu_identifier_wrapper cpu_identifier_wrapper_type;
    typedef cpu_loader cpu_loader_type;
    typedef hwloc_impl::cpu_loader_vector_type cpu_loader_vector_type;
    typedef get_affinity get_affinity_type;
    typedef set_affinity set_affinity_type;

#if defined(CPUAFF_PCI_SUPPORTED)
    typedef hwloc_impl::pci_address_type pci_address_type;
    typedef pci_address_wrapper pci_address_wrapper_type;
    typedef pci_loader pci_loader_type;
    typedef hwloc_impl::pci_loader_vector_type pci_loader_vector_type;
#endif
};

}  // namespace hwloc_impl
}  // namespace impl
}  // namespace cpuaff
