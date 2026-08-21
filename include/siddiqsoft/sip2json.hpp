/*
    A SIP Parser for Modern C++
    Version 2.5.x
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License

    Copyright (c) 2003-2024, Abdelkareem Siddiq
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

#ifndef SIP2JSON_HPP
#define SIP2JSON_HPP

#include <algorithm>
#include <string>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>
#include <functional>
#include <optional>
#include <format>
#include <vector>

#include "nlohmann/json.hpp"

#include "private/sip2json_exception.hpp"
#include "private/sip2json_response_codes.hpp"
#include "private/sip2json_utils.hpp"

#include "sipmessage.hpp"

namespace siddiqsoft
{
    /// @brief SIP message encoder and decoder utility class
    class sip2json final
    {
    private:
        // Named constants for magic numbers
        static constexpr size_t TYPICAL_SIP_MESSAGE_SIZE = 3 * 1024; ///< Typical SIP message buffer size
        static constexpr size_t METADATA_ONLY_SIZE       = 1;        ///< Size when only metadata is present

        static bool parseStartLine(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false);
        static std::string escapeJsonPointerToken(const std::string& token);
        static bool storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value) noexcept(false);
        static bool parseHeaders(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false);
        static bool parseBodySDP(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false);
        static std::string serializeSDP(sipmessage& sipm) noexcept(false);
        static std::string serializeSDPelement(nlohmann::json& sdpBlock, const std::string& element);

    public:
        [[nodiscard("Remaining contents of the buffer")]] static std::string& parseAsync(
                std::string&                      frameBuffer,
                std::function<void(sipmessage&&)> parseCallback,
                std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>>
                        errorCallback = {}) noexcept;

        [[nodiscard]] static std::vector<sipmessage> parse(std::string::iterator&       bufferStart,
                                                           const std::string::iterator& bufferEnd) noexcept(false);

        [[nodiscard]] static sipmessage parseFromBuffer(std::string::iterator&       bufferStart,
                                                        const std::string::iterator& bufferEnd) noexcept(false);

        static std::string serialize(sipmessage& sipm) noexcept(false);
    };
} // namespace siddiqsoft

#include "private/sip2json_sdp.hpp"
#include "private/sip2json_parser.hpp"
#include "private/sip2json_serializer.hpp"

#endif
