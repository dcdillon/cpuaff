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

#include "../config.hpp"
#include "../fwd.hpp"
#include "../pci_device_spec.hpp"
#include <iomanip>
#include <iostream>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_pci_device is a representation of a pci_device on the system.  It
 * contains identification, address, and numa information.  The device can
 * then be used to determine what cpus are local to it.
 */
template < typename TRAITS >
class basic_pci_device
{
   public:
    typedef typename TRAITS::pci_address_type pci_address_type;
    typedef typename TRAITS::pci_address_wrapper_type pci_address_wrapper_type;

   public:
    /*!
     * Constructs a basic_pci_device with invalid values for all members.
     */
    inline basic_pci_device() : numa_(-1) {}

    /*!
     * Constructs a basic_pci_device with the given values for spec, address,
     *  and numa
     *
     * \param s the pci_device_spec
     * \param a the pci address
     * \param n the numa node
     */
    inline basic_pci_device(const pci_device_spec &s,
                            const pci_address_wrapper_type &a,
                            const numa_type &n)
        : spec_(s), address_(a), numa_(n)
    {
    }

    /*!
     * Get the pci_device_spec for this device
     *
     * \return the pci_device_spec
     */
    inline const pci_device_spec &spec() const { return spec_; }

    /*!
     * Get the pci vendor id.
     *
     * \return the pci vendor id
     */
    inline const pci_vendor_id_type &vendor() const { return spec_.vendor(); }

    /*!
     * Get the pci device id.
     *
     * \return the pci device id
     */
    inline const pci_device_id_type &device() const { return spec_.device(); }

    /*!
     * Get the pci address wrapper.
     *
     * \return the pci address wrapper
     */
    inline const pci_address_wrapper_type &address() const { return address_; }

    /*!
     * Get the numa node.
     *
     * \return the numa node
     */
    inline const numa_type &numa() const { return numa_; }

    /*!
     * Less than operator to allow basic_pci_device to be a key in an stl
     * maps/sets.
     *
     * \param rhs the basic_pci_device to compare to
     * \return true if this basic_pci_device compares less than rhs, false
     *         otherwise.
     */
    inline bool operator<(const basic_pci_device &rhs) const
    {
        return address_ < rhs.address_;
    }

   private:
    pci_device_spec spec_;
    pci_address_wrapper_type address_;
    numa_type numa_;
};
}  // namespace impl
}  // namespace cpuaff

template < typename TRAITS >
std::ostream &operator<<(std::ostream &s,
                         const cpuaff::impl::basic_pci_device< TRAITS > &dev)
{
    std::ostream::fmtflags flags = s.flags();
    s << "[address: " << dev.address().get();
    s << ", numa: " << dev.numa();
    s << std::hex;
    s << ", device id: " << std::setfill('0') << std::setw(4) << dev.vendor();
    s << ":" << std::setfill('0') << std::setw(4) << dev.device() << "]";
    s.flags(flags);
    return s;
}
