/*
    A SIP Parser for Modern C++: SDP Parsing & Serialization Helpers
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
#include <algorithm>
#include <format>

#include "nlohmann/json.hpp"
#include "sip2json_exception.hpp"
#include "sip2json_utils.hpp"
#include "../sipmessage.hpp"

namespace siddiqsoft
{
    /// @brief Escapes key tokens for use in nlohmann::json::json_pointer per RFC 6901
    inline std::string sip2json::escapeJsonPointerToken(std::string_view token)
    {
        std::string escaped;
        escaped.reserve(token.size());
        for (char c : token)
        {
            if (c == '~')
                escaped += "~0";
            else if (c == '/')
                escaped += "~1";
            else
                escaped += c;
        }
        return escaped;
    }

    /// @brief Decode SDP (Session Description Protocol) message blocks
    /// @details This method parses SDP blocks from the buffer according to RFC 4566.
    /// It handles multiple SDP blocks (separated by v=0 lines) and supports:
    /// - Session-level attributes: v (version), o (origin), s (session name), i (session info),
    ///   u (URI), e (email), p (phone), c (connection), t (timing)
    /// - Media-level attributes: m (media), a (attributes)
    /// - Special parsing for connection lines (c=), origin lines (o=), session info (i=)
    /// - Attribute lines with both key:value and flag formats
    /// - Multiple attributes with the same key (stored as arrays)
    ///
    /// The method increments blockIndex for each new SDP session (v=0 line encountered).
    /// Attributes are stored in the JSON structure at /b/sdp/{blockIndex}/{key}/{subkey}
    ///
    /// @param sipm Destination sipmessage object to store parsed SDP data
    /// @param bufferStart Start of the buffer (modified to point past parsed content)
    /// @param bufferEnd End of the content area (not the end of the stream)
    /// @return true if at least one SDP element was parsed, false if no elements found
    /// @throws std::exception if parsing fails
    ///
    /// @note bufferStart must point to the location past the very first v=0 as this signals
    ///       the start of the body. The method starts with blockIndex = -1 and increments
    ///       it to 0 on the first v=0 match.
    inline bool sip2json::parseBodySDP(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
    {
        using namespace std;

        bool found = false;

        // NOTE: bufferStart points to the location past the very first v=0 as this is the signal of the start
        // of the body. Therefore, we start with blockIndex = 0 and then increment everytime we encounter next v=0
        int32_t blockIndex = -1;

        while (bufferStart < bufferEnd)
        {
            auto matcher = ctre::search<SIP_PATTERN_BODY_RE>(bufferStart, bufferEnd);
            if (!matcher) break;

            auto key   = string(matcher.get<1>().to_view());
            auto value = string(matcher.get<2>().to_view());

            found = true;
            if (key == "v"s)
            {
                // First element; increment blockIndex.
                // Add the next element to a new SDP object.
                blockIndex++; // the first match will increment this to "0"
                sipm["b"s]["sdp"s][blockIndex][key] = 0;
            }
            else
            {
                if (blockIndex < 0)
                    throw invalid_document_error {std::format("{}:SDP block must start with v=0", __func__)};

                if (key == "a"s)
                {
                    // attribute lines: https://en.wikipedia.org/wiki/Session_Description_Protocol#Attributes
                    auto alineMatcher = ctre::search<SIP_PATTERN_BODY_ALINE_RE>(value);

                    if (alineMatcher)
                    {
                        auto akey = string(alineMatcher.get<1>().to_view());
                        auto aval = string(alineMatcher.get<2>().to_view());

                        // This is the form where a=attribute:value
                        nlohmann::json::json_pointer pkey(std::format("/b/sdp/{}/{}/{}", blockIndex, key, escapeJsonPointerToken(akey)));

                        // We may get multiple items for the same "key" such as `a=rtpmap:x` and `a=rtpmap:y`
                        // In this case we should start an array
                        if (sipm.contains(pkey) && !sipm[pkey].is_array())
                        {
                            auto previousValue = sipm[pkey]; // make a copy!
                            sipm[pkey]         = {previousValue, aval};
                        }
                        else if (sipm[pkey].is_array())
                            sipm[pkey].push_back(aval);
                        else if (!aval.empty())
                            sipm[pkey] = aval;
                        else
                            sipm[pkey] = nullptr;
                    }
                    else if (!value.empty())
                    {
                        // This is the form where a=flag
                        // We matched a=key without the `:` or the "value" so we should store the value with nullptr
                        nlohmann::json::json_pointer pkey(std::format("/b/sdp/{}/{}/{}", blockIndex, key, escapeJsonPointerToken(value)));
                        sipm[pkey] = true;
                    }
                }
                else
                {
                    nlohmann::json::json_pointer pkey(std::format("/b/sdp/{}/{}", blockIndex, escapeJsonPointerToken(key)));

                    if (key == "c"s)
                    {
                        auto clineMatcher = ctre::search<SIP_PATTERN_BODY_CLINE_RE>(value);
                        if (clineMatcher)
                        {
                            sipm[pkey] = nlohmann::json {{"type"s, string(clineMatcher.get<1>().to_view())},
                                                         {"subtype"s, string(clineMatcher.get<2>().to_view())},
                                                         {"dn"s, string(clineMatcher.get<3>().to_view())}};
                        }
                        else if (!value.empty()) { sipm[pkey] = value; }
                    }
                    else if (key == "o"s)
                    {
                        auto olineMatcher = ctre::search<SIP_PATTERN_BODY_OLINE_RE>(value);
                        if (olineMatcher)
                        {
                            sipm[pkey] = nlohmann::json {{"user"s, string(olineMatcher.get<1>().to_view())},
                                                         {"t1"s, string(olineMatcher.get<2>().to_view())},
                                                         {"t2"s, string(olineMatcher.get<3>().to_view())},
                                                         {"type"s, string(olineMatcher.get<4>().to_view())},
                                                         {"subtype"s, string(olineMatcher.get<5>().to_view())},
                                                         {"host"s, string(olineMatcher.get<6>().to_view())}};
                        }
                        else if (!value.empty()) { sipm[pkey] = value; }
                    }
                    else if (key.compare("i") == 0)
                    {
                        // Identity and number and type of call.
                        auto ilineMatcher = ctre::search<SIP_PATTERN_BODY_ILINE_RE>(value);
                        if (ilineMatcher)
                        {
                            auto iName = string(ilineMatcher.get<1>().to_view());
                            // Set the name but check to ensure that if we have a " in the name that we strip it..
                            sipm[pkey] = nlohmann::json {
                                    {"name"s, iName.starts_with("\""s) ? iName.substr(1, iName.length() - 2) : iName},
                                    {"dn"s, string(ilineMatcher.get<2>().to_view())},
                                    {"type"s, string(ilineMatcher.get<3>().to_view())}};
                        }
                        else if (!value.empty()) { sipm[pkey] = value; }
                        else
                        {
                            sipm[pkey] = "";
                        }
                    }
                    else if (key.compare("t"s) == 0)
                    {
                        // timing - FIX: Validate exactly 2 values are parsed
                        uint32_t ts = 0, te = 0;
                        int      parsed = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(WINDOWS) || defined(WIN32)
                        parsed = ::sscanf_s(value.c_str(), "%u %u", &ts, &te);
#else
                        parsed = std::sscanf(value.c_str(), "%u %u", &ts, &te);
#endif
                        if (parsed == 2)
                        {
                            sipm[pkey].push_back(ts);
                            sipm[pkey].push_back(te);
                        }
                        else if (parsed > 0)
                        {
                            throw invalid_document_error {
                                    std::format("{}:Timing element must have exactly 2 values, got {}", __func__, parsed)};
                        }
                    }
                    else if (!key.empty() && value.empty()) { sipm[pkey] = ""; }
                    else if (!key.empty()) { sipm[pkey] = value; }
                }
            }

            // Offset the start to the point after the match.
            bufferStart = matcher.get<0>().end();
            // Skip over trailing line endings
            while (bufferStart < bufferEnd && (*bufferStart == '\r' || *bufferStart == '\n'))
                ++bufferStart;
        }

        return found;
    }

    /// @brief Helper to serialize the SDP element with custom decode
    /// @param sdpBlock The SDP block from the SDP array
    /// @param element The element: o, s, i, c, t, m, a. When returning a= the code builds CRLF terminators.
    /// @return Returns the sdp element as string.
    inline std::string sip2json::serializeSDPelement(nlohmann::json& sdpBlock, const std::string& element)
    {
        using namespace std;

        if (!sdpBlock.contains("v"s) && !sdpBlock.contains("o"s) && !sdpBlock.contains("s"s) && !sdpBlock.contains("t"s) &&
            !sdpBlock.contains("m"s))
            throw missing_required_element {std::format("{}:Required Element {} not present.", __func__, element)};

        // If we donot have it then just return..
        if (sdpBlock.contains(element))
        {
            // Continue to build
            if (auto item = sdpBlock.at(element); item.is_object())
            {
                if (element == "a"s)
                {
                    std::string ret {};

                    for (auto& [kv, v] : item.items())
                    {
                        if (v.is_array())
                        {
                            for (auto& i : v.items())
                            {
                                auto& vi = i.value();
                                if (vi.is_string())
                                    std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, vi.get<std::string>());
                                else if (vi.is_number() || vi.is_number_integer())
                                    std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, vi.get<int64_t>());
                                else if (vi.is_number_unsigned())
                                    std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, vi.get<uint64_t>());
                                else if (vi.is_number_float())
                                    std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, vi.get<double>());
                                else if (vi.is_boolean() && vi == true)
                                    std::format_to(std::back_inserter(ret), "a={}\r\n", kv);
                                else
                                    std::format_to(std::back_inserter(ret), "a={}\r\n", kv);
                            }
                        }
                        else if (v.is_string())
                            std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, v.get<std::string>());
                        else if (v.is_number() || v.is_number_integer())
                            std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, v.get<int64_t>());
                        else if (v.is_number_unsigned())
                            std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, v.get<uint64_t>());
                        else if (v.is_number_float())
                            std::format_to(std::back_inserter(ret), "a={}:{}\r\n", kv, v.get<double>());
                        else if (v.is_boolean() && v == true)
                            std::format_to(std::back_inserter(ret), "a={}\r\n", kv);
                        else
                            std::format_to(std::back_inserter(ret), "a={}\r\n", kv);
                    }

                    return ret;
                }
                if (element == "o"s)
                {
                    return std::format("{} {} {} {} {} {}",
                                       item.value("user"s, ""s),
                                       item.value("t1"s, ""s),
                                       item.value("t2"s, ""s),
                                       item.value("type"s, ""s),
                                       item.value("subtype"s, ""s),
                                       item.value("host"s, ""s));
                }
                if (element == "i"s)
                {
                    return std::format(
                            "\"{}\" ({}) {}", item.value("name"s, ""), item.value("dn"s, ""), item.value("type"s, ""));
                }
                if (element == "c"s)
                {
                    return std::format("{} {} {}", item.value("type"s, ""), item.value("subtype"s, ""), item.value("dn"s, ""));
                }
            }
            else if (item.is_array())
            {
                if (element == "t"s)
                {
                    // FIX: Add bounds check before accessing array elements
                    if (item.size() < 2)
                        throw missing_required_element {
                                std::format("{}:Timing element must have 2 values, got {}", __func__, item.size())};
                    return std::format("{} {}", item[0].get<uint32_t>(), item[1].get<uint32_t>());
                }
            }
            else if (item.is_string())
            {
                // In case the parse wasn't able to split properly, it will store it as a string value.
                // Serialize the as-is case.
                return item.get<std::string>();
            }
        }

        return std::string {};
    }

    /// @brief Serializes the SDP content
    /// @param sipm sipmessage object
    /// @return string representing the sdp
    inline std::string sip2json::serializeSDP(sipmessage& sipm) noexcept(false)
    {
        using namespace std;

        std::string buffer {};
        auto        contentType = sipm.getContentType();

        // If content-type is not set, then just return regardless of the body element contents.
        if (contentType.empty()) return buffer;

        // Check for a valid/supported contenttype
        if (!(contentType == CONTENT_TYPE_APP_SDP || contentType == CONTENT_TYPE_TEXT_PLAIN))
            throw invalid_document_error {std::format("{}:Unsupported content-type:{}", __func__, contentType)};

        // Body
        // NOTE: we extract the contentType value during the header serialization.
        if (contentType == CONTENT_TYPE_APP_SDP)
        {
            if (sipm.contains("b"s) && !sipm.body().is_null())
            {
                if (sipm.contains("/b/sdp"_json_pointer))
                {
                    // the sdp is stored as an array of objects
                    auto sdp = sipm.at("/b/sdp"_json_pointer);
                    for (auto& block : sdp)
                    {
                        // Build each block; order is critical. We do not support session-level attributes (only media-level attributes)
                        std::format_to(std::back_inserter(buffer),
                                       "v=0\r\no={}\r\ns={}\r\ni={}\r\n",
                                       serializeSDPelement(block, "o"),
                                       serializeSDPelement(block, "s"),
                                       serializeSDPelement(block, "i"));
                        // Optional..
                        if (block.contains("u"))
                            std::format_to(std::back_inserter(buffer), "u={}\r\n", serializeSDPelement(block, "u"s));
                        // Optional..
                        if (block.contains("e"))
                            std::format_to(std::back_inserter(buffer), "e={}\r\n", serializeSDPelement(block, "e"s));
                        // Optional..
                        if (block.contains("p"))
                            std::format_to(std::back_inserter(buffer), "p={}\r\n", serializeSDPelement(block, "p"s));
                        // Mandatory (typical); No support for session a-lines.
                        std::format_to(std::back_inserter(buffer),
                                       "c={}\r\nt={}\r\nm={}\r\n",
                                       serializeSDPelement(block, "c"),
                                       serializeSDPelement(block, "t"),
                                       serializeSDPelement(block, "m"));
                        // Media a-lines
                        buffer += serializeSDPelement(block, "a"s);
                    }
                }
                else
                {
                    throw invalid_document_error {std::format("{}:sipm `b`ody does not have sdp element.", __func__)};
                }
            }
            else
            {
                // This should not be an error; there are live SIP messages where the client sets the Content-Type
                // but also sets the Content-Length to `0` so we should avoid encoding anything.
            }
        }
        else if ((contentType.compare(CONTENT_TYPE_TEXT_PLAIN) == 0) && (sipm.contains("b"s) && sipm.body().is_string()))
        {
            buffer += sipm.body();
        }

        return buffer;
    }
} // namespace siddiqsoft
