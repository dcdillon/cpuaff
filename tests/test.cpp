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
#include <iostream>

#include "../include/cpuaff/cpuaff.hpp"

#if defined(CPUAFF_PCI_SUPPORTED)
TEST_CASE("pci_device_manager", "[pci_device_manager]")
{
    cpuaff::pci_device_manager manager;

    if (manager.has_pci_devices())
    {
        cpuaff::pci_device_set devices;
        cpuaff::pci_device device;

        REQUIRE(manager.get_pci_devices(devices));
        REQUIRE(!devices.empty());

        device = *devices.begin();

        REQUIRE(manager.get_pci_device_for_address(device, device.address()));
        REQUIRE(
            manager.get_pci_device_for_address(device, device.address().get()));
        REQUIRE(manager.get_pci_devices_by_spec(devices, device.spec()));
        REQUIRE(manager.get_pci_devices_by_numa(devices, device.numa()));
        REQUIRE(manager.get_pci_devices_by_vendor(devices, device.vendor()));
    }
}
#endif

TEST_CASE("affinity_manager", "[affinity_manager]")
{
    cpuaff::affinity_manager manager;

    SECTION("affinity_manager member functions")
    {
        REQUIRE(manager.has_cpus());

        cpuaff::cpu first_cpu;

        // We can get the first CPU by index
        REQUIRE(manager.get_cpu_from_index(first_cpu, 0));
        REQUIRE(first_cpu.socket() >= 0);
        REQUIRE(first_cpu.core() >= 0);
        REQUIRE(first_cpu.processing_unit() >= 0);
        
        // We can get this same CPU by its native handle wrapper
        {
            cpuaff::cpu cpu;
            REQUIRE(manager.get_cpu_from_id(cpu, first_cpu.id()));
            REQUIRE(cpu == first_cpu);
        }
        
        // We can get this came CPU by its raw native id
        {
            cpuaff::cpu cpu;
            REQUIRE(manager.get_cpu_from_id(cpu, first_cpu.id().get()));
            REQUIRE(cpu == first_cpu);
        }
        
        // We can get this same CPU by its cpu_spec
        {
            cpuaff::cpu cpu;
            REQUIRE(manager.get_cpu_from_spec(
                cpu,
                cpuaff::cpu_spec(first_cpu.socket(), first_cpu.core(), first_cpu.processing_unit())));
        }

        // We can get all the CPUs
        {
            cpuaff::cpu_set cpus;
            REQUIRE(manager.get_cpus(cpus));
            REQUIRE(!cpus.empty() > 0);
        }
        
        // We can get all the CPUs for the NUMA of the first CPU
        {
            cpuaff::cpu_set cpus;
            REQUIRE(manager.get_cpus_by_numa(cpus, first_cpu.numa()));
            REQUIRE(!cpus.empty());
        }
        
        // We can get all the CPUs for the socket of the first CPU
        {
            cpuaff::cpu_set cpus;
            REQUIRE(manager.get_cpus_by_socket(cpus, first_cpu.socket()));
            REQUIRE(!cpus.empty());
        }
        
        // We can get all the CPUs with the same core as the first CPU
        {
            cpuaff::cpu_set cpus;
            REQUIRE(manager.get_cpus_by_core(cpus, first_cpu.core()));
            REQUIRE(!cpus.empty());
        }
        
        // We can get all the CPUs with the same processing unit as the first
        // CPU
        {
            cpuaff::cpu_set cpus;
            REQUIRE(
                manager.get_cpus_by_processing_unit(cpus, first_cpu.processing_unit()));
            REQUIRE(!cpus.empty());
        }
        
        // We can get all the CPUs on the same socket and core as the first cpu
        {
            cpuaff::cpu_set cpus;
            REQUIRE(manager.get_cpus_by_socket_and_core(cpus, first_cpu.socket(),
                                                        first_cpu.core()));
            REQUIRE(!cpus.empty());
        }

        // We can get the affinity of the current thread
        {
            cpuaff::cpu_set cpus;
            REQUIRE(manager.get_affinity(cpus));
            REQUIRE(!cpus.empty());
        }

        // We can set the affinity of the current thread
        {
            cpuaff::cpu_set cpus;
            cpuaff::cpu_set new_affinity;
            
            new_affinity.insert(first_cpu);
            
            REQUIRE(manager.set_affinity(new_affinity));
            REQUIRE(manager.get_affinity(cpus));

#if !defined(USING_TRAVIS_CI)
            cpuaff::cpu_set::iterator i = cpus.begin();
            cpuaff::cpu_set::iterator iend = cpus.end();
            cpuaff::cpu_set::iterator j = new_affinity.begin();
            cpuaff::cpu_set::iterator jend = new_affinity.end();
    
            for (; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
#endif
        }

        // We can set the affinity of the current thread back to all cpus
        {
            cpuaff::cpu_set cpus;
            cpuaff::cpu_set new_affinity;
            
            REQUIRE(manager.get_cpus(new_affinity));
            
            REQUIRE(manager.set_affinity(new_affinity));
            REQUIRE(manager.get_affinity(cpus));

#if !defined(USING_TRAVIS_CI)
            cpuaff::cpu_set::iterator i = cpus.begin();
            cpuaff::cpu_set::iterator iend = cpus.end();
            cpuaff::cpu_set::iterator j = new_affinity.begin();
            cpuaff::cpu_set::iterator jend = new_affinity.end();
    
            for (; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
#endif
        }

        // We can pin the affinity of the current thread to a particular CPU
        {
            cpuaff::cpu_set cpus;
            
            REQUIRE(manager.pin(first_cpu));
            REQUIRE(manager.get_affinity(cpus));
            
#if !defined(USING_TRAVIS_CI)
            REQUIRE(cpus.size() == 1);
            REQUIRE(*cpus.begin() == first_cpu);
#endif
        }
    }
}

TEST_CASE("affinity_stack", "[affinity_stack]")
{
    SECTION("affinity_stack member functions")
    {
        cpuaff::affinity_manager manager;

        REQUIRE(manager.has_cpus());

        cpuaff::affinity_stack stack(manager);

        cpuaff::cpu_set original_affinity;
        cpuaff::cpu_set cpus;

        // get the current affinity of this thread
        REQUIRE(stack.get_affinity(original_affinity));
        REQUIRE(!original_affinity.empty());
        
        // push the current affinity of this thread onto the stack
        REQUIRE(stack.push_affinity());
        
        // verify that the affinities are the same (as we didn't change them)
        REQUIRE(stack.get_affinity(cpus));
        REQUIRE(cpus.size() == original_affinity.size());

#if !defined(USING_TRAVIS_CI)
        {
            cpuaff::cpu_set::iterator i = cpus.begin();
            cpuaff::cpu_set::iterator iend = cpus.end();
            cpuaff::cpu_set::iterator j = original_affinity.begin();
            cpuaff::cpu_set::iterator jend = original_affinity.end();
        
            for (; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
#endif

        cpuaff::cpu_set new_affinity;
        new_affinity.insert(*original_affinity.begin());
        cpus.clear();

        // set the thread's affinity to a single core
        REQUIRE(stack.set_affinity(new_affinity));
        REQUIRE(stack.get_affinity(cpus));
        
#if !defined(USING_TRAVIS_CI)
        {
            cpuaff::cpu_set::iterator i = cpus.begin();
            cpuaff::cpu_set::iterator iend = cpus.end();
            cpuaff::cpu_set::iterator j = new_affinity.begin();
            cpuaff::cpu_set::iterator jend = new_affinity.end();
    
            for (; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
#endif

        cpus.clear();
        
        // pop the affinity off the top of the stack and test that it is the
        // same as the original affinity
        REQUIRE(stack.pop_affinity());
        REQUIRE(stack.get_affinity(cpus));

#if !defined(USING_TRAVIS_CI)
        {
            cpuaff::cpu_set::iterator i = cpus.begin();
            cpuaff::cpu_set::iterator iend = cpus.end();
            cpuaff::cpu_set::iterator j = original_affinity.begin();
            cpuaff::cpu_set::iterator jend = original_affinity.end();
    
            for (; i != iend && j != jend; ++i, ++j)
            {
                REQUIRE((*i) == (*j));
            }
        }
#endif
    }
}

TEST_CASE("round_robin_allocator", "[round_robin_allocator]")
{
    SECTION("round_robin_allocator member functions")
    {
        cpuaff::affinity_manager manager;

        REQUIRE(manager.has_cpus());

        cpuaff::cpu_set cpus;
        cpuaff::cpu_set allocated_cpus;
        cpuaff::cpu cpu;

        REQUIRE(manager.get_cpus(cpus));

        cpuaff::round_robin_allocator allocator(cpus);

        for (int i = 0; i < cpus.size() * 2; ++i)
        {
            cpu = allocator.allocate();
            bool test = cpu.socket() >= 0 && cpu.core() >= 0 &&
                        cpu.processing_unit() >= 0;
            REQUIRE(test);
        }

        REQUIRE(allocator.allocate(allocated_cpus, 4));

        bool test =
            allocated_cpus.size() == 4 ||
            (allocated_cpus.size() < 4 && allocated_cpus.size() == cpus.size());
        REQUIRE(test);
    }
}

TEST_CASE("native_cpu_mapper", "[native_cpu_mapper]")
{
    SECTION("native_cpu_mapper member functions")
    {
#if !defined(USING_TRAVIS_CI)
        cpuaff::affinity_manager manager;

        REQUIRE(manager.has_cpus());

        cpuaff::native_cpu_mapper mapper;

        REQUIRE(mapper.initialize(manager));
        cpuaff::cpu_set cpus;
        manager.get_cpus(cpus);

        cpuaff::cpu_set::iterator i = cpus.begin();
        cpuaff::cpu_set::iterator iend = cpus.end();

        for (; i != iend; ++i)
        {
            cpuaff::native_cpu_mapper::native_cpu_wrapper_type native;
            cpuaff::cpu cpu;

            REQUIRE(mapper.get_native_from_cpu(native, *i));
            REQUIRE(mapper.get_cpu_from_native(cpu, native));
            REQUIRE(mapper.get_cpu_from_native(cpu, native.get()));
        }
#endif
    }
}
