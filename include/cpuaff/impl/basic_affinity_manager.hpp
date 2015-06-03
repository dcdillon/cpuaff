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

#include "../config.hpp"
#include "basic_cpu.hpp"
#include "basic_cpu_set.hpp"
#include <cassert>
#include <map>
#include <set>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_affinity_manager is a collection of all the valid cpus.  It provides
 * interfaces to get and set cpu affinity as well as ways to classify cpus
 * on the system so that intelligent choices can be made about what affinity a
 * thread should have.
 */
template < typename TRAITS >
class basic_affinity_manager
{
   public:
    typedef typename TRAITS::cpu_identifier_type cpu_identifier_type;
    typedef typename TRAITS::cpu_identifier_wrapper_type
        cpu_identifier_wrapper_type;
    typedef typename TRAITS::cpu_loader_type cpu_loader_type;
    typedef typename TRAITS::cpu_loader_vector_type cpu_loader_vector_type;
    typedef typename TRAITS::get_affinity_type get_affinity_type;
    typedef typename TRAITS::set_affinity_type set_affinity_type;
    typedef typename TRAITS::native_cpu_type native_cpu_type;
    typedef typename TRAITS::native_cpu_wrapper_type native_cpu_wrapper_type;
    typedef typename TRAITS::native_get_affinity_type native_get_affinity_type;

    typedef basic_cpu< TRAITS > cpu_type;
    typedef basic_cpu_set< TRAITS > cpu_set_type;

   public:
    /*!
     * Construct an uninitialized basic_affinity_manager.
     */
    inline basic_affinity_manager() : loaded_cpus_(false) { initialize(); }

    /*!
     * Check if this basic_affinity_manager has been successfully initialized
     * and has cpus defined.
     *
     * \return true if cpu initialization succeeded, false otherwise.
     */
    inline bool has_cpus() const { return loaded_cpus_; }

    /*!
     * Get the cpu with the given identifier.
     *
     * \param cpu [out] the cpu with the given identifier
     * \param id [in] the cpu identifier
     * \return true if the cpu is found, false otherwise.
     */
    inline bool get_cpu_from_id(cpu_type &cpu,
                                const cpu_identifier_wrapper_type &id) const
    {
        if (has_cpus())
        {
            typename std::map< cpu_identifier_wrapper_type,
                               cpu_type >::const_iterator i =
                cpu_by_id_.find(id);

            if (i != cpu_by_id_.end())
            {
                cpu = i->second;
                return true;
            }
        }

        return false;
    }

    /*!
     * Get the cpu with the given identifier.
     *
     * \param cpu [out] the cpu with the given identifier
     * \param id [in] the cpu identifier
     * \return true if the cpu is found, false otherwise.
     */
    inline bool get_cpu_from_id(cpu_type &cpu,
                                const cpu_identifier_type &id) const
    {
        return get_cpu_from_id(cpu, cpu_identifier_wrapper_type(id));
    }

    /*!
     * Get the cpu with the given cpu_spec.
     *
     * \param cpu [out] the cpu with the given cpu_spec
     * \param spec [in] the cpu_spec
     * \return true if the cpu is found, false otherwise.
     */
    inline bool get_cpu_from_spec(cpu_type &cpu, const cpu_spec &spec) const
    {
        if (has_cpus())
        {
            typename std::map< cpu_spec, cpu_type >::const_iterator i =
                cpu_by_spec_.find(spec);

            if (i != cpu_by_spec_.end())
            {
                cpu = i->second;
                return true;
            }
        }

        return false;
    }

    /*!
     * Get a cpu by arbitrary numeric index.
     *
     * \param cpu [out] the cpu with the given numeric index
     * \param i [in] the numeric index
     * \return true if the cpu is found, false otherwise.
     */
    inline bool get_cpu_from_index(cpu_type &cpu, const int32_t i) const
    {
        if (has_cpus())
        {
            if (i >= 0 && i < cpu_by_index_.size())
            {
                cpu = cpu_by_index_[i];
                return true;
            }
        }

        return false;
    }

