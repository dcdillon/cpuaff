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
#include <stack>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_affinity_stack is used to keep track of affinities as you get and set
 * them.  It is basically a wrapper for an affinity_manager that gives you the
 * ability to keep track of what affinities have been and reset them later.
 */
template < typename TRAITS >
class basic_affinity_stack
{
   public:
    typedef basic_affinity_manager< TRAITS > affinity_manager_type;
    typedef basic_cpu_set< TRAITS > cpu_set_type;

   public:
    /*!
     * Construct a basic_affinity stack for the given affinity_manager
     *
     * \param affinity_manager the affinity_manager that this affinity stack
     *                          should use for getting and setting affinities.
     */
    inline basic_affinity_stack(affinity_manager_type &affinity_manager)
        : affinity_manager_(affinity_manager)
    {
    }

    /*!
     * Push the current cpu affinity onto the stack.
     *
     * \return true if successful, false otherwise.
     */
    inline bool push_affinity()
    {
        cpu_set_type cpus;
        if (affinity_manager_.get_affinity(cpus))
        {
            affinity_stack_.push(cpus);
            return true;
        }

        return false;
    }

    /*!
     * Pops a previously pushed affinity off the stack and sets the current
     * thread's affinity to the popped affinity.
     *
     * \return true if successful, false otherwise.
     */
    inline bool pop_affinity()
    {
        if (!affinity_stack_.empty())
        {
            cpu_set_type cpus = affinity_stack_.top();
            affinity_stack_.pop();
            return affinity_manager_.set_affinity(cpus);
        }

        return false;
    }

    /*!
     * Get the affinity of the calling thread
     *
     * \param cpus [out] set of cpus that this thread can run on
     * \return true if the affinity could be determined, false otherwise.
     */
    inline bool get_affinity(cpu_set_type &cpus)
    {
        return affinity_manager_.get_affinity(cpus);
    }

    /*!
     * Set the affinity of the calling thread.
     *
     * \param cpus [in] the set of cpus that this thread can run on
     * \return true if the affinity could be set, false otherwise.
     */
    inline bool set_affinity(const cpu_set_type &cpus)
    {
        return affinity_manager_.set_affinity(cpus);
    }

   private:
    affinity_manager_type &affinity_manager_;
    std::stack< cpu_set_type > affinity_stack_;
};
}  // namespace impl
}  // namespace cpuaff
