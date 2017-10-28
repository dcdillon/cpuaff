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

        cpuaff::cpu cpu;
        cpuaff::cpu_set cpus;

        REQUIRE(manager.get_cpu_from_index(cpu, 0));
        REQUIRE(manager.get_cpu_from_id(cpu, cpu.id()));
        REQUIRE(manager.get_cpu_from_id(cpu, cpu.id().get()));
        REQUIRE(manager.get_cpu_from_spec(
            cpu,
            cpuaff::cpu_spec(cpu.socket(), cpu.core(), cpu.processing_unit())));

        REQUIRE(manager.get_cpus(cpus));
        REQUIRE(manager.get_cpus_by_numa(cpus, cpu.numa()));
        REQUIRE(manager.get_cpus_by_socket(cpus, cpu.socket()));
        REQUIRE(manager.get_cpus_by_core(cpus, cpu.core()));
        REQUIRE(
            manager.get_cpus_by_processing_unit(cpus, cpu.processing_unit()));
        REQUIRE(manager.get_cpus_by_socket_and_core(cpus, cpu.socket(),
                                                    cpu.core()));

        REQUIRE(manager.get_affinity(cpus));
        REQUIRE(cpus.size() > 0);

        cpuaff::cpu_set new_affinity;
        new_affinity.insert(cpu);
        REQUIRE(manager.set_affinity(new_affinity));
        REQUIRE(manager.get_affinity(cpus));

        if (cpus.size() == new_affinity.size())
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

        REQUIRE(manager.get_cpus(new_affinity));
        REQUIRE(manager.set_affinity(new_affinity));
        REQUIRE(manager.get_affinity(cpus));

        if (cpus.size() == new_affinity.size())
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

        REQUIRE(manager.pin(cpu));
        REQUIRE(manager.get_affinity(cpus));
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
        cpuaff::cpu_set new_affinity;
        cpuaff::cpu_set cpus;

        REQUIRE(stack.get_affinity(original_affinity));
        REQUIRE(original_affinity.size() > 0);
        REQUIRE(stack.push_affinity());
        REQUIRE(stack.get_affinity(cpus));
        REQUIRE(cpus.size() == original_affinity.size());

        if (cpus.size() == original_affinity.size())
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

        new_affinity.insert(*original_affinity.begin());

        REQUIRE(stack.set_affinity(new_affinity));
        REQUIRE(stack.get_affinity(cpus));

        REQUIRE(stack.pop_affinity());
        REQUIRE(stack.get_affinity(cpus));

        if (cpus.size() == original_affinity.size())
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
        cpuaff::affinity_manager manager;

        REQUIRE(manager.has_cpus());

        cpuaff::native_cpu_mapper mapper;

        if (mapper.initialize(manager))
        {
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
        }
    }
}
