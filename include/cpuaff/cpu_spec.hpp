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
#include <stdlib.h>
#include <sstream>

namespace cpuaff
{
/*!
 * A class representing a cpu by its socket, core, and processing unit.  For
 * instance, the first processing unit on the first core of the first socket
 * is represented as socket = 0, core = 0, processing unit = 0.
 */
class cpu_spec
{
   public:
    /*!
     * Constructs a cpu_spec with socket = -1, core = -1, processing unit = -1.
     * This will never be a legal CPU.
     */
    inline cpu_spec() : socket_(-1), core_(-1), processing_unit_(-1) {}

    /*!
     * Constructs a cpu_spec with the given socket, core, and processing unit.
     *
     * \param s Zero based socket
     * \param c Zero based core
     * \param h Zero based processing unit (hyperthread)
     */
    inline cpu_spec(const socket_type &s,
                    const core_type &c,
                    const processing_unit_type &h)
        : socket_(s), core_(c), processing_unit_(h)
    {
    }

   public:
    /*!
     * Parse a string into a cpu_spec
     *
     * \param rhs the string to parse
     *
     * \return a cpu_spec parsed from the string
     */
    static inline cpu_spec parse(const std::string &rhs)
    {
        std::istringstream buf(rhs);
        cpu_spec spec;
        buf >> spec;
        return spec;
    }

    /*!
     * Get the zero based socket identifier for this cpu_spec.
     *
     * \return the zero based socket identifier
     */
    const inline socket_type &socket() const { return socket_; }

    /*!
     * Get the zero based core identifier for this cpu_spec
     *
     * \return the zero based core identifier
     */
    const inline core_type &core() const { return core_; }

    /*!
     * Get the zero based processing unit identifier for this cpu_spec.
     *
     * \return the zero based processing unit identifier
     */
    const inline processing_unit_type &processing_unit() const
    {
        return processing_unit_;
    }

    /*!
     * Set the zero based socket identifier for this cpu_spec.
     *
     * \param socket the zero based socket identifier for this cpu_spec
     */
    inline void socket(const socket_type &socket) { socket_ = socket; }

    /*!
     * Set the zero based core identifier for this cpu_spec
     *
     * \param core the zero based core identifier for this cpu_spec
     */
    inline void core(const core_type &core) { core_ = core; }

    /*!
     * Set the zero based processing unit identifier for this cpu_spec
     *
     * \param processing_unit the zero based processing unit identifier for this
     *        cpu_spec
     */
    inline void processing_unit(const processing_unit_type &processing_unit)
    {
        processing_unit_ = processing_unit;
    }

    /*!
     * Less than operator to allow cpu_spec to be a key in stl maps/sets
     *
     * \param rhs the cpu_spec to compare to
     * \return true if this cpu_spec compares less than rhs.  false otherwise.
     */
    inline bool operator<(const cpu_spec &rhs) const
    {
        if (socket_ < rhs.socket_)
        {
            return true;
        }
        else if (rhs.socket_ < socket_)
        {
            return false;
        }
        else
        {
            if (core_ < rhs.core_)
            {
                return true;
            }
            else if (rhs.core_ < core_)
            {
                return false;
            }
            else
            {
                return processing_unit_ < rhs.processing_unit_;
            }
        }
    }

    /*!
     * Equality operator
     *
     * \param rhs the basic_cpu to compare to
     * \return true if this basic_cpu has the same socket, core, and processing
               unit as rhs, false otherwise.
     */
    inline bool operator==(const cpu_spec &rhs) const
    {
        return (socket_ == rhs.socket_ && core_ == rhs.core_ &&
                processing_unit_ == rhs.processing_unit_);
    }
    /*!
    *  Stream in operator
    *
    * \parse a string triplet of socket,core,processing_unit into a cpu spec
    */
    friend inline std::istream &operator>>(std::istream &s, cpu_spec &rhs)
    {
        s >> rhs.socket_;
        s.get();
        s >> rhs.core_;
        s.get();
        s >> rhs.processing_unit_;

        return s;
    }
    /*!
    *  Stream out operator
    *
    * \ stream out socket,core,processing_unit
    */

    friend inline std::ostream &operator<<(std::ostream &s, const cpu_spec &rhs)
    {
        s << rhs.socket_ << "," << rhs.core_ << "," << rhs.processing_unit_;
        return s;
    }

   private:
    socket_type socket_;
    core_type core_;
    processing_unit_type processing_unit_;
};

}  // namespace cpuaff
