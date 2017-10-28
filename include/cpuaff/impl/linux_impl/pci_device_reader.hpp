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
#include "../../pci_device_spec.hpp"
#include "set_reader.hpp"
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace cpuaff
{
namespace impl
{
namespace linux_impl
{
class pci_device_reader
{
   public:
    struct raw_pci_info
    {
        pci_device_spec spec;
        numa_type numa;
        std::string address;

        inline raw_pci_info() : numa(-1) {}
        inline raw_pci_info(const pci_device_spec &s,
                            const numa_type &n,
                            const std::string &a)
            : spec(s), numa(n), address(a)
        {
        }
    };

   public:
    inline bool load()
    {
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir("/sys/bus/pci/devices")) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                load_device(ent->d_name);
            }

            closedir(dir);

            return true;
        }

        return false;
    }

    inline const std::vector< raw_pci_info > &devices() const
    {
        return devices_;
    }

   private:
    inline bool load_device(const std::string &address)
    {
        raw_pci_info info;
        info.address = address;

        if (!read_numa(info))
        {
            info.numa = -1;
        }

        if (read_device_spec(info))
        {
            devices_.push_back(info);
            return true;
        }

        return false;
    }

    inline bool read_numa(raw_pci_info &info)
    {
        bool retval = false;
        std::ostringstream buf;
        buf << "/sys/bus/pci/devices/" << info.address << "/numa_node";

        std::ifstream infile(buf.str().c_str());

        if (infile.good())
        {
            std::string line;
            std::getline(infile, line);

            info.numa = atoi(line.c_str());
            retval = true;

            infile.close();
        }

        return retval;
    }

    inline bool read_device_spec(raw_pci_info &info)
    {
        bool retval = false;

        std::ostringstream buf;
        buf << "/sys/bus/pci/devices/" << info.address << "/vendor";

        std::ifstream vendorFile(buf.str().c_str());

        if (vendorFile.good())
        {
            std::string line;
            std::getline(vendorFile, line);

            info.spec.vendor(pci_vendor_id_type(strtol(line.c_str(), NULL, 0)));

            vendorFile.close();

            buf.str("");
            buf << "/sys/bus/pci/devices/" << info.address << "/device";

            std::ifstream deviceFile(buf.str().c_str());

            if (deviceFile.good())
            {
                std::getline(deviceFile, line);

                info.spec.device(
                    pci_device_id_type(strtol(line.c_str(), NULL, 0)));

                vendorFile.close();
                retval = true;
            }
        }

        return retval;
    }

   private:
    std::vector< raw_pci_info > devices_;
};

}  // namespace linux_impl
}  // namespace impl
}  // namespace cpuaff
