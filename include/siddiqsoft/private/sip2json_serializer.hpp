/*
    A SIP Parser for Modern C++: SIP Message Serializer Implementation
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

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iterator>
#include <format>

#include "nlohmann/json.hpp"
#include "sip2json_exception.hpp"
#include "sip2json_utils.hpp"
#include "../sipmessage.hpp"

namespace siddiqsoft
{
    /// @brief Serializes the sipmessage document
    /// @param sipm Source sipmessage
    /// @return Return serialized sipmessage
    inline std::string sip2json::serialize(sipmessage& sipm) noexcept(false)
    {
        using namespace std;

        static const std::vector<std::string_view> supportedMethodsList {METHOD_MESSAGE,
                                                                         METHOD_INFO,
                                                                         METHOD_INVITE,
                                                                         METHOD_ACK,
                                                                         METHOD_OPTIONS,
                                                                         METHOD_BYE,
                                                                         METHOD_CANCEL,
                                                                         METHOD_REGISTER,
                                                                         METHOD_SUBSCRIBE,
                                                                         METHOD_NOTIFY,
                                                                         METHOD_REFER,
                                                                         METHOD_PUBLISH,
                                                                         METHOD_UPDATE,
                                                                         METHOD_PRACK,
                                                                         SIPVER_20};
        std::string                                buffer {};
        std::string                                contentType {};

        // Reserve the size of a typical SIP Message. Typical message size of 3K
        buffer.reserve(3 * 1024);

        // Assert: non-empty json document
        if (sipm.size() == 0) throw empty_message_error {std::format("{}:sipm is empty.", __func__)};
        // Assert: non-empty json document; starting with v1.9 we have a meta element for diagnostics; this is to be treated as "empty".
        if (sipm.contains(JSON_KEY_META) && sipm.size() == 1)
            throw empty_message_error {std::format("{}:sipm is empty (except for meta).", __func__)};

        // Assert: Header must exist
        if (!sipm.contains(JSON_KEY_HEADERS)) throw invalid_document_error {std::format("{}:sipm does not contain `h`eaders.", __func__)};

        if (sipm.isMessageRequest())
        {
            auto method = sipm.getMethod();
            auto uri    = sipm.getUri();

            if (method.find('\r') != std::string::npos || method.find('\n') != std::string::npos ||
                std::find(supportedMethodsList.begin(), supportedMethodsList.end(), method) == supportedMethodsList.end())
                throw invalid_document_error {std::format("{}:Unsupported method:{}", __func__, method)};

            if (uri.find('\r') != std::string::npos || uri.find('\n') != std::string::npos)
                throw invalid_document_error {std::format("{}:URI contains line breaks:{}", __func__, uri)};

            // Request Line
            std::format_to(std::back_inserter(buffer), "{} {} {}\r\n", method, uri, SIPVER_20);
        }
        else if (sipm.isMessageResponse())
        {
            auto reason = sipm.getReason();
            if (reason.find('\r') != std::string::npos || reason.find('\n') != std::string::npos)
                throw invalid_document_error {std::format("{}:Reason phrase contains line breaks:{}", __func__, reason)};
            // Status Line
            std::format_to(std::back_inserter(buffer), "{} {} {}\r\n", SIPVER_20, sipm.getStatusCode(), reason);
        }
        else
        {
            throw invalid_document_error {
                    std::format("{}:sipm /type is neither `SIPMessageType::request` nor `SIPMessageType::response`.", __func__)};
        }

        // Encode the body first so we can get the content-length properly.
        auto body = serializeSDP(sipm);
        sipm.setHeader(HFS_CONTENT_LENGTH[1], body.length());

        // Headers
        if (auto mh = sipm.headers(); mh.size() > 0)
        {
            // NOTE: Header order is not preserved during serialization.
            // The nlohmann::json library does not maintain insertion order.
            // This is acceptable for SIP as header order is not significant per RFC 3261.
            for (auto& [key, val] : sipm.headers().items())
            {
                if (key.find('\r') != std::string::npos || key.find('\n') != std::string::npos)
                    throw invalid_document_error {std::format("{}:Header key contains line breaks:{}", __func__, key)};

                if (contentType.empty() && (key.compare(HFS_CONTENT_TYPE[1]) == 0) && val.is_string()) contentType = val;

                if (val.is_null())
                {
                    // For null entries, put a blank entry. This is the same as our decode
                    std::format_to(std::back_inserter(buffer), "{}: \r\n", key);
                }
                else if (val.is_number_unsigned())
                {
                    std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<uint64_t>());
                }
                else if (val.is_number_integer() || val.is_number())
                {
                    std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<int64_t>());
                }
                else if (val.is_number_float()) { std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<float>()); }
                else if (val.is_string())
                {
                    std::string sval = val.get<std::string>();
                    if (sval.find('\r') != std::string::npos || sval.find('\n') != std::string::npos)
                        throw invalid_document_error {std::format("{}:Header value contains line breaks:{}", __func__, key)};
                    std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, sval);
                }
                else if (val.is_boolean())
                {
                    std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val ? "true" : "false");
                }
                else if (val.is_array())
                {
                    // Special handling for arrays.
                    // We serialize with the same key and the various values follow.
                    for (auto& item : val.items())
                    {
                        auto iv = item.value();
                        if (iv.is_string())
                        {
                            std::string sval = iv.get<std::string>();
                            if (sval.find('\r') != std::string::npos || sval.find('\n') != std::string::npos)
                                throw invalid_document_error {
                                        std::format("{}:Header array value contains line breaks:{}", __func__, key)};
                            std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, sval);
                        }
                        else
                        {
                            std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, iv.dump());
                        }
                    }
                }
            }

            // End of the message header section
            buffer += ELEM_NEWLINE;
        }

        // Add the body
        buffer += body;

        // At the end we must have a complete and true serialized (ready for the wire) sip message
        return buffer;
    }
} // namespace siddiqsoft
