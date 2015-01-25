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

#include "../../cpu_spec.hpp"
#include <set>
#include <vector>

namespace cpuaff
{
namespace impl
{
namespace null_impl
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

typedef std::vector< std::pair< cpu_identifier_type, cpu_spec > >
    cpu_loader_vector_type;

struct cpu_loader
{
    inline bool operator()(cpu_loader_vector_type &v) { return false; }
};

struct get_affinity
{
    inline bool operator()(std::set< cpu_identifier_wrapper > &cpus)
    {
        return false;
    }
};

struct set_affinity
{
    inline bool operator()(const std::set< cpu_identifier_wrapper > &cpus)
    {
        return false;
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
    typedef null::cpu_identifier_type cpu_identifier_type;
    typedef cpu_identifier_wrapper cpu_identifier_wrapper_type;
    typedef cpu_loader cpu_loader_type;
    typedef null::cpu_loader_vector_type cpu_loader_vector_type;
    typedef get_affinity get_affinity_type;
    typedef set_affinity set_affinity_type;

#if defined(CPUAFF_PCI_SUPPORTED)
    typedef null::pci_address_type pci_address_type;
    typedef pci_address_wrapper pci_address_wrapper_type;
    typedef pci_loader pci_loader_type;
    typedef null::pci_loader_vector_type pci_loader_vector_type;
#endif
};

}  // namespace null_impl
}  // namespace impl
}  // namespace cpuaff
