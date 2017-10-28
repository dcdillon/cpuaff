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
#include <iomanip>
#include <map>
#include <set>
#include <vector>

#include <libgen.h>
#include <sched.h>
#include <unistd.h>

#include "sysfs_reader.hpp"

#if defined(CPUAFF_PCI_SUPPORTED)
#include "../../pci_device_spec.hpp"
#include "pci_device_reader.hpp"
#endif

namespace cpuaff
{
namespace impl
{
namespace linux_impl
{
typedef int cpu_identifier_type;

class cpu_identifier_wrapper
{
   public:
    inline cpu_identifier_wrapper() {}
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

typedef std::vector< cpu_info > cpu_loader_vector_type;

struct cpu_loader
{
    inline bool operator()(cpu_loader_vector_type &v)
    {
        v.clear();

        std::vector< sysfs_reader::pu > pus;
        std::map< int32_t, std::map< int32_t, int32_t > > cores_by_socket;
        std::map< int32_t, std::map< int32_t, std::vector< int32_t > > >
            pus_by_socket_by_core;

        if (sysfs_reader::load_cpus(pus))
        {
            std::vector< sysfs_reader::pu >::iterator pu = pus.begin();
            std::vector< sysfs_reader::pu >::iterator puend = pus.end();

            for (; pu != puend; ++pu)
            {
                core_type core;

                std::map< int32_t, int32_t >::iterator i =
                    cores_by_socket[pu->socket].find(pu->core);

                if (i != cores_by_socket[pu->socket].end())
                {
                    core = core_type(i->second);
                }
                else
                {
                    core = core_type(cores_by_socket[pu->socket].size());
                    cores_by_socket[pu->socket][pu->core] = int32_t(core);
                }

                processing_unit_type pu_id;

                pu_id = pus_by_socket_by_core[pu->socket][core].size();
                pus_by_socket_by_core[pu->socket][core].push_back(0);

                v.push_back(cpu_info(
                    cpu_spec(socket_type(pu->socket), core_type(core),
                             processing_unit_type(pu_id)),
                    cpu_identifier_type(pu->native), numa_type(pu->node)));
            }
        }

        return !!v.size();
    }
};

struct get_affinity
{
    inline bool operator()(std::set< cpu_identifier_wrapper > &cpus)
    {
        cpu_set_t cpu_set;
        if (0 == sched_getaffinity(0, sizeof(cpu_set_t), &cpu_set))
        {
            for (cpu_identifier_type i = 0; i < CPU_SETSIZE; ++i)
            {
                if (CPU_ISSET(i, &cpu_set))
                {
                    cpus.insert(cpu_identifier_wrapper(i));
                }
            }

            return true;
        }
        else
        {
            return false;
        }
    }
};

struct set_affinity
{
    inline bool operator()(const std::set< cpu_identifier_wrapper > &cpus)
    {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);

        std::set< cpu_identifier_wrapper >::iterator i = cpus.begin();
        std::set< cpu_identifier_wrapper >::iterator iend = cpus.end();

        for (; i != iend; ++i)
        {
            CPU_SET(i->get(), &cpu_set);
        }

        return (0 == sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set));
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

    inline pci_device_info(const pci_device_spec &s,
                           const numa_type &n,
                           const pci_address_type &a)
        : spec(s), numa(n), address(a)
    {
    }
};

typedef std::vector< pci_device_info > pci_loader_vector_type;

struct pci_loader
{
    inline bool operator()(pci_loader_vector_type &v)
    {
        pci_device_reader device_reader;

        if (device_reader.load())
        {
            std::vector< pci_device_reader::raw_pci_info >::const_iterator i =
                device_reader.devices().begin();
            std::vector< pci_device_reader::raw_pci_info >::const_iterator
                iend = device_reader.devices().end();

            for (; i != iend; ++i)
            {
                std::ostringstream buf;
                buf << std::hex;
                buf << std::setfill('0') << std::setw(4) << i->spec.vendor();
                buf << ":" << std::setfill('0') << std::setw(4)
                    << i->spec.device();

                v.push_back(pci_device_info(i->spec, i->numa, i->address));
            }

            return true;
        }

        return false;
    }
};

#endif

struct traits
{
    typedef linux_impl::cpu_identifier_type cpu_identifier_type;
    typedef cpu_identifier_wrapper cpu_identifier_wrapper_type;
    typedef cpu_loader cpu_loader_type;
    typedef linux_impl::cpu_loader_vector_type cpu_loader_vector_type;
    typedef get_affinity get_affinity_type;
    typedef set_affinity set_affinity_type;

#if defined(CPUAFF_PCI_SUPPORTED)
    typedef linux_impl::pci_address_type pci_address_type;
    typedef pci_address_wrapper pci_address_wrapper_type;
    typedef pci_loader pci_loader_type;
    typedef linux_impl::pci_loader_vector_type pci_loader_vector_type;
#endif
};

}  // namespace linux_impl
}  // namespace impl
}  // namespace cpuaff
