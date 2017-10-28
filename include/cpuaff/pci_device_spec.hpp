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

#include "fwd.hpp"

namespace cpuaff
{
/*!
 * A class representing a pci_device by its vendor, device, subvendor, and
 * subdevice.  This is not a unique identifier for a device.
 */
class pci_device_spec
{
   public:
    /*!
     * Constructs a pci_device_spec with vendor, device, subvendor, and
     * subdevice set to 0.  This will never be a legal pci device.
     */
    inline pci_device_spec() : vendor_(0), device_(0) {}

    /*!
     * Constructs a pci_device_spec with the given vendor and device.
     *
     * \param v vendor id
     * \param d device id
     */
    inline pci_device_spec(const pci_vendor_id_type &v,
                           const pci_device_id_type &d)
        : vendor_(v), device_(d)
    {
    }

   public:
    /*!
     * Get the pci vendor id for this pci_device_spec.
     *
     * \return the pci vendor id for this pci_device_spec
     */
    inline const pci_vendor_id_type &vendor() const { return vendor_; }

    /*!
     * Get the pci device id for this pci_device_spec.
     *
     * \return the pci device id for this pci_device_spec
     */
    inline const pci_device_id_type &device() const { return device_; }

    /*!
     * Set the pci vendor id for this pci_device_spec.
     *
     * \param v the vendor id
     */
    inline void vendor(const pci_vendor_id_type &v) { vendor_ = v; }

    /*!
     * Set the pci device id for this pci_device_spec.
     *
     * \param d the device id
     */
    inline void device(const pci_device_id_type &d) { device_ = d; }

    /*!
     * Less than operator to allow pci_device_spec to be a key in stl maps/sets
     *
     * \param rhs the cpu_spec to compare to
     * \return true if this pci_device_spec compares less than rhs, false
     *         otherwise.
     */
    inline bool operator<(const pci_device_spec &rhs) const
    {
        if (vendor_ < rhs.vendor_)
        {
            return true;
        }
        else if (rhs.vendor_ < vendor_)
        {
            return false;
        }
        else
        {
            return device_ < rhs.device_;
        }
    }

   private:
    pci_vendor_id_type vendor_;
    pci_device_id_type device_;
};

}  // namespace cpuaff
