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
#include <queue>
#include <map>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_round_robin_allocator is a utility class that takes a set of cpus
 * and returns them as requested in a round-robin fashion.  It organizes the
 * cpus such that it returns consecutive cpus from different cores if it can.
 */
template < typename TRAITS >
class basic_round_robin_allocator
{
   public:
    typedef basic_cpu< TRAITS > cpu_type;
    typedef basic_cpu_set< TRAITS > cpu_set_type;

   public:
    /*!
     * Constructs a basic_round_robin_allocator with the given set of cpus.
     *
     * \param cpus the set of cpus that this allocator should iterate over
     */
    inline basic_round_robin_allocator(const cpu_set_type &cpus)
    {
        initialize(cpus);
    }

    /*!
     * Get the next cpu in the round-robin.
     *
     * \return the next cpu in the round robin
     */
    inline cpu_type allocate()
    {
        cpu_type retval = cpu_queue_.front();
        cpu_queue_.pop();
        cpu_queue_.push(retval);
        return retval;
    }

    /*!
     * Get the next count cpus in the round-robin.
     *
     * \param cpus [out] the set of the next count cpus in the round-robin
     * \param count [in] the number of cpus to return.  If this number is
     *                   greater than the total number of cpus in the
     *                   round-robin just the full set will be returned.
     */
    inline bool allocate(cpu_set_type &cpus, uint32_t count)
    {
        cpus.clear();

        for (uint32_t i = 0; i < count; ++i)
        {
            cpus.insert(allocate());
        }

        return true;
    }

   private:
    /*!
     * Initializes a basic_round_robin_allocator with the given set of cpus.
     *
     * \param cpus the set of cpus that this allocator should iterate over
     * \return true if there are cpus in the cpu set, false otherwise
     */
    inline bool initialize(const cpu_set_type &cpus)
    {
        std::map< processing_unit_type, cpu_set_type > cpus_by_pu;

        typename cpu_set_type::iterator i = cpus.begin();
        typename cpu_set_type::iterator iend = cpus.end();

        for (; i != iend; ++i)
        {
            cpus_by_pu[i->processing_unit()].insert(*i);
        }

        typename std::map< processing_unit_type, cpu_set_type >::const_iterator
            j = cpus_by_pu.begin();
        typename std::map< processing_unit_type, cpu_set_type >::const_iterator
            jend = cpus_by_pu.end();

        for (; j != jend; ++j)
        {
            typename cpu_set_type::iterator k = j->second.begin();
            typename cpu_set_type::iterator kend = j->second.end();

            for (; k != kend; ++k)
            {
                cpu_queue_.push(*k);
            }
        }

        return !!cpus.size();
    }

   private:
    std::queue< cpu_type > cpu_queue_;
};
}  // namespace impl
}  // namespace cpuaff