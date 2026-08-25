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

#include "nlohmann/json.hpp"
#include "sip2json_exception.hpp"
#include "sip2json_utils.hpp"
#include "../sipmessage.hpp"

namespace siddiqsoft
{
    /// @brief Parse the start line
    /// @param sipm Destination sipmessage
    /// @param bufferStart Start of the stream.
    /// @param bufferEnd End of the stream
    /// @return true/false depending on the state of the decode of start line.
    inline bool sip2json::parseStartLine(sipmessage&                  sipm,
                                         std::string::iterator&       bufferStart,
                                         const std::string::iterator& bufferEnd) noexcept(false)
    {
        using namespace std;

        auto matchStartLine = ctre::search<SIP_PATTERN_STARTLINE>(bufferStart, bufferEnd);
        bool found          = static_cast<bool>(matchStartLine);

        // Did we find a message..?
        if (found)
        {
            auto g1 = matchStartLine.get<1>().to_view();
            auto g2 = matchStartLine.get<2>().to_view();
            auto g3 = matchStartLine.get<3>().to_view();

            // The regex is very precise and there is no chance we will end up here
            // with an ill-formed (or unsupported) start-line.
            if (SIPVER_20 == g3)
            {
                sipm[JSON_KEY_STARTLINE] = {{JSON_KEY_TYPE, SIPMessageType::request},
                                            {JSON_KEY_METHOD, string(g1)},
                                            {JSON_KEY_URI, string(g2)},
                                            {JSON_KEY_VERSION, string(g3)}};
            }
            else if (SIPVER_20 == g1)
            {
                uint32_t statusCode = 0;
                auto [ptr, ec] = std::from_chars(g2.data(), g2.data() + g2.size(), statusCode);
                if (ec != std::errc()) { statusCode = static_cast<uint32_t>(std::stoi(string(g2))); }

                sipm[JSON_KEY_STARTLINE] = {{JSON_KEY_TYPE, SIPMessageType::response},
                                            {JSON_KEY_REASON, string(g3)},
                                            {JSON_KEY_STATUS, statusCode},
                                            {JSON_KEY_VERSION, string(g1)}};
            }
            else
            {
                throw invalid_startline_error {std::format("{}:Unsupported SIP version in startline: '{}'", __func__, string(g3))};
            }

            // Offset the start to the point after the match (full match end).
            // This accounts for any prefix junk before the start-line.
            bufferStart = matchStartLine.get<0>().end();
            // Skip over any trailing \r\n after the match
            while (bufferStart != bufferEnd && (*bufferStart == '\r' || *bufferStart == '\n'))
                ++bufferStart;
        }
        else
        {
            throw invalid_startline_error {std::format("{}:SIP Startline not found.", __func__)};
        }

        return found;
    }


    /// @brief Appends or initializes a header entry as a multi-line array in the headers JSON block.
    /// @param headersJson The headers JSON object (`sipm["h"]`).
    /// @param targetKey The target header key string.
    /// @param value The header value to store or append.
    inline void storeMultiLineHeader(nlohmann::json& headersJson, const std::string& targetKey, const std::string& value)
    {
        auto it = headersJson.find(targetKey);
        if (it != headersJson.end())
        {
            if (it->is_array())
                it->push_back(value);
            else
            {
                auto existing = *it;
                *it           = nlohmann::json::array({existing, value});
            }
        }
        else
        {
            headersJson.emplace(targetKey, nlohmann::json::array({value}));
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
    /// @param bufferStart Start of the buffer. Just past the end of the start line section (tip of the header section).
    /// @param bufferEnd End of the stream
    /// @return true/false depending on the state of the decode of headers.
    inline bool sip2json::parseHeaders(sipmessage&                  sipm,
                                       std::string::iterator&       bufferStart,
                                       const std::string::iterator& bufferEnd) noexcept(false)
    {
        using namespace std::string_literals;

        bool done {false};
        bool found {false};

        auto useCRLF             = true;
        auto headerDelimiterSize = ELEM_HEADERSECTIONDELIMITER.size();
        auto lineEndSize         = ELEM_NEWLINE.size();

        const char* pStart = std::to_address(bufferStart);
        const char* pEnd   = std::to_address(bufferEnd);
        size_t totalLen    = static_cast<size_t>(pEnd - pStart);
        std::string_view bufView(pStart, totalLen);

        auto delimPos = bufView.find("\r\n\r\n");
        if (delimPos == std::string_view::npos)
        {
            useCRLF             = false;
            lineEndSize         = ELEM_NEWLINE_LF.size();
            headerDelimiterSize = ELEM_HEADERSECTIONDELIMITER_LF.size();
            delimPos = bufView.find("\n\n");
        }

        if (delimPos == std::string_view::npos)
            throw incomplete_buffer_for_header_error {std::format("{}:Cannot find header section delimiter.", __func__).c_str()};

        auto headerEnd = bufferStart + delimPos;

        while (!done && bufferStart < headerEnd)
        {
            // Scan for the first `:`
            auto hsep = std::find(bufferStart, headerEnd, ':');
            if (hsep != headerEnd)
            {
                // Found the separator element.
                // Key is from bufferStart until the separator
                std::string_view keyView(std::to_address(bufferStart), static_cast<size_t>(hsep - bufferStart));
                if (!keyView.empty())
                {
                    std::string foldedValue {};
                    auto hval = hsep + 1; // Start past ':'

                    // Skip leading spaces or tabs in value
                    while (hval < headerEnd && (*hval == ' ' || *hval == '\t'))
                        ++hval;

                    // Process header value, handling folded headers (RFC 2822 header folding)
                    bool headerProcessed = false;
                    while (!headerProcessed)
                    {
                        auto hend = std::find(hval, headerEnd, '\n');
                        if (hend != headerEnd)
                        {
                            auto lineEnd = hend;
                            if (lineEnd != hval && *(lineEnd - 1) == '\r')
                                --lineEnd;

                            // Check if this is a folded element
                            auto nextPos = hend + 1;
                            if (nextPos < headerEnd && (*nextPos == ' ' || *nextPos == '\t'))
                            {
                                foldedValue.append(hval, lineEnd);
                                hval = nextPos + 1;
                                while (hval < headerEnd && (*hval == ' ' || *hval == '\t'))
                                    ++hval;
                            }
                            else
                            {
                                if (!foldedValue.empty())
                                {
                                    foldedValue.append(hval, lineEnd);
                                    found = storeHeaderValue(sipm, keyView, foldedValue);
                                }
                                else
                                {
                                    std::string_view valView(std::to_address(hval), static_cast<size_t>(lineEnd - hval));
                                    found = storeHeaderValue(sipm, keyView, valView);
                                }
                                bufferStart     = hend + 1;
                                headerProcessed = true;
                            }
                        }
                        else
                        {
                            // reached headerEnd
                            auto lineEnd = headerEnd;
                            if (lineEnd != hval && *(lineEnd - 1) == '\r')
                                --lineEnd;

                            if (!foldedValue.empty())
                            {
                                foldedValue.append(hval, lineEnd);
                                found = storeHeaderValue(sipm, keyView, foldedValue);
                            }
                            else
                            {
                                std::string_view valView(std::to_address(hval), static_cast<size_t>(lineEnd - hval));
                                found = storeHeaderValue(sipm, keyView, valView);
                            }
                            bufferStart     = headerEnd + headerDelimiterSize;
                            done            = true;
                            headerProcessed = true;
                        }
                    }
                }
                else
                {
                    done = true;
                }
            }
            else
            {
                done = true;
            }
        }

        if (bufferStart < headerEnd + headerDelimiterSize)
            bufferStart = headerEnd + headerDelimiterSize;

        return found;
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
        std::string::iterator       bufferStart = frameBuffer.begin();
        const std::string::iterator bufferEnd   = frameBuffer.end();
        size_t                      decodedMessageCount {0};

        while (bufferStart != bufferEnd)
        {
            try
            {
                // If the callback is provided, then we invoke the callback. Nothing is returned to caller.
                if (auto&& sipm {parseFromBuffer(bufferStart, bufferEnd)}; !sipm.empty())
                {
                    decodedMessageCount++;
                    sipm["meta"]["parseCountThisBuffer"] = decodedMessageCount;
                    parseCallback(std::move(sipm));
                }
            }
            catch (const sip2json_exception& e)
            {
                // Consolidated error handling for all sip2json exceptions
                if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
                break;
            }
            catch (const std::exception& e)
            {
                // Catch-all for standard exceptions
                sip2json_exception ex(e);
                if (errorCallback.has_value()) errorCallback.value()(ex, bufferStart, bufferEnd);
                break;
            }
            catch (...)
            {
                // Catch-all for unknown exceptions
                sip2json_exception ex("Unknown generic error");
                if (errorCallback.has_value()) errorCallback.value()(ex, bufferStart, bufferEnd);
                break;
            }
        }

        // Remove the processed elements from the buffer.
        // The bufferStart will point to the location past the point where
        // the frame was extracted.
        // We must therefore remove anything prior and upto the bufferStart
        frameBuffer.erase(frameBuffer.begin(), bufferStart);
        // reset the iterators..
        bufferStart = frameBuffer.begin();

        return frameBuffer;
    }

    /// @brief Given a buffer, parse as many frames and return the vector of messages. Re-Throws only if there was not possible to decode even a single message. Stops parsing on any additional exception.
    /// @param bufferStart Start of the buffer (modified by call to this method).
    /// @param bufferEnd End of the buffer
    /// @return Vector of sipmessage decoded within the stream.
    inline std::vector<sipmessage> sip2json::parse(std::string::iterator&       bufferStart,
                                                   const std::string::iterator& bufferEnd) noexcept(false)
    {
        std::vector<sipmessage> msgs;
        if (bufferEnd > bufferStart)
        {
            size_t estCount = std::max<size_t>(1, static_cast<size_t>(bufferEnd - bufferStart) / 1024);
            msgs.reserve(std::min<size_t>(estCount, 64));
        }
        size_t decodedMessageCount {0};

        while (bufferStart != bufferEnd)
        {
            try
            {
                // If the callback is provided, then we invoke the callback. Nothing is returned to caller.
                if (auto&& sipm {parseFromBuffer(bufferStart, bufferEnd)}; !sipm.empty())
                {
                    decodedMessageCount++;
                    sipm["meta"]["parseCountThisBuffer"] = decodedMessageCount;
                    // otherwise we push to the vector to return to caller
                    msgs.emplace_back(std::move(sipm));
                }
            }
            catch (std::exception& ex)
            {
                if (msgs.size() == 0) throw std::invalid_argument("Nothing was parsed.");
                break;
            }
        }

        return msgs;
    }

    /// @brief De-serialize the *first* SIP message (if present) from the buffer. Repeated calls to this method will extract the remaining messages.
    /// @param bufferStart iterator to the start of the buffer the client expects a SIP message.
    /// @param bufferEnd iterator to the end of the buffer the client expects a SIP message.
    /// @return A sipmessage object containing the document representing the first decoded sipmessage in the buffer.
    inline sipmessage sip2json::parseFromBuffer(std::string::iterator&       bufferStart,
                                                const std::string::iterator& bufferEnd) noexcept(false)
    {
        auto       previousBufferStart = bufferStart; // save the value so we can reset if we end up with exception.
        sipmessage sipm;
#if defined(DEBUG) || defined(_DEBUG)
        [[maybe_unused]] InvokeOnDestruct timeTaken {[&](long long delta)
                                                      {
                                                          sipm["meta"]["ttx"]  = delta;
                                                          sipm["meta"]["pre"]  = bufferStart - previousBufferStart;
                                                          sipm["meta"]["post"] = bufferEnd - bufferStart;
                                                      }}; // upon destruction, sets the ttx to account for parse time
#endif

        if (bufferStart != bufferEnd)
        {
            if (size_t diff = bufferEnd - bufferStart; diff > SIP_SAMPLE_MINIMAL_MESSAGE.length())
            {
                try
                {
                    if (auto foundRequest = parseStartLine(sipm, bufferStart, bufferEnd); foundRequest)
                    {
                        if (auto foundHeaders = parseHeaders(sipm, bufferStart, bufferEnd); foundHeaders)
                        {
                            if (sipm.getContentTypeView() == CONTENT_TYPE_APP_SDP)
                            {
                                // It is acceptable in some implementations to declare the Content-Type as application/sdp
                                // but provide no actual body. We must not fault this case.
                                if (sipm.getContentLength() > 0)
                                {
                                    // Check to make sure that we have sufficient content in the buffer
                                    // to process the body..
                                    if (auto availableRemainingBufferSize = bufferEnd - bufferStart;
                                        availableRemainingBufferSize >= sipm.getContentLength())
                                    {
                                        // We must limit the decode to the reported size of the content
                                        auto bodyEnd = bufferStart;
                                        bodyEnd += sipm.getContentLength();
                                        // Decode the SDP
                                        parseBodySDP(sipm, bufferStart, bodyEnd);
                                    }
                                    else
                                    {
                                        bufferStart = previousBufferStart;
                                        throw incomplete_buffer_for_content_error {
                                                std::format("{}: Available buffer length:{} < Content-Length:{}",
                                                            __func__,
                                                            availableRemainingBufferSize,
                                                            sipm.getContentLength())};
                                    }
                                }
                            }
                            else if (!sipm.getContentTypeView().empty())
                            {
                                bufferStart = previousBufferStart;
                                throw unsupported_contenttype_error {
                                        std::format("{}:Content-Type {} not supported", __func__, sipm.getContentType())};
                            }
                        }
                    }
                }
                catch (...)
                {
                    // We must reset the buffer to ensure that we can re-parse when there is sufficient buffer
                    bufferStart = previousBufferStart;
                    // Rethrow
                    throw;
                }
            }
            else
            {
                // This will end our scan.
                bufferStart = previousBufferStart;
                throw incomplete_buffer_for_parse_error {std::format("{}:Incomplete Buffer for parse to continue.", __func__)};
            }
        }

        // Let the compiler perform copy-elison; don't use move here!
        return sipm;
    }
} // namespace siddiqsoft
