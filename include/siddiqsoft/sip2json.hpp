/*
    A SIP Parser for Modern C++
    Version 3
    https://github.com/siddiqsoft/sip2json/

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

namespace siddiqsoft {
    /// @brief SIP message encoder and decoder utility class
    class sip2json final {
    private:
        static bool        parseStartLine(sipmessage& sipm, std::string_view& buffer) noexcept(false);
        static bool        parseStartLine(sipmessage&                  sipm,
                                          std::string::iterator&       bufferStart,
                                          const std::string::iterator& bufferEnd) noexcept(false);
        static std::string escapeJsonPointerToken(const std::string& token);
        static bool        storeHeaderValue(sipmessage& sipm, std::string_view key, std::string_view value) noexcept(false);
        static bool        storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value) noexcept(false);
        static bool        parseHeaders(sipmessage& sipm, std::string_view& buffer) noexcept(false);
        static bool
        parseHeaders(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false);
        static bool parseBodySDP(sipmessage& sipm, std::string_view& buffer) noexcept(false);
        static bool
        parseBodySDP(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false);
        static std::string serializeSDP(const sipmessage& sipm) noexcept(false);
        static std::string serializeSDPelement(const nlohmann::json& sdpBlock, const std::string& element);

    public:
        // --- String View High-Performance API ---

        /// @brief Given a non-owning buffer view, parses each message and invokes the callback with the decoded sipmessage.
        /// @param frameBuffer std::string_view reference; upon return, advanced past all successfully parsed messages.
        /// @param parseCallback Callback which takes a reference to the sipmessage just decoded.
        /// @param errorCallback Optional callback to handle errors during parsing.
        /// @return Returns the number of bytes consumed from the buffer.
        static size_t
        parseAsync(std::string_view&                                                               frameBuffer,
                   std::function<void(sipmessage&&)>                                               parseCallback,
                   std::optional<std::function<void(const sip2json_exception&, std::string_view)>> errorCallback = {}) noexcept;

        /// @brief Given a buffer view, parse all complete frames and return the vector of messages. Advances the view in-place.
        /// @param buffer std::string_view reference; upon return, advanced past all successfully parsed messages.
        /// @return Vector of sipmessage decoded within the stream.
        [[nodiscard]] static std::vector<sipmessage> parse(std::string_view& buffer) noexcept(false);

        /// @brief Given a buffer view, parse all complete frames and return the vector of messages.
        /// @param buffer std::string_view buffer.
        /// @param bytesConsumed Output parameter populated with total bytes consumed.
        /// @return Vector of sipmessage decoded within the stream.
        [[nodiscard]] static std::vector<sipmessage> parse(std::string_view buffer, size_t& bytesConsumed) noexcept(false)
        {
            size_t initialSize = buffer.size();
            auto   result      = parse(buffer);
            bytesConsumed      = initialSize - buffer.size();
            return result;
        }

        /// @brief De-serialize the *first* SIP message from the buffer view and advances the view past the message.
        /// @param buffer std::string_view reference; upon return, advanced past the consumed message.
        /// @return A sipmessage object representing the decoded message.
        [[nodiscard]] static sipmessage parseFromBuffer(std::string_view& buffer) noexcept(false);

        /// @brief De-serialize the *first* SIP message from the buffer view.
        /// @param buffer std::string_view buffer.
        /// @param bytesConsumed Output parameter populated with bytes consumed by the message.
        /// @return A sipmessage object representing the decoded message.
        [[nodiscard]] static sipmessage parseFromBuffer(std::string_view buffer, size_t& bytesConsumed) noexcept(false)
        {
            size_t initialSize = buffer.size();
            auto   result      = parseFromBuffer(buffer);
            bytesConsumed      = initialSize - buffer.size();
            return result;
        }

        // --- Backward-Compatible API ---

        [[nodiscard("Remaining contents of the buffer")]] static std::string& parseAsync(
                std::string&                      frameBuffer,
                std::function<void(sipmessage&&)> parseCallback,
                std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>>
                        errorCallback = {}) noexcept;

        [[nodiscard]] static std::vector<sipmessage> parse(std::string::iterator&       bufferStart,
                                                           const std::string::iterator& bufferEnd) noexcept(false);

        [[nodiscard]] static sipmessage parseFromBuffer(std::string::iterator&       bufferStart,
                                                        const std::string::iterator& bufferEnd) noexcept(false);

        static std::string serialize(const sipmessage& sipm) noexcept(false);
        static std::string serialize(sipmessage& sipm) noexcept(false);
    };
} // namespace siddiqsoft

#include "private/sip2json_sdp.hpp"
#include "private/sip2json_parser.hpp"
#include "private/sip2json_serializer.hpp"

#endif
