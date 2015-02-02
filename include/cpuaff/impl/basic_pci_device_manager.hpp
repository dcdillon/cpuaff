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

#if defined(CPUAFF_PCI_SUPPORTED)

#include "../config.hpp"
#include "basic_pci_device.hpp"
#include "basic_pci_device_set.hpp"
#include <map>
#include <set>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_pci_device_manager is a collection of all the pci devices on the
 * system.
 */
template < typename TRAITS >
class basic_pci_device_manager
{
   public:
    typedef typename TRAITS::pci_address_type pci_address_type;
    typedef typename TRAITS::pci_address_wrapper_type pci_address_wrapper_type;
    typedef typename TRAITS::pci_loader_type pci_loader_type;
    typedef typename TRAITS::pci_loader_vector_type pci_loader_vector_type;

    typedef basic_pci_device< TRAITS > pci_device_type;
    typedef basic_pci_device_set< TRAITS > pci_device_set_type;

   public:
    /*!
     * Construct an uninitialized basic_affinity_manager.
     */
    inline basic_pci_device_manager() : loaded_pci_(false) { initialize(); }

    /*!
     * Check if this basic_affinity_manager has been successfully initialized
     * and has cpus defined.
     *
     * \return true if cpu initialization succeeded, false otherwise.
     */
    inline bool has_pci_devices() const { return loaded_pci_; }

    /*!
     * Get all the pci devices.
     *
     * \param devices [out] the set of pci devices
     * \return true if devices are found, false otherwise.
     */
    inline bool get_pci_devices(pci_device_set_type &devices) const
    {
        devices.clear();

        if (has_pci_devices())
        {
            devices = pci_devices_;
        }

        return !devices.empty();
    }

    /*!
     * Get the pci device with the given address
     *
     * \param device [out] the pci device
     * \param address [in] the address
     * \return true if the device is found, false otherwise.
     */
    inline bool get_pci_device_for_address(
        pci_device_type &device, const pci_address_wrapper_type &address)
    {
        bool retval = false;

        if (has_pci_devices())
        {
            typename std::map< pci_address_wrapper_type,
                               pci_device_type >::iterator i =
                pci_device_by_address_.find(address);

            if (i != pci_device_by_address_.end())
            {
                device = i->second;
                retval = true;
            }
        }

        return retval;
    }

    /*!
     * Get the pci device with the given address
     *
     * \param device [out] the pci device
     * \param address [in] the address
     * \return true if the device is found, false otherwise.
     */
    inline bool get_pci_device_for_address(pci_device_type &device,
                                           const pci_address_type &address)
    {
        return get_pci_device_for_address(device,
                                          pci_address_wrapper_type(address));
    }

    /*!
     * Get the pci devices with the given pci_device_spec
     *
     * \param devices [out] the set of pci devices
     * \param spec [in] the pci_device_spec
     * \return true if devices are found, false otherwise.
     */
    inline bool get_pci_devices_by_spec(pci_device_set_type &devices,
                                        const pci_device_spec &spec)
    {
        devices.clear();

        if (has_pci_devices())
        {
            typename std::map< pci_device_spec, pci_device_set_type >::iterator
                i = pci_devices_by_spec_.find(spec);

            if (i != pci_devices_by_spec_.end())
            {
                devices = i->second;
            }
        }

        return !devices.empty();
    }

    /*!
     * Get the pci devices on the given numa node
     *
     * \param devices [out] the set of pci devices
     * \param numa [in] the numa node identifier
     * \return true if devices are found, false otherwise.
     */
    inline bool get_pci_devices_by_numa(pci_device_set_type &devices,
                                        const numa_type &numa)
    {
        devices.clear();

        if (has_pci_devices())
        {
            typename std::map< numa_type, pci_device_set_type >::iterator i =
                pci_devices_by_numa_.find(numa);

            if (i != pci_devices_by_numa_.end())
            {
                devices = i->second;
            }
        }

        return !devices.empty();
    }

    /*!
     * Get the pci devices with the given vendor
     *
     * \param devices [out] the set of pci devices
     * \param vendor [in] the vendor id
     * \return true if devices are found, false otherwise.
     */
    inline bool get_pci_devices_by_vendor(pci_device_set_type &devices,
                                          const pci_vendor_id_type &vendor)
    {
        devices.clear();

        if (has_pci_devices())
        {
            typename std::map< pci_vendor_id_type,
                               pci_device_set_type >::iterator i =
                pci_devices_by_vendor_.find(vendor);

            if (i != pci_devices_by_vendor_.end())
            {
                devices = i->second;
            }
        }

        return !devices.empty();
    }

   private:
    /*!
     * Initialize a basic_pci_device_manager.  This reads the hardware layout
     * and creates the pci mappings.
     *
     * \return true if initialization is successful, false otherwise.
     */
    inline bool initialize()
    {
        bool retval = true;

        pci_loader_vector_type devices;
        loaded_pci_ = pci_loader_type()(devices);
        retval = retval && loaded_pci_;

        typename pci_loader_vector_type::iterator k = devices.begin();
        typename pci_loader_vector_type::iterator kend = devices.end();

        for (; k != kend; ++k)
        {
            pci_device_type device(k->spec, k->address, k->numa);
            pci_device_by_address_[k->address] = device;
            pci_devices_.insert(device);
            pci_devices_by_numa_[k->numa].insert(device);
            pci_devices_by_spec_[k->spec].insert(device);
            pci_devices_by_vendor_[k->spec.vendor()].insert(device);
        }

        return retval;
    }

   private:
    pci_device_set_type pci_devices_;
    std::map< numa_type, pci_device_set_type > pci_devices_by_numa_;
    std::map< pci_device_spec, pci_device_set_type > pci_devices_by_spec_;
    std::map< pci_address_wrapper_type, pci_device_type >
        pci_device_by_address_;
    std::map< pci_vendor_id_type, pci_device_set_type > pci_devices_by_vendor_;
    bool loaded_pci_;
};
}  // namespace impl
}  // namespace cpuaff

template < typename TRAITS >
std::ostream &operator<<(
    std::ostream &s, const cpuaff::impl::basic_pci_device_manager< TRAITS > &m)
{
    cpuaff::impl::basic_pci_device_set< TRAITS > devices;
    m.get_pci_devices(devices);

    typename cpuaff::impl::basic_pci_device_set< TRAITS >::iterator j =
        devices.begin();
    typename cpuaff::impl::basic_pci_device_set< TRAITS >::iterator jend =
        devices.end();

    for (; j != jend; ++j)
    {
        s << (*j) << std::endl;
    }

    return s;
}

#endif
