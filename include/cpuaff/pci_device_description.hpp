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

#include "fwd.hpp"
#include <string>
#include <iostream>

namespace cpuaff
{
/*!
 * A class representing a description of a pci_device keyed by vendor and
 * device
 */
class pci_device_description
{
   public:
    /*!
     * Construct an empty pci_device_description.
     */
    inline pci_device_description() {}

    /*!
     * Construct a pci_device_description with the given vendor description
     * and device description.
     *
     * \param vendor_description the vendor description
     * \param device_description the device description
     */
    inline pci_device_description(const std::string &vendor_description,
                                  const std::string &device_description)
        : vendor_description_(vendor_description)
        , device_description_(device_description)
    {
    }

    /*!
     * Get the vendor description.
     *
     * \return the vendor description
     */
    inline const std::string &vendor_description() const
    {
        return vendor_description_;
    }

    /*!
     * Get the device description.
     *
     * \return the device description
     */
    inline const std::string &device_description() const
    {
        return device_description_;
    }

    /*!
     * Set the vendor description.
     *
     * \param val the vendor description
     */
    inline void vendor_description(const std::string &val)
    {
        vendor_description_ = val;
    }

    /*!
     * Set the device description.
     *
     * \param val the device description
     */
    inline void device_description(const std::string &val)
    {
        device_description_ = val;
    }

   private:
    std::string vendor_description_;
    std::string device_description_;
};
}  // namespace cpuaff

inline std::ostream &operator<<(std::ostream &s,
                         const cpuaff::pci_device_description &dev)
{
    s << dev.vendor_description() << ":" << dev.device_description();
    return s;
}
