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
#include "../cpu_spec.hpp"
#include "../fwd.hpp"
#include <iostream>

namespace cpuaff
{
namespace impl
{
/*!
 * basic_cpu is a representation of a cpu on the system.  It contains all the
 * data needed to get and set thread affinity as well data specifying the
 * hardware layout of the cpu.
 */
template < typename TRAITS >
class basic_cpu
{
   public:
    typedef typename TRAITS::cpu_identifier_type cpu_identifier_type;
    typedef typename TRAITS::cpu_identifier_wrapper_type
        cpu_identifier_wrapper_type;

   public:
    /*!
     * Constructs a basic_cpu with invalid values for all member variables
     */
    inline basic_cpu() {}

    /*!
     * Constructs a basic_cpu with the given cpu_spec, id, and numa.
     *
     * \param spec the cpu_spec (socket, core, processing unit) for this cpu
     * \param id the native identifier for this cpu
     * \param the numa node identifier for this cpu
     */
    inline basic_cpu(const cpu_spec &spec,
                     const cpu_identifier_wrapper_type &id,
                     const numa_type &numa)
        : spec_(spec), id_(id), numa_(numa)
    {
    }

   public:
    /*!
     * Get the zero based numa node identifier for this cpu.
     *
     * \return the zero based numa node identifier for this cpu
     */
    const inline numa_type &numa() const { return numa_; }

    /*!
     * Get the zero based socket identifier for this cpu.
     *
     * \return the zero based socket identifier
     */
    const inline socket_type &socket() const { return spec_.socket(); }

    /*!
     * Get the zero based core identifier for this cpu.
     *
     * \return the zero based core identifier
     */
    const inline core_type &core() const { return spec_.core(); }

    /*!
     * Get the zero based processing unit identifier for this cpu.
     *
     * \return the zero based processing unit identifier
     */
    const inline processing_unit_type &processing_unit() const
    {
        return spec_.processing_unit();
    }

    /*!
     * Get the cpu_spec for this cpu
     *
     * \return the cpu_spec for this cpu
     */
    const inline cpu_spec &spec() const { return spec_; }

    /*!
     * Get the native cpu identifier wrapper for this cpu.
     *
     * \return the native cpu identifier wrapper for this cpu
     */
    const inline cpu_identifier_wrapper_type &id() const { return id_; }

    /*!
     * Less than operator to allow basic_cpu to be a key in stl maps/sets
     *
     * \param rhs the basic_cpu to compare to
     * \return true if this basic_cpu compares less than rhs.  false otherwise.
     */
    inline bool operator<(const basic_cpu &rhs) const
    {
        return spec_ < rhs.spec_;
    }

    /*!
     * Equality operator
     *
     * \param rhs the basic_cpu to compare to
     * \return true if this basic_cpu has the same socket, core, and processing
               unit as rhs, false otherwise.
     */
    inline bool operator==(const basic_cpu &rhs) const
    {
        return spec_ == rhs.spec_;
    }

    /*!
     * Stream operator
     *
     * Print [id, numa, socket, core, processing unit]
     *
     */
    friend std::ostream &operator<<(std::ostream &s,
                         const basic_cpu &cpu)
    {
        s << "[id: " << cpu.id().get() << ", numa: " << cpu.numa()
          << ", socket: " << cpu.socket() << ", core: " << cpu.core()
          << ", processing_unit: " << cpu.processing_unit() << "]";
        return s;
    }

   private:
    cpu_spec spec_;
    cpu_identifier_wrapper_type id_;
    numa_type numa_;
};
}  // namespace impl
}  // namespace cpuaff