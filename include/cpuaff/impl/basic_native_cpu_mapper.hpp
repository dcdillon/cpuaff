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
#include "basic_affinity_manager.hpp"
#include "basic_cpu.hpp"
#include "basic_cpu_set.hpp"
#include <map>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_native_cpu_mapper is a utility class that maps native cpu
 * representations to cpus from a basic_cpu_manager.  It may not be available
 * for every platform.
 */
template < typename TRAITS >
class basic_native_cpu_mapper
{
   public:
    typedef typename TRAITS::cpu_identifier_type cpu_identifier_type;
    typedef typename TRAITS::cpu_identifier_wrapper_type
        cpu_identifier_wrapper_type;

    typedef typename TRAITS::native_cpu_type native_cpu_type;
    typedef typename TRAITS::native_cpu_wrapper_type native_cpu_wrapper_type;
    typedef typename TRAITS::native_get_affinity_type native_get_affinity_type;

    typedef basic_affinity_manager< TRAITS > affinity_manager_type;
    typedef basic_cpu< TRAITS > cpu_type;
    typedef basic_cpu_set< TRAITS > cpu_set_type;

   public:
    /*!
     * Constructs an uninitialized basic_native_cpu_mapper.
     */
    inline basic_native_cpu_mapper() {}

    /*!
     * Initializes a basic_native_cpu_mapper from the given affinity_manager.
     * This function sets the affinity of the calling thread consecutively to
     * every cpu on the system.  If this isn't possible, the initialization will
     * fail, or worse, hang.
     *
     * \param affinity_manager the affinity_manager to load configured cpus from
     * \return true if initialization succeeds, false otherwise
     */
    inline bool initialize(const affinity_manager_type &affinity_manager)
    {
        bool retval = true;
        cpu_set_type cpus;

        if (affinity_manager.get_cpus(cpus))
        {
            cpu_set_type orig;

            if (affinity_manager.get_affinity(orig))
            {
                typename cpu_set_type::iterator i = cpus.begin();
                typename cpu_set_type::iterator iend = cpus.end();

                for (; i != iend; ++i)
                {
                    cpu_set_type affinity;
                    affinity.insert(*i);

                    if (affinity_manager.set_affinity(affinity))
                    {
                        std::set< native_cpu_wrapper_type > ids;

                        if (native_get_affinity_type()(ids) && ids.size() == 1)
                        {
                            cpu_by_native_[*ids.begin()] = *i;
                            native_by_cpu_[*i] = *ids.begin();
                        }
                        else
                        {
                            retval = false;
                            break;
                        }
                    }
                    else
                    {
                        retval = false;
                    }
                }

                affinity_manager.set_affinity(orig);
            }
            else
            {
                retval = false;
            }
        }
        else
        {
            retval = false;
        }

        return retval;
    }

    /*!
     * Get the cpu with the given native identifier.
     *
     * \param cpu [out] the cpu with the given native identifier
     * \param native [in] the native cpu identifier
     * \return true if the cpu is found, false otherwise.
     */
    inline bool get_cpu_from_native(cpu_type &cpu,
                                    const native_cpu_wrapper_type &native) const
    {
        typename std::map< native_cpu_wrapper_type, cpu_type >::const_iterator
            i = cpu_by_native_.find(native);

        if (i != cpu_by_native_.end())
        {
            cpu = i->second;
            return true;
        }

        return false;
    }

    /*!
     * Get the cpu with the given native identifier.
     *
     * \param cpu [out] the cpu with the given native identifier
     * \param native [in] the native cpu identifier
     * \return true if the cpu is found, false otherwise.
     */
    inline bool get_cpu_from_native(cpu_type &cpu,
                                    const native_cpu_type &native) const
    {
        return get_cpu_from_native(cpu, native_cpu_wrapper_type(native));
    }

    /*!
     * Get the native identifier for the given cpu.
     *
     * \param native [out] native cpu identifier wrapper
     * \param cpu [in] the cpu
     * \return true if the native identifier is found, false otherwise.
     */

    inline bool get_native_from_cpu(native_cpu_wrapper_type &native,
                                    const cpu_type &cpu) const
    {
        typename std::map< cpu_type, native_cpu_wrapper_type >::const_iterator
            i = native_by_cpu_.find(cpu);

        if (i != native_by_cpu_.end())
        {
            native = i->second;
            return true;
        }

        return false;
    }

   private:
    std::map< native_cpu_wrapper_type, cpu_type > cpu_by_native_;
    std::map< cpu_type, native_cpu_wrapper_type > native_by_cpu_;
};
}  // namespace impl
}  // namespace cpuaff
