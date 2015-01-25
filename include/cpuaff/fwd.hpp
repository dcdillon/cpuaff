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

#include "options.hpp"
#include <stdint.h>

namespace cpuaff
{
class cpu_spec;

namespace impl
{
template < typename TRAITS >
class basic_cpu;

template < typename TRAITS >
class basic_affinity_manager;

template < typename TRAITS >
class basic_cpu_set;

template < typename TRAITS >
class basic_affinity_stack;

template < typename TRAITS >
class basic_affinity_stack;
}  // namespace impl

typedef int32_t socket_type;
typedef int32_t core_type;
typedef int32_t processing_unit_type;
typedef int32_t numa_type;

#if defined(CPUAFF_PCI_SUPPORTED)

class pci_device_spec;
class pci_device_description;
class pci_name_resolver;

namespace impl
{
template < typename TRAITS >
class basic_pci_device;

template < typename TRAITS >
class basic_pci_device_set;
}  // namespace impl

typedef int32_t pci_vendor_id_type;
typedef int32_t pci_device_id_type;

#endif
}  // namespace cpuaff
