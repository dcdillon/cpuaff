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

#include "config.hpp"

#include "cpu_spec.hpp"
#include "impl/basic_cpu.hpp"
#include "impl/basic_cpu_set.hpp"
#include "impl/basic_affinity_manager.hpp"
#include "impl/basic_native_cpu_mapper.hpp"
#include "impl/basic_affinity_stack.hpp"
#include "impl/basic_round_robin_allocator.hpp"

#if defined(CPUAFF_PCI_SUPPORTED)

#include "impl/basic_pci_device.hpp"
#include "impl/basic_pci_device_set.hpp"
#include "impl/basic_pci_device_manager.hpp"
#include "pci_device_description.hpp"
#include "pci_device_spec.hpp"
#include "pci_name_resolver.hpp"

#endif

/*!
 * Namespace for all cpuaff functionality
 */
namespace cpuaff
{
/*!
 * affinity_stack is used to keep track of affinities as you get and set
 * them.  It is basically a wrapper for an affinity_manager that gives you the
 * ability to keep track of what affinities have been and reset them later.
 */
typedef impl::basic_affinity_stack< traits > affinity_stack;

/*!
 * cpu is a representation of a cpu on the system.  It contains all the
 * data needed to get and set thread affinity as well data specifying the
 * hardware layout of the cpu.
 */
typedef impl::basic_cpu< traits > cpu;

/*!
 * affinity_manager is a collection of all the valid cpus.  It provides
 * interfaces to get and set cpu affinity as well as ways to classify cpus
 * on the system so that intelligent choices can be made about what affinity a
 * thread should have.
 */
typedef impl::basic_affinity_manager< traits > affinity_manager;

/*!
 * native_cpu_mapper is a utility class that maps native cpu
 * representations to cpus from a basic_cpu_manager.  It may not be available
 * for every platform.
 */
typedef impl::basic_native_cpu_mapper< traits > native_cpu_mapper;

/*!
 * A set that can hold unique cpus.
 */
typedef impl::basic_cpu_set< traits > cpu_set;

/*!
 * round_robin_allocator is a utility class that takes a set of cpus
 * and returns them as requested in a round-robin fashion.  It organizes the
 * cpus such that it returns consecutive cpus from different cores if it can.
 */
typedef impl::basic_round_robin_allocator< traits > round_robin_allocator;

#if defined(CPUAFF_PCI_SUPPORTED)
/*!
 * basic_pci_device_manager is a collection of all the pci devices on the
 * system.
 */
typedef impl::basic_pci_device_manager< traits > pci_device_manager;

/*!
 * basic_pci_device is a representation of a pci_device on the system.  It
 * contains identification, address, and numa information.  The device can
 * then be used to determine what cpus are local to it.
 */
typedef impl::basic_pci_device< traits > pci_device;

/*!
 * A set that can hold unique pci devices
 */
typedef impl::basic_pci_device_set< traits > pci_device_set;

#endif
}
