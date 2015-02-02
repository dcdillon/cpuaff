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

#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <string>
#include <set>
#include <vector>

namespace cpuaff
{
namespace impl
{
namespace linux_impl
{
namespace set_reader
{
inline bool read_int_set(std::set< int32_t > &set, const std::string &str)
{
    set.clear();

    std::vector< std::string > elem;

    char buf[2048];
    strcpy(buf, str.c_str());

    char *tok = strtok(buf, ",");

    while (tok)
    {
        elem.push_back(tok);
        tok = strtok(NULL, ",");
    }

    std::vector< std::string >::iterator i = elem.begin();
    std::vector< std::string >::iterator iend = elem.end();

    for (; i != iend; ++i)
    {
        if (i->find("-") == std::string::npos)
        {
            set.insert(atoi(i->c_str()));
        }
        else
        {
            char buf2[32];
            strcpy(buf2, i->c_str());

            tok = strtok(buf2, "-");

            int32_t begin = atoi(tok);

            tok = strtok(NULL, "-");

            int32_t end = atoi(tok);

            for (int32_t j = begin; j <= end; ++j)
            {
                set.insert(j);
            }
        }
    }

    return true;
}
}  // namespace set_reader
}  // namespace linux_impl
}  // namespace impl
}  // namespace cpuaff