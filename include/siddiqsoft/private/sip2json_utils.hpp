/*
    A SIP Parser for Modern C++: Utilities and Helpers
    Version 2.5.x
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License

    Copyright (c) 2003-2020, Abdelkareem Siddiq
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <string>
#include <random>
#include <sstream>

#include "ctre.hpp"
#include "nlohmann/json.hpp"
#include "sip2json_header_keys.hpp"
#include "sip2json_constants.hpp"
#include "sip2json_datetime.hpp"

namespace siddiqsoft
{
    /// @brief Creates a pseudo random number generated UUID v4. It is best to use platform-specific method to ensure guid
    /// @return string 44 character of the format: 7792eaf4-456f-4d47-d93-863af0e0-a8b99b9b9988
    static std::string createCallId()
    {
        static thread_local std::random_device            rd;
        static thread_local std::mt19937_64               generator(rd());
        static thread_local std::uniform_int_distribution ud(0, 15);
        static thread_local std::uniform_int_distribution ud2(8, 11);

        std::stringstream sBuffer;

        sBuffer << std::hex;
        for (auto i = 0; i < 8; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 4; i++)
            sBuffer << ud(generator);
        sBuffer << "-4";
        for (auto i = 0; i < 3; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 3; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 8; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 12; i++)
            sBuffer << ud2(generator);
        return sBuffer.str();
    }
} // namespace siddiqsoft