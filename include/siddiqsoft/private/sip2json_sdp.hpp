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
    inline std::string sip2json::escapeJsonPointerToken(const std::string& token)
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
    /// @brief Decode SDP Body given the buffer view
    /// @param sipm Destination sipmessage object to store parsed SDP data
    /// @param buffer Buffer view containing SDP body (advanced in-place past parsed content)
    /// @return true if at least one SDP element was parsed, false if no elements found
    /// @throws std::exception if parsing fails
    inline bool sip2json::parseBodySDP(sipmessage& sipm, std::string_view& buffer) noexcept(false)
    {
        using namespace std;

        bool            found           = false;
        int32_t         blockIndex      = -1;
        nlohmann::json* currentSdpBlock = nullptr;

        while (!buffer.empty())
        {
            auto             lfPos       = buffer.find('\n');
            std::string_view line        = (lfPos != std::string_view::npos) ? buffer.substr(0, lfPos) : buffer;
            std::string_view lineContent = line;
            if (!lineContent.empty() && lineContent.back() == '\r') lineContent.remove_suffix(1);

            if (lineContent.size() >= 2 && lineContent[1] == '=')
            {
                char             keyChar = lineContent[0];
                std::string      key(1, keyChar);
                std::string_view valueView = lineContent.substr(2);
                std::string      value(valueView);

                found = true;
                if (keyChar == 'v')
                {
                    blockIndex++;
                    auto& sdpArray            = sipm["b"s]["sdp"s];
                    sdpArray[blockIndex][key] = 0;
                    currentSdpBlock           = &sdpArray[blockIndex];
                }
                else
                {
                    if (blockIndex < 0 || currentSdpBlock == nullptr)
                        throw invalid_document_error {std::format("{}:SDP block must start with v=0", __func__)};

                    auto& sdpBlock = *currentSdpBlock;

                    if (keyChar == 'a')
                    {
                        auto colonPos = valueView.find(':');
                        if (colonPos != std::string_view::npos)
                        {
                            auto akey = std::string(valueView.substr(0, colonPos));
                            auto aval = std::string(valueView.substr(colonPos + 1));

                            auto& aObj = sdpBlock["a"s];
                            if (aObj.contains(akey) && !aObj[akey].is_array())
                            {
                                auto previousValue = aObj[akey];
                                aObj[akey]         = {previousValue, aval};
                            }
                            else if (aObj[akey].is_array())
                                aObj[akey].push_back(aval);
                            else if (!aval.empty())
                                aObj[akey] = aval;
                            else
                                aObj[akey] = nullptr;
                        }
                        else if (!value.empty()) { sdpBlock["a"s][value] = true; }
                    }
                    else if (keyChar == 'c')
                    {
                        // We expect the c= line to have 3 space-separated values: nettype, addrtype, and address.
                        auto s1 = valueView.find(' ');
                        auto s2 = (s1 != std::string_view::npos) ? valueView.find(' ', s1 + 1) : std::string_view::npos;
                        if (s1 != std::string_view::npos && s2 != std::string_view::npos)
                        {
                            auto nettype  = valueView.substr(0, s1);
                            auto addrtype = valueView.substr(s1 + 1, s2 - (s1 + 1));
                            auto addr     = valueView.substr(s2 + 1);
                            sdpBlock[key] = nlohmann::json {
                                    {"type"s, string(nettype)}, {"subtype"s, string(addrtype)}, {"dn"s, string(addr)}};
                        }
                        else if (!value.empty()) { sdpBlock[key] = value; }
                    }
                    else if (keyChar == 'o')
                    {
                        // The o= line is expected to have 6 space-separated values: username, session id, session version, nettype, addrtype, and address.
                        std::string_view rem = valueView;
                        std::string_view parts[6];
                        size_t           count = 0;
                        while (!rem.empty() && count < 6)
                        {
                            auto sp = (count < 5) ? rem.find(' ') : std::string_view::npos;
                            if (sp != std::string_view::npos)
                            {
                                parts[count++] = rem.substr(0, sp);
                                rem            = rem.substr(sp + 1);
                            }
                            else
                            {
                                parts[count++] = rem;
                                break;
                            }
                        }
                        if (count == 6)
                        {
                            sdpBlock[key] = nlohmann::json {{"user"s, string(parts[0])},
                                                            {"t1"s, string(parts[1])},
                                                            {"t2"s, string(parts[2])},
                                                            {"type"s, string(parts[3])},
                                                            {"subtype"s, string(parts[4])},
                                                            {"host"s, string(parts[5])}};
                        }
                        else if (!value.empty()) { sdpBlock[key] = value; }
                    }
                    else if (keyChar == 'i')
                    {
                        // The i= line is expected to have the format: "name" (dn) type
                        auto p1 = valueView.find(" (");
                        auto p2 = (p1 != std::string_view::npos) ? valueView.find(") ", p1 + 2) : std::string_view::npos;
                        if (p1 != std::string_view::npos && p2 != std::string_view::npos)
                        {
                            auto iName = string(valueView.substr(0, p1));
                            if (iName.starts_with("\""s) && iName.ends_with("\""s) && iName.length() >= 2)
                                iName = iName.substr(1, iName.length() - 2);

                            sdpBlock[key] = nlohmann::json {{"name"s, iName},
                                                            {"dn"s, string(valueView.substr(p1 + 2, p2 - (p1 + 2)))},
                                                            {"type"s, string(valueView.substr(p2 + 2))}};
                        }
                        else if (!value.empty()) { sdpBlock[key] = value; }
                        else
                        {
                            sdpBlock[key] = "";
                        }
                    }
                    else if (keyChar == 't')
                    {
                        uint32_t ts = 0, te = 0;
                        int      parsed = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(WINDOWS) || defined(WIN32)
                        parsed = ::sscanf_s(value.c_str(), "%u %u", &ts, &te);
#else
                        parsed = std::sscanf(value.c_str(), "%u %u", &ts, &te);
#endif
                        if (parsed == 2)
                        {
                            sdpBlock[key].push_back(ts);
                            sdpBlock[key].push_back(te);
                        }
                        else if (parsed > 0)
                        {
                            throw invalid_document_error {
                                    std::format("{}:Timing element must have exactly 2 values, got {}", __func__, parsed)};
                        }
                    }
                    else if (!key.empty() && value.empty()) { sdpBlock[key] = ""; }
                    else if (!key.empty()) { sdpBlock[key] = value; }
                }

                if (lfPos != std::string_view::npos)
                    buffer.remove_prefix(lfPos + 1);
                else
                    buffer = {};
            }
            else
            {
                // Skip noise until next SDP element (a valid SDP key followed by '=')
                static constexpr std::string_view validSdpKeys = "vosiuepcbtzkma";
                size_t                            nextPos      = std::string_view::npos;
                for (size_t i = 0; i + 1 < buffer.size(); ++i)
                {
                    if (buffer[i + 1] == '=' && validSdpKeys.find(buffer[i]) != std::string_view::npos)
                    {
                        if (i == 0 || buffer[i - 1] == '\n')
                        {
                            nextPos = i;
                            break;
                        }
                    }
                }
                if (nextPos == std::string_view::npos) break;
                buffer.remove_prefix(nextPos);
            }
        }

        return found;
    }

    inline bool sip2json::parseBodySDP(sipmessage&                  sipm,
                                       std::string::iterator&       bufferStart,
                                       const std::string::iterator& bufferEnd) noexcept(false)
    {
        if (bufferStart == bufferEnd) return false;
        const char*      pStart = std::to_address(bufferStart);
        const char*      pEnd   = std::to_address(bufferEnd);
        std::string_view sv(pStart, static_cast<size_t>(pEnd - pStart));
        bool             res      = parseBodySDP(sipm, sv);
        size_t           consumed = (pEnd - pStart) - sv.size();
        bufferStart += consumed;
        return res;
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
                    return std::format("\"{}\" ({}) {}", item.value("name"s, ""), item.value("dn"s, ""), item.value("type"s, ""));
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
        // We need to check if the following are empty
        // i=, u=, e=, p=, c=
        // ..skip them if they are empty.
        // The only required elements are v=, o=, s=, t=, m=
        // NOTE: we extract the contentType value during the header serialization.
        if (contentType == CONTENT_TYPE_APP_SDP)
        {
            if (sipm.contains(JSON_KEY_BODY) && !sipm.body().is_null())
            {
                if (sipm.contains("/b/sdp"_json_pointer))
                {
                    // the sdp is stored as an array of objects
                    auto sdp = sipm.at("/b/sdp"_json_pointer);
                    for (auto& block : sdp)
                    {
                        // Build each block; order is critical.
                        // We do not support session-level attributes (only media-level attributes)
                        std::format_to(std::back_inserter(buffer),
                                       "v=0\r\no={}\r\ns={}\r\n",
                                       serializeSDPelement(block, "o"),
                                       serializeSDPelement(block, "s"));
                        // Note the optional elements are skipped if they are not present.
                        // The serializeSDPelement will return an empty string if the element is not present.
                        // The standard has these optional elements in a "sequence".
                        if (auto elem = serializeSDPelement(block, "i"s); !elem.empty())
                            std::format_to(std::back_inserter(buffer), "i={}\r\n", elem);
                        // Optional.. "u"
                        if (auto elem = serializeSDPelement(block, "u"s); !elem.empty())
                            std::format_to(std::back_inserter(buffer), "u={}\r\n", elem);
                        // Optional.. "e"
                        if (auto elem = serializeSDPelement(block, "e"s); !elem.empty())
                            std::format_to(std::back_inserter(buffer), "e={}\r\n", elem);
                        // Optional.. "p"
                        if (auto elem = serializeSDPelement(block, "p"s); !elem.empty())
                            std::format_to(std::back_inserter(buffer), "p={}\r\n", elem);
                        // Optional.. "c"
                        if (auto elem = serializeSDPelement(block, "c"s); !elem.empty())
                            std::format_to(std::back_inserter(buffer), "c={}\r\n", elem);
                        // Mandatory (typical); No support for session a-lines.
                        std::format_to(std::back_inserter(buffer),
                                       "t={}\r\nm={}\r\n",
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
        else if ((contentType.compare(CONTENT_TYPE_TEXT_PLAIN) == 0) && (sipm.contains(JSON_KEY_BODY) && sipm.body().is_string()))
        {
            buffer += sipm.body();
        }

        return buffer;
    }
} // namespace siddiqsoft