    /*!
     * Get all the cpus.
     *
     * \param cpus [out] the set of cpus
     * \return true if cpus are found, false otherwise.
     */
    inline bool get_cpus(cpu_set_type &cpus) const
    {
        cpus.clear();

        if (has_cpus())
        {
            cpus = cpus_;
        }

        return !cpus.empty();
    }

    /*!
     * Get all the cpus for a given numa node.
     *
     * \param cpus [out] the cpus for the given numa node
     * \param numa [in] the zero based numa identifier
     * \return true if cpus are found, false otherwise
     */
    inline bool get_cpus_by_numa(cpu_set_type &cpus,
                                 const numa_type &numa) const
    {
        cpus.clear();

        if (has_cpus())
        {
            typename std::map< numa_type, cpu_set_type >::const_iterator i =
                cpus_by_numa_.find(numa);

            if (i != cpus_by_numa_.end())
            {
                cpus = i->second;
            }
        }

        return !cpus.empty();
    }

    /*!
     * Get all the cpus for the given socket and core.
     *
     * \param cpus [out] the cpus for the given socket and core
     * \param socket [in] the zero based socket identifier
     * \param core [in] the zero based core identifier
     * \return true if cpus are found, false otherwise
     */
    inline bool get_cpus_by_socket_and_core(cpu_set_type &cpus,
                                            const socket_type &socket,
                                            const core_type &core)
    {
        if (has_cpus())
        {
            typename std::map< std::pair< socket_type, core_type >,
                               cpu_set_type >::const_iterator i =
                cpus_by_socket_and_core_.find(std::make_pair(socket, core));

            if (i != cpus_by_socket_and_core_.end())
            {
                cpus = i->second;
            }

            return !cpus.empty();
        }

        return false;
    }

    /*!
     * Get all the cpus for the given socket identifier.
     *
     * \param cpus [out] the cpus for the given socket identifier
     * \param socket [in] the zero based socket identifier
     * \return true if cpus are found, false otherwise
     */
    inline bool get_cpus_by_socket(cpu_set_type &cpus,
                                   const socket_type &socket) const
    {
        cpus.clear();

        if (has_cpus())
        {
            typename std::map< socket_type, cpu_set_type >::const_iterator i =
                cpus_by_socket_.find(socket);

            if (i != cpus_by_socket_.end())
            {
                cpus = i->second;
            }
        }

        return !cpus.empty();
    }

    /*!
     * Get all the cpus for the given core identifier.  Note: core identifiers
     * are zero based per socket, so this will return the nth core on every
     * socket.
     *
     * \param cpus [out] the cpus for the given core identifier
     * \param core [in] the zero based core identifier
     */
    inline bool get_cpus_by_core(cpu_set_type &cpus,
                                 const core_type &core) const
    {
        cpus.clear();

        if (has_cpus())
        {
            typename std::map< core_type, cpu_set_type >::const_iterator i =
                cpus_by_core_.find(core);

            if (i != cpus_by_core_.end())
            {
                cpus = i->second;
            }
        }

        return !cpus.empty();
    }

    /*!
     * Get all the cpus for the given processing unit identifier.  Note:
     * processing unit identifiers are zero based per core, so this will return
     * the nth processing unit on every core.
     *
     * \param cpus [out] the cpus for the given processing unit identifier
     * \param processing_unit [in] the zero based processing unit identifier
     * \return true if cpus are found, false otherwise.
     */
    inline bool get_cpus_by_processing_unit(
        cpu_set_type &cpus, const processing_unit_type &processing_unit) const
    {
        cpus.clear();

        if (has_cpus())
        {
            typename std::map< processing_unit_type,
                               cpu_set_type >::const_iterator i =
                cpus_by_processing_unit_.find(processing_unit);

            if (i != cpus_by_processing_unit_.end())
            {
                cpus = i->second;
            }
        }

        return !cpus.empty();
    }

