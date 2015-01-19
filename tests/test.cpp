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
 
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../include/cpuaff/cpuaff.hpp"

TEST_CASE("affinity_manager", "[affinity_manager]")
{
    cpuaff::affinity_manager manager;
    
    SECTION("affinity_manager::initialize(true)")
    {
        REQUIRE(manager.initialize(true));
        REQUIRE(manager.has_cpus());
        REQUIRE(manager.has_native_mapping());
        
#if defined(CPUAFF_PCI_SUPPORTED)
        REQUIRE(manager.has_pci_devices());
#endif
    }
    
    SECTION("affinity_manager::initialize(false)")
    {
        REQUIRE(manager.initialize(false));
        REQUIRE(manager.has_cpus());
        REQUIRE(!manager.has_native_mapping());
        
#if defined(CPUAFF_PCI_SUPPORTED)
        REQUIRE(manager.has_pci_devices());
#endif
    }
    
    SECTION("affinity_manager member functions")
    {
        REQUIRE(manager.initialize(true));
        
        cpuaff::cpu cpu;
        cpuaff::affinity_manager::native_cpu_wrapper_type native_wrapper;
        cpuaff::cpu_set_type cpus;
        
#if defined(CPUAFF_PCI_SUPPORTED)
        cpuaff::pci_device_set devices;
        cpuaff::pci_device device;
#endif
        
        REQUIRE(manager.get_cpu_from_index(cpu, 0));
        REQUIRE(manager.get_cpu_from_id(cpu, cpu.id()));
        REQUIRE(manager.get_cpu_from_id(cpu, cpu.id().get()));
        REQUIRE(manager.get_cpu_from_spec(cpu, cpuaff::cpu_spec(cpu.socket(), cpu.core(), cpu.processing_unit())));
        REQUIRE(manager.get_native_from_cpu(native_wrapper, cpu));
        REQUIRE(manager.get_cpu_from_native(cpu, native_wrapper));
        REQUIRE(manager.get_cpu_from_native(cpu, native_wrapper.get()));
        REQUIRE(manager.get_cpus(cpus));
        REQUIRE(manager.get_cpus_by_numa(cpus, cpu.numa()));
        REQUIRE(manager.get_cpus_by_socket(cpus, cpu.socket()));
        REQUIRE(manager.get_cpus_by_core(cpus, cpu.core()));
        REQUIRE(manager.get_cpus_by_processing_unit(cpus, cpu.processing_unit()));
        REQUIRE(manager.get_cpus_by_socket_and_core(cpus, cpu.socket(), cpu.core()));
        
#if defined(CPUAFF_PCI_SUPPORTED)
        REQUIRE(manager.get_pci_devices(devices));
        device = *devices.begin();
        REQUIRE(manager.get_pci_device_for_address(device, device.address()));
        REQUIRE(manager.get_pci_device_for_address(device, device.address().get()));
        REQUIRE(manager.get_pci_devices_by_spec(devices, cpuaff::pci_device_spec(device.vendor(), device.device())));
        REQUIRE(manager.get_pci_devices_by_numa(devices, device.numa()));
        REQUIRE(manager.get_pci_devices_by_vendor(devices, device.vendor()));
        REQUIRE(manager.get_nearby_cpus(cpus, device));
#endif

        REQUIRE(manager.get_affinity(cpus));
        REQUIRE(cpus.size() > 0);
        
        cpuaff::cpu_set_type new_affinity;
        new_affinity.insert(cpu);
        REQUIRE(manager.set_affinity(new_affinity));
        REQUIRE(manager.get_affinity(cpus));
        REQUIRE(cpus.size() == new_affinity.size());
        
        if (cpus.size() == new_affinity.size())
        {
            cpuaff::cpu_set_type::iterator i = cpus.begin();
            cpuaff::cpu_set_type::iterator iend = cpus.end();
            cpuaff::cpu_set_type::iterator j = new_affinity.begin();
            cpuaff::cpu_set_type::iterator jend = new_affinity.end();
            
            for ( ; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
        
        REQUIRE(manager.get_cpus(new_affinity));
        REQUIRE(manager.set_affinity(new_affinity));
        REQUIRE(manager.get_affinity(cpus));
        REQUIRE(cpus.size() == new_affinity.size());
        
        if (cpus.size() == new_affinity.size())
        {
            cpuaff::cpu_set_type::iterator i = cpus.begin();
            cpuaff::cpu_set_type::iterator iend = cpus.end();
            cpuaff::cpu_set_type::iterator j = new_affinity.begin();
            cpuaff::cpu_set_type::iterator jend = new_affinity.end();
            
            for ( ; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
        
        REQUIRE(manager.pin(cpu));
        REQUIRE(manager.get_affinity(cpus));
        REQUIRE(cpus.size() == 1);
        REQUIRE(*cpus.begin() == cpu);
    }
}

TEST_CASE("affinity_stack", "[affinity_stack]")
{
    SECTION("affinity_stack member functions")
    {
        cpuaff::affinity_manager manager;
        
        REQUIRE(manager.initialize(false));
        
        cpuaff::affinity_stack stack(manager);
        
        cpuaff::cpu_set_type original_affinity;
        cpuaff::cpu_set_type new_affinity;
        cpuaff::cpu_set_type cpus;
        
        REQUIRE(stack.get_affinity(original_affinity));
        REQUIRE(original_affinity.size() > 0);
        REQUIRE(stack.push_affinity());
        REQUIRE(stack.get_affinity(cpus));
        REQUIRE(cpus.size() == original_affinity.size());
        
        if (cpus.size() == original_affinity.size())
        {
            cpuaff::cpu_set_type::iterator i = cpus.begin();
            cpuaff::cpu_set_type::iterator iend = cpus.end();
            cpuaff::cpu_set_type::iterator j = original_affinity.begin();
            cpuaff::cpu_set_type::iterator jend = original_affinity.end();
            
            for ( ; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
        
        new_affinity.insert(*original_affinity.begin());
        
        REQUIRE(stack.set_affinity(new_affinity));
        REQUIRE(stack.get_affinity(cpus));
        REQUIRE(cpus.size() == 1);
        REQUIRE(*cpus.begin() == *new_affinity.begin());
        
        REQUIRE(stack.pop_affinity());
        REQUIRE(stack.get_affinity(cpus));
        REQUIRE(cpus.size() == original_affinity.size());
        
        if (cpus.size() == original_affinity.size())
        {
            cpuaff::cpu_set_type::iterator i = cpus.begin();
            cpuaff::cpu_set_type::iterator iend = cpus.end();
            cpuaff::cpu_set_type::iterator j = original_affinity.begin();
            cpuaff::cpu_set_type::iterator jend = original_affinity.end();
            
            for ( ; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
    }
}
