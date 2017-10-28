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
#include "pci_device_description.hpp"
#include "pci_device_spec.hpp"
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace cpuaff
{
class pci_name_resolver
{
   public:
    /*!
     * Constructs a pci_name_resolver with the given file.  The given file
     * should be a pci.ids file from the
     * (PCI ID Project)[http://pci-ids.ucw.cz].
     *
     * \param file the file name to read
     */
    inline pci_name_resolver(const std::string &file) { initialize(file); }

    /*!
     * Get the pci_device_description for the given pci_device_spec.
     *
     * \param des [out] the pci_device_description
     * \param spec [in] the pci_device_spec
     * \return true if the description can be found, false otherwise.
     */
    inline bool get_description(pci_device_description &des,
                                const pci_device_spec &spec)
    {
        std::map< pci_device_spec, pci_device_description >::iterator i =
            description_by_id_.find(spec);

        if (i != description_by_id_.end())
        {
            des = i->second;
            return true;
        }
        else
        {
            i = description_by_id_.find(pci_device_spec(spec.vendor(), 0));

            if (i != description_by_id_.end())
            {
                des = i->second;
                std::ostringstream buf;
                buf << std::hex << std::setfill('0') << std::setw(4)
                    << spec.device();
                des.device_description(buf.str());
                return true;
            }
            else
            {
                std::ostringstream buf;
                buf << std::hex << std::setfill('0') << std::setw(4)
                    << spec.vendor();
                des.vendor_description(buf.str());
                buf.str("");
                buf << std::hex << std::setfill('0') << std::setw(4)
                    << spec.device();
                des.device_description(buf.str());
                return true;
            }
        }
    }

   private:
    /*!
     * Initializes a pci_name_resolver with the given file.  The given file
     * should be a pci.ids file from the
     * (PCI ID Project)[http://pci-ids.ucw.cz].
     *
     * \param file the file name to read
     * \return true if the file could be read, false otherwise.
     */
    inline bool initialize(const std::string &file)
    {
        std::ifstream infile(file.c_str());
        bool retval = infile.good();

        int32_t vendor = 0;
        std::string vendor_description;
        int32_t device = 0;
        std::string device_description;

        while (infile.good())
        {
            std::string line;
            std::getline(infile, line);

            if (line.length() > 2 && line[0] != '#' &&
                line.substr(0, 2) != "C ")
            {
                if (line[0] == '\t')
                {
                    if (line[1] != '\t')
                    {
                        if (parse_device(line, device, device_description))
                        {
                            description_by_id_[pci_device_spec(vendor,
                                                               device)] =
                                pci_device_description(vendor_description,
                                                       device_description);
                        }
                        else
                        {
                            device = 0;
                            device_description = "";
                        }
                    }
                }
                else
                {
                    if (parse_vendor(line, vendor, vendor_description))
                    {
                        description_by_id_[pci_device_spec(vendor, 0)] =
                            pci_device_description(vendor_description, "");
                    }
                    else
                    {
                        vendor = 0;
                        vendor_description = "";
                    }

                    device = 0;
                    device_description = "";
                }
            }
        }

        return retval;
    }

    inline bool parse_vendor(const std::string &line,
                             int32_t &vendor,
                             std::string &vendor_description)
    {
        if (line.length() > 6)
        {
            vendor = int32_t(strtol(line.substr(0, 4).c_str(), NULL, 16));
            vendor_description = line.substr(6);
            return true;
        }

        return false;
    }

    inline bool parse_device(const std::string &line,
                             int32_t &device,
                             std::string &device_description)
    {
        if (line.length() > 7)
        {
            device = int32_t(strtol(line.substr(1, 4).c_str(), NULL, 16));
            device_description = line.substr(7);
            return true;
        }

        return false;
    }

   private:
    std::map< pci_device_spec, pci_device_description > description_by_id_;
};

}  // namespace cpuaff
