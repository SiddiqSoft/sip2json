/*
    A SIP Parser for Modern C++: SIP Message Parser Implementation
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
#include <algorithm>
#include <functional>
#include <optional>
#include <string_view>
#include <vector>
#include <format>
#include <charconv>

#include "nlohmann/json.hpp"
#include "sip2json_exception.hpp"
#include "sip2json_utils.hpp"
#include "../sipmessage.hpp"

namespace siddiqsoft
{
    /// @brief Parse the start line from buffer view
    /// @param sipm Destination sipmessage
    /// @param buffer Buffer view (advanced past start line on return)
    /// @return true if valid start line decoded
    inline bool sip2json::parseStartLine(sipmessage& sipm, std::string_view& buffer) noexcept(false)
    {
        if (buffer.empty())
            throw invalid_startline_error {std::format("{}:SIP Startline not found.", __func__)};

        // Fast-path: find line end
        auto lfPos = buffer.find('\n');
        if (lfPos != std::string_view::npos)
        {
            std::string_view line = buffer.substr(0, lfPos);
            if (!line.empty() && line.back() == '\r') line.remove_suffix(1);

            // Fast check: Response message (starts with "SIP/2.0 ")
            if (line.starts_with("SIP/2.0 ") || line.starts_with("SIP/2.0\t"))
            {
                auto rem = line.substr(7);
                while (!rem.empty() && (rem.front() == ' ' || rem.front() == '\t')) rem.remove_prefix(1);

                uint32_t statusCode = 0;
                auto [ptr, ec] = std::from_chars(rem.data(), rem.data() + rem.size(), statusCode);
                if (ec == std::errc() && ptr != rem.data())
                {
                    std::string_view reason(ptr, static_cast<size_t>((rem.data() + rem.size()) - ptr));
                    while (!reason.empty() && (reason.front() == ' ' || reason.front() == '\t')) reason.remove_prefix(1);

                    auto& sl = sipm[JSON_KEY_STARTLINE];
                    sl[JSON_KEY_TYPE]    = SIPMessageType::response;
                    sl[JSON_KEY_REASON]  = std::string(reason);
                    sl[JSON_KEY_STATUS]  = statusCode;
                    sl[JSON_KEY_VERSION] = SIPVER_20;

                    buffer.remove_prefix(lfPos + 1);
                    while (!buffer.empty() && (buffer.front() == '\r' || buffer.front() == '\n')) buffer.remove_prefix(1);
                    return true;
                }
            }
            // Fast check: Request message (ends with "SIP/2.0")
            else if (line.ends_with("SIP/2.0"))
            {
                auto sp1 = line.find_first_of(" \t");
                if (sp1 != std::string_view::npos)
                {
                    std::string_view method = line.substr(0, sp1);
                    static constexpr std::string_view validMethods[] = {
                            "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER",
                            "SUBSCRIBE", "NOTIFY", "REFER", "PUBLISH", "UPDATE", "PRACK",
                            "INFO", "MESSAGE"};

                    bool isMethodValid = false;
                    for (const auto& vm : validMethods)
                    {
                        if (method == vm) { isMethodValid = true; break; }
                    }

                    if (isMethodValid)
                    {
                        auto sp2 = line.rfind(" SIP/2.0");
                        if (sp2 == std::string_view::npos) sp2 = line.rfind("\tSIP/2.0");

                        if (sp2 != std::string_view::npos && sp1 < sp2)
                        {
                            std::string_view uri = line.substr(sp1 + 1, sp2 - (sp1 + 1));
                            while (!uri.empty() && (uri.front() == ' ' || uri.front() == '\t')) uri.remove_prefix(1);
                            while (!uri.empty() && (uri.back() == ' ' || uri.back() == '\t')) uri.remove_suffix(1);

                            auto& sl = sipm[JSON_KEY_STARTLINE];
                            sl[JSON_KEY_TYPE]    = SIPMessageType::request;
                            sl[JSON_KEY_METHOD]  = std::string(method);
                            sl[JSON_KEY_URI]     = std::string(uri);
                            sl[JSON_KEY_VERSION] = SIPVER_20;

                            buffer.remove_prefix(lfPos + 1);
                            while (!buffer.empty() && (buffer.front() == '\r' || buffer.front() == '\n')) buffer.remove_prefix(1);
                            return true;
                        }
                    }
                }
            }
        }

        // Fallback to CTRE regex for complex start-lines / torture tests
        auto matchStartLine = ctre::search<SIP_PATTERN_STARTLINE>(buffer.data(), buffer.data() + buffer.size());
        if (matchStartLine)
        {
            auto g1 = matchStartLine.get<1>().to_view();
            auto g2 = matchStartLine.get<2>().to_view();
            auto g3 = matchStartLine.get<3>().to_view();

            if (SIPVER_20 == g3)
            {
                sipm[JSON_KEY_STARTLINE] = {{JSON_KEY_TYPE, SIPMessageType::request},
                                            {JSON_KEY_METHOD, std::string(g1)},
                                            {JSON_KEY_URI, std::string(g2)},
                                            {JSON_KEY_VERSION, std::string(g3)}};
            }
            else if (SIPVER_20 == g1)
            {
                uint32_t statusCode = 0;
                auto [ptr, ec] = std::from_chars(g2.data(), g2.data() + g2.size(), statusCode);
                if (ec != std::errc()) { statusCode = static_cast<uint32_t>(std::stoi(std::string(g2))); }

                sipm[JSON_KEY_STARTLINE] = {{JSON_KEY_TYPE, SIPMessageType::response},
                                            {JSON_KEY_REASON, std::string(g3)},
                                            {JSON_KEY_STATUS, statusCode},
                                            {JSON_KEY_VERSION, std::string(g1)}};
            }
            else
            {
                throw invalid_startline_error {std::format("{}:Unsupported SIP version in startline: '{}'", __func__, std::string(g3))};
            }

            size_t matchEndOffset = matchStartLine.get<0>().end() - buffer.data();
            buffer.remove_prefix(matchEndOffset);
            while (!buffer.empty() && (buffer.front() == '\r' || buffer.front() == '\n'))
                buffer.remove_prefix(1);

            return true;
        }

        throw invalid_startline_error {std::format("{}:SIP Startline not found.", __func__)};
    }

    inline bool sip2json::parseStartLine(sipmessage&                  sipm,
                                         std::string::iterator&       bufferStart,
                                         const std::string::iterator& bufferEnd) noexcept(false)
    {
        if (bufferStart == bufferEnd) throw invalid_startline_error {std::format("{}:SIP Startline not found.", __func__)};
        const char* pStart = std::to_address(bufferStart);
        const char* pEnd   = std::to_address(bufferEnd);
        std::string_view sv(pStart, static_cast<size_t>(pEnd - pStart));
        bool res = parseStartLine(sipm, sv);
        size_t consumed = (pEnd - pStart) - sv.size();
        bufferStart += consumed;
        return res;
    }


    /// @brief Appends or initializes a header entry as a multi-line array in the headers JSON block.
    /// @param headersJson The headers JSON object (`sipm["h"]`).
    /// @param targetKey The target header key string.
    /// @param value The header value to store or append.
    inline void storeMultiLineHeader(nlohmann::json& headersJson, const std::string& targetKey, std::string value)
    {
        auto it = headersJson.find(targetKey);
        if (it != headersJson.end())
        {
            if (it->is_array())
                it->push_back(std::move(value));
            else
            {
                auto existing = *it;
                *it           = nlohmann::json::array({existing, std::move(value)});
            }
        }
        else
        {
            headersJson.emplace(targetKey, nlohmann::json::array({std::move(value)}));
        }
    }

    /// @brief Validates and parses the Content-Length header value.
    /// @param value The header value string to parse.
    /// @return Returns parsed uint32_t content length.
    inline uint32_t parseContentLengthValue(std::string_view value) noexcept(false)
    {
        while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.remove_prefix(1);
        while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) value.remove_suffix(1);

        uint64_t len = 0;
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), len);
        if (ec != std::errc() || ptr != (value.data() + value.size()) || len > 100 * 1024 * 1024)
            throw invalid_document_error {std::format("storeHeaderValue:Invalid Content-Length value '{}'", value)};
        return static_cast<uint32_t>(len);
    }

    inline uint32_t parseContentLengthValue(const std::string& value) noexcept(false)
    {
        return parseContentLengthValue(std::string_view(value));
    }

    /// @brief Validates and parses the Expires header value.
    /// @param value The header value string to parse.
    /// @return Returns parsed uint32_t expires value.
    inline uint32_t parseExpiresValue(std::string_view value) noexcept(false)
    {
        while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.remove_prefix(1);
        while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) value.remove_suffix(1);

        uint64_t val = 0;
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), val);
        if (ec != std::errc() || ptr != (value.data() + value.size()) || val > std::numeric_limits<uint32_t>::max())
            throw invalid_document_error {std::format("storeHeaderValue:Invalid Expires value '{}'", value)};
        return static_cast<uint32_t>(val);
    }

    inline uint32_t parseExpiresValue(const std::string& value) noexcept(false)
    {
        return parseExpiresValue(std::string_view(value));
    }

    /// @brief Store the value in the header section. Performs from basic transforms/detection of bool, integer
    /// @param sipm The target sipmessage object
    /// @param key The key
    /// @param value The value
    /// @return Returns true if the store was successful.
    inline bool sip2json::storeHeaderValue(sipmessage& sipm, std::string_view key, std::string_view value) noexcept(false)
    {
        auto& headersJson = sipm[JSON_KEY_HEADERS];
        const HeaderKeySet& keySet = canonicalizeHeaderKey(key);
        const std::string&  keyStr = keySet.canonical();

        if (keySet.isMultiLine)
        {
            storeMultiLineHeader(headersJson, keyStr, std::string(value));
        }
        else if (&keySet == &HFS_CONTENT_LENGTH)
        {
            headersJson[keyStr] = parseContentLengthValue(value);
        }
        else if (&keySet == &HFS_EXPIRES)
        {
            headersJson[keyStr] = parseExpiresValue(value);
        }
        else
        {
            auto it = headersJson.find(keyStr);
            if (it != headersJson.end())
            {
                if (it->is_array())
                    it->push_back(std::string(value));
                else
                {
                    auto existing = *it;
                    *it           = nlohmann::json::array({existing, std::string(value)});
                }
            }
            else
            {
                headersJson.emplace(keyStr, std::string(value));
            }
        }

        return true;
    }

    inline bool sip2json::storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value) noexcept(false)
    {
        return storeHeaderValue(sipm, std::string_view(key), std::string_view(value));
    }

    /// @brief Decode headers within the stream
    /// @param sipm Destination sipmessage
    /// @param buffer Buffer view (advanced past header section on return)
    /// @return true/false depending on the state of the decode of headers.
    inline bool sip2json::parseHeaders(sipmessage& sipm, std::string_view& buffer) noexcept(false)
    {
        auto delimPos = buffer.find("\r\n\r\n");
        size_t headerDelimiterSize = 4;
        if (delimPos == std::string_view::npos)
        {
            delimPos = buffer.find("\n\n");
            headerDelimiterSize = 2;
        }

        if (delimPos == std::string_view::npos)
            throw incomplete_buffer_for_header_error {std::format("{}:Cannot find header section delimiter.", __func__).c_str()};

        std::string_view headerSection = buffer.substr(0, delimPos);
        buffer.remove_prefix(delimPos + headerDelimiterSize);

        bool found = false;
        while (!headerSection.empty())
        {
            auto colonPos = headerSection.find(':');
            if (colonPos == std::string_view::npos) break;

            std::string_view keyView = headerSection.substr(0, colonPos);
            if (keyView.empty()) break;

            headerSection.remove_prefix(colonPos + 1);
            while (!headerSection.empty() && (headerSection.front() == ' ' || headerSection.front() == '\t'))
                headerSection.remove_prefix(1);

            std::string foldedValue;
            bool headerDone = false;
            while (!headerDone)
            {
                auto lfPos = headerSection.find('\n');
                if (lfPos != std::string_view::npos)
                {
                    std::string_view lineVal = headerSection.substr(0, lfPos);
                    if (!lineVal.empty() && lineVal.back() == '\r') lineVal.remove_suffix(1);

                    headerSection.remove_prefix(lfPos + 1);

                    if (!headerSection.empty() && (headerSection.front() == ' ' || headerSection.front() == '\t'))
                    {
                        foldedValue.append(lineVal);
                        while (!headerSection.empty() && (headerSection.front() == ' ' || headerSection.front() == '\t'))
                            headerSection.remove_prefix(1);
                    }
                    else
                    {
                        if (!foldedValue.empty())
                        {
                            foldedValue.append(lineVal);
                            found = storeHeaderValue(sipm, keyView, foldedValue);
                        }
                        else
                        {
                            found = storeHeaderValue(sipm, keyView, lineVal);
                        }
                        headerDone = true;
                    }
                }
                else
                {
                    std::string_view lineVal = headerSection;
                    if (!lineVal.empty() && lineVal.back() == '\r') lineVal.remove_suffix(1);
                    headerSection = {};

                    if (!foldedValue.empty())
                    {
                        foldedValue.append(lineVal);
                        found = storeHeaderValue(sipm, keyView, foldedValue);
                    }
                    else
                    {
                        found = storeHeaderValue(sipm, keyView, lineVal);
                    }
                    headerDone = true;
                }
            }
        }

        return found;
    }

    inline bool sip2json::parseHeaders(sipmessage&                  sipm,
                                       std::string::iterator&       bufferStart,
                                       const std::string::iterator& bufferEnd) noexcept(false)
    {
        if (bufferStart == bufferEnd) return false;
        const char* pStart = std::to_address(bufferStart);
        const char* pEnd   = std::to_address(bufferEnd);
        std::string_view sv(pStart, static_cast<size_t>(pEnd - pStart));
        bool res = parseHeaders(sipm, sv);
        size_t consumed = (pEnd - pStart) - sv.size();
        bufferStart += consumed;
        return res;
    }

    /// @brief Given a non-owning buffer view, parse each message and invoke the callback with the decoded sipmessage.
    /// @param frameBuffer Buffer containing SIP messages (advanced past parsed messages).
    /// @param parseCallback Callback which takes a reference to the sipmessage just decoded.
    /// @param errorCallback Optional callback to handle the error on the parse.
    /// @return Returns the number of bytes consumed from the buffer.
    inline size_t sip2json::parseAsync(
            std::string_view&                 frameBuffer,
            std::function<void(sipmessage&&)> parseCallback,
            std::optional<std::function<void(const sip2json_exception&, std::string_view)>> errorCallback) noexcept
    {
        size_t initialSize = frameBuffer.size();
        size_t decodedMessageCount {0};

        while (!frameBuffer.empty())
        {
            try
            {
                if (auto&& sipm {parseFromBuffer(frameBuffer)}; !sipm.empty())
                {
                    decodedMessageCount++;
                    sipm["meta"]["parseCountThisBuffer"] = decodedMessageCount;
                    parseCallback(std::move(sipm));
                }
                else
                {
                    break;
                }
            }
            catch (const sip2json_exception& e)
            {
                if (errorCallback.has_value()) errorCallback.value()(e, frameBuffer);
                break;
            }
            catch (const std::exception& e)
            {
                sip2json_exception ex(e);
                if (errorCallback.has_value()) errorCallback.value()(ex, frameBuffer);
                break;
            }
            catch (...)
            {
                sip2json_exception ex("Unknown generic error");
                if (errorCallback.has_value()) errorCallback.value()(ex, frameBuffer);
                break;
            }
        }

        return initialSize - frameBuffer.size();
    }

    /// @brief Given a buffer, parse each message and invoke the callback with the decoded sipmessage object.
    /// @param frameBuffer Buffer containing SIP messages.
    /// @param parseCallback Callback which takes a reference to the sipmessage just decoded.
    /// @param errorCallback Optional callback to handle the error on the parse.
    /// @return Returns the remaining contents of the buffer.
    inline std::string& sip2json::parseAsync(
            std::string&                      frameBuffer,
            std::function<void(sipmessage&&)> parseCallback,
            std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>>
                    errorCallback) noexcept
    {
        std::string_view sv(frameBuffer);
        size_t consumed = parseAsync(
                sv,
                std::move(parseCallback),
                errorCallback.has_value()
                        ? std::optional<std::function<void(const sip2json_exception&, std::string_view)>>(
                                  [&](const sip2json_exception& ex, std::string_view rem)
                                  {
                                      auto curStart = frameBuffer.begin() + (frameBuffer.size() - rem.size());
                                      errorCallback.value()(ex, curStart, frameBuffer.end());
                                  })
                        : std::nullopt);

        frameBuffer.erase(0, consumed);
        return frameBuffer;
    }

    /// @brief Given a buffer view, parse as many frames and return the vector of messages. Advances view in-place.
    /// @param buffer Buffer view containing SIP stream.
    /// @return Vector of sipmessage decoded within the stream.
    inline std::vector<sipmessage> sip2json::parse(std::string_view& buffer) noexcept(false)
    {
        std::vector<sipmessage> msgs;
        if (!buffer.empty())
        {
            size_t estCount = std::max<size_t>(1, buffer.size() / 1024);
            msgs.reserve(std::min<size_t>(estCount, 64));
        }
        size_t decodedMessageCount {0};

        while (!buffer.empty())
        {
            try
            {
                if (auto&& sipm {parseFromBuffer(buffer)}; !sipm.empty())
                {
                    decodedMessageCount++;
                    sipm["meta"]["parseCountThisBuffer"] = decodedMessageCount;
                    msgs.emplace_back(std::move(sipm));
                }
            }
            catch (std::exception& ex)
            {
                if (msgs.empty()) throw std::invalid_argument("Nothing was parsed.");
                break;
            }
        }

        return msgs;
    }

    /// @brief Given a buffer, parse as many frames and return the vector of messages.
    inline std::vector<sipmessage> sip2json::parse(std::string::iterator&       bufferStart,
                                                   const std::string::iterator& bufferEnd) noexcept(false)
    {
        if (bufferStart == bufferEnd) return {};
        const char* pStart = std::to_address(bufferStart);
        const char* pEnd   = std::to_address(bufferEnd);
        std::string_view sv(pStart, static_cast<size_t>(pEnd - pStart));
        auto msgs = parse(sv);
        size_t consumed = (pEnd - pStart) - sv.size();
        bufferStart += consumed;
        return msgs;
    }

    /// @brief De-serialize the *first* SIP message (if present) from the buffer view and advances the view.
    /// @param buffer Buffer view containing SIP message.
    /// @return A sipmessage object containing the decoded message.
    inline sipmessage sip2json::parseFromBuffer(std::string_view& buffer) noexcept(false)
    {
        auto initialBuffer = buffer;
        sipmessage sipm;

        if (!buffer.empty())
        {
            if (buffer.size() > SIP_SAMPLE_MINIMAL_MESSAGE.length())
            {
                try
                {
                    if (auto foundRequest = parseStartLine(sipm, buffer); foundRequest)
                    {
                        if (auto foundHeaders = parseHeaders(sipm, buffer); foundHeaders)
                        {
                            if (sipm.getContentTypeView() == CONTENT_TYPE_APP_SDP)
                            {
                                if (sipm.getContentLength() > 0)
                                {
                                    if (buffer.size() >= sipm.getContentLength())
                                    {
                                        std::string_view sdpBuffer = buffer.substr(0, sipm.getContentLength());
                                        buffer.remove_prefix(sipm.getContentLength());
                                        parseBodySDP(sipm, sdpBuffer);
                                    }
                                    else
                                    {
                                        size_t avail = buffer.size();
                                        buffer = initialBuffer;
                                        throw incomplete_buffer_for_content_error {
                                                std::format("{}: Available buffer length:{} < Content-Length:{}",
                                                            __func__,
                                                            avail,
                                                            sipm.getContentLength())};
                                    }
                                }
                            }
                            else if (!sipm.getContentTypeView().empty())
                            {
                                buffer = initialBuffer;
                                throw unsupported_contenttype_error {
                                        std::format("{}:Content-Type {} not supported", __func__, sipm.getContentType())};
                            }
                        }
                    }
                }
                catch (...)
                {
                    buffer = initialBuffer;
                    throw;
                }
            }
            else
            {
                buffer = initialBuffer;
                throw incomplete_buffer_for_parse_error {std::format("{}:Incomplete Buffer for parse to continue.", __func__)};
            }
        }

        return sipm;
    }

    inline sipmessage sip2json::parseFromBuffer(std::string::iterator&       bufferStart,
                                                const std::string::iterator& bufferEnd) noexcept(false)
    {
        if (bufferStart == bufferEnd) return {};
        const char* pStart = std::to_address(bufferStart);
        const char* pEnd   = std::to_address(bufferEnd);
        std::string_view sv(pStart, static_cast<size_t>(pEnd - pStart));
        auto sipm = parseFromBuffer(sv);
        size_t consumed = (pEnd - pStart) - sv.size();
        bufferStart += consumed;
        return sipm;
    }
} // namespace siddiqsoft