    /*!
     * Get the affinity of the calling thread
     *
     * \param cpus [out] set of cpus that this thread can run on
     * \return true if the affinity could be determined, false otherwise.
     */
    inline bool get_affinity(cpu_set_type &cpus) const
    {
        cpus.clear();
        std::set< cpu_identifier_wrapper_type > ids;

        if (get_affinity_type()(ids))
        {
            typename std::set< cpu_identifier_wrapper_type >::iterator i =
                ids.begin();
            typename std::set< cpu_identifier_wrapper_type >::iterator iend =
                ids.end();

            for (; i != iend; ++i)
            {
                cpu_type cpu;
                get_cpu_from_id(cpu, *i);
                cpus.insert(cpu);
            }

            return true;
        }

        return false;
    }

    /*!
     * Set the affinity of the calling thread.
     *
     * \param cpus [in] the set of cpus that this thread can run on
     * \return true if the affinity could be set, false otherwise.
     */
    inline bool set_affinity(const cpu_set_type &cpus) const
    {
        std::set< cpu_identifier_wrapper_type > ids;

        typename cpu_set_type::iterator i = cpus.begin();
        typename cpu_set_type::iterator iend = cpus.end();

        for (; i != iend; ++i)
        {
            ids.insert(i->id());
        }

        return set_affinity_type()(ids);
    }

    /*!
     * Set the affinity of the calling thread to a single cpu
     *
     * \param cpu the cpu to pin this thread to
     * \return true if the affinity could be set, false otherwise.
     */
    inline bool pin(const cpu_type &cpu)
    {
        cpu_set_type cpus;
        cpus.insert(cpu);
        return set_affinity(cpus);
    }

   private:
    /*!
     * Initialize a basic_affinity_manager.  This reads the hardware layout and
     * creates the cpu and pci mappings.
     *
     * \return true if initialization is successful.  false otherwise.
     */
    inline bool initialize()
    {
        bool retval = true;

        cpu_loader_vector_type cpus;
        loaded_cpus_ = cpu_loader_type()(cpus);

        retval = retval && loaded_cpus_;
        typename cpu_loader_vector_type::iterator i = cpus.begin();
        typename cpu_loader_vector_type::iterator iend = cpus.end();

        for (; i != iend; ++i)
        {
            cpu_type cpu(i->spec, i->id, i->numa);
            cpus_.insert(cpu);
            cpu_by_id_[i->id] = cpu;
            cpu_by_spec_[i->spec] = cpu;
            cpus_by_numa_[i->numa].insert(cpu);
            cpus_by_socket_[i->spec.socket()].insert(cpu);
            cpus_by_core_[i->spec.core()].insert(cpu);
            cpus_by_processing_unit_[i->spec.processing_unit()].insert(cpu);
        }

        typename cpu_set_type::iterator j = cpus_.begin();
        typename cpu_set_type::iterator jend = cpus_.end();

        for (; j != jend; ++j)
        {
            cpu_by_index_.push_back(*j);
        }

        return retval;
    }

   private:
    cpu_set_type cpus_;
    std::vector< cpu_type > cpu_by_index_;
    std::map< cpu_identifier_wrapper_type, cpu_type > cpu_by_id_;

    std::map< cpu_spec, cpu_type > cpu_by_spec_;
    std::map< numa_type, cpu_set_type > cpus_by_numa_;
    std::map< socket_type, cpu_set_type > cpus_by_socket_;
    std::map< core_type, cpu_set_type > cpus_by_core_;
    std::map< std::pair< socket_type, core_type >, cpu_set_type >
        cpus_by_socket_and_core_;
    std::map< processing_unit_type, cpu_set_type > cpus_by_processing_unit_;
    bool loaded_cpus_;
};
}  // namespace impl
}  // namespace cpuaff

template < typename TRAITS >
std::ostream &operator<<(
    std::ostream &s, const cpuaff::impl::basic_affinity_manager< TRAITS > &m)
{
    cpuaff::impl::basic_cpu_set< TRAITS > cpus;
    m.get_cpus(cpus);

    typename cpuaff::impl::basic_cpu_set< TRAITS >::iterator i = cpus.begin();
    typename cpuaff::impl::basic_cpu_set< TRAITS >::iterator iend = cpus.end();

    for (; i != iend; ++i)
    {
        s << (*i) << std::endl;
    }

    return s;
}
