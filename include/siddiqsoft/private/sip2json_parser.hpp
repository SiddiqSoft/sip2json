/*
    A SIP Parser for Modern C++: SIP Message Parser Implementation
    Version 1.0.0
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
                sipm["s"s] = {{"type"s, SIPMessageType::request},
                              {"method"s, string(g1)},
                              {"uri"s, string(g2)},
                              {"version"s, string(g3)}};
            }
            else if (SIPVER_20 == g1)
            {
                sipm["s"s] = {{"type"s, SIPMessageType::response},
                              {"reason"s, string(g3)},
                              {"status"s, std::stoi(string(g2))},
                              {"version"s, string(g1)}};
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

    struct CanonicalHeaderKeyResult
    {
        bool             isCanonical {false};
        bool             isMultiLine {false};
        std::string_view canonicalKey {};
                         operator std::string() const { return std::string {canonicalKey}; }
    };

    inline CanonicalHeaderKeyResult canonicalizeHeaderKey(const std::string& keyFromPayload)
    {
        // Convert the key to lowercase for comparison
        std::string lowerKey;
        lowerKey.reserve(keyFromPayload.size());
        std::transform(keyFromPayload.begin(), keyFromPayload.end(), std::back_inserter(lowerKey), ::tolower);

        // compare the lowerKey against the known header key sets and return the canonical form if found
        // compare the lowerKey against the known header key sets and return the canonical form if found
        // we match against the lowercase version of the key or the abbreviation (if present) in the HeaderKeySet
        // Note: The Authorization header has a special case where some implementations send "uthorization" instead of "Authorization".
        if (HFS_AUTHORIZATION[0] == lowerKey || HFS_AUTHORIZATION[2] == lowerKey || "uthorization" == lowerKey)
            return {true, false, HFS_AUTHORIZATION[1]};

        if (HFS_FROM[0] == lowerKey || HFS_FROM[2] == lowerKey) return {true, false, HFS_FROM[1]};
        if (HFS_TO[0] == lowerKey || HFS_TO[2] == lowerKey) return {true, false, HFS_TO[1]};
        if (HFS_PRIORITY[0] == lowerKey || HFS_PRIORITY[2] == lowerKey) return {true, false, HFS_PRIORITY[1]};
        if (HFS_CONTENT_ENCODING[0] == lowerKey || HFS_CONTENT_ENCODING[2] == lowerKey)
            return {true, false, HFS_CONTENT_ENCODING[1]};
        if (HFS_CONTENT_LENGTH[0] == lowerKey || HFS_CONTENT_LENGTH[2] == lowerKey) return {true, false, HFS_CONTENT_LENGTH[1]};
        if (HFS_CONTENT_TYPE[0] == lowerKey || HFS_CONTENT_TYPE[2] == lowerKey) return {true, false, HFS_CONTENT_TYPE[1]};
        if (HFS_CALLID[0] == lowerKey || HFS_CALLID[2] == lowerKey) return {true, false, HFS_CALLID[1]};
        if (HFS_CSEQ[0] == lowerKey || HFS_CSEQ[2] == lowerKey) return {true, false, HFS_CSEQ[1]};
        if (HFS_VIA[0] == lowerKey || HFS_VIA[2] == lowerKey) return {true, true, HFS_VIA[1]};
        if (HFS_ENCRYPTION[0] == lowerKey || HFS_ENCRYPTION[2] == lowerKey) return {true, false, HFS_ENCRYPTION[1]};
        if (HFS_SUBJECT[0] == lowerKey || HFS_SUBJECT[2] == lowerKey) return {true, false, HFS_SUBJECT[1]};
        if (HFS_LOCATION[0] == lowerKey || HFS_LOCATION[2] == lowerKey) return {true, false, HFS_LOCATION[1]};
        if (HFS_EXPIRES[0] == lowerKey || HFS_EXPIRES[2] == lowerKey) return {true, false, HFS_EXPIRES[1]};
        if (HFS_CONTACT[0] == lowerKey || HFS_CONTACT[2] == lowerKey) return {true, false, HFS_CONTACT[1]};
        if (HFS_ACCEPT[0] == lowerKey || HFS_ACCEPT[2] == lowerKey) return {true, true, HFS_ACCEPT[1]};
        if (HFS_ACCEPT_ENCODING[0] == lowerKey || HFS_ACCEPT_ENCODING[2] == lowerKey) return {true, false, HFS_ACCEPT_ENCODING[1]};
        if (HFS_ACCEPT_LANGUAGE[0] == lowerKey || HFS_ACCEPT_LANGUAGE[2] == lowerKey) return {true, false, HFS_ACCEPT_LANGUAGE[1]};
        if (HFS_DATE[0] == lowerKey || HFS_DATE[2] == lowerKey) return {true, false, HFS_DATE[1]};
        if (HFS_RECORD_ROUTE[0] == lowerKey || HFS_RECORD_ROUTE[2] == lowerKey) return {true, true, HFS_RECORD_ROUTE[1]};
        if (HFS_TIMESTAMP[0] == lowerKey || HFS_TIMESTAMP[2] == lowerKey) return {true, false, HFS_TIMESTAMP[1]};
        if (HFS_HIDE[0] == lowerKey || HFS_HIDE[2] == lowerKey) return {true, false, HFS_HIDE[1]};
        if (HFS_MAX_FORWARDS[0] == lowerKey || HFS_MAX_FORWARDS[2] == lowerKey) return {true, false, HFS_MAX_FORWARDS[1]};
        if (HFS_ORGANIZATION[0] == lowerKey || HFS_ORGANIZATION[2] == lowerKey) return {true, false, HFS_ORGANIZATION[1]};
        if (HFS_PROXY_AUTHORIZATION[0] == lowerKey || HFS_PROXY_AUTHORIZATION[2] == lowerKey)
            return {true, false, HFS_PROXY_AUTHORIZATION[1]};
        if (HFS_PROXY_REQUIRE[0] == lowerKey || HFS_PROXY_REQUIRE[2] == lowerKey) return {true, false, HFS_PROXY_REQUIRE[1]};
        if (HFS_ROUTE[0] == lowerKey || HFS_ROUTE[2] == lowerKey) return {true, true, HFS_ROUTE[1]};
        if (HFS_REQUIRE[0] == lowerKey || HFS_REQUIRE[2] == lowerKey) return {true, false, HFS_REQUIRE[1]};
        if (HFS_RESPONSE_KEY[0] == lowerKey || HFS_RESPONSE_KEY[2] == lowerKey) return {true, false, HFS_RESPONSE_KEY[1]};
        if (HFS_USER_AGENT[0] == lowerKey || HFS_USER_AGENT[2] == lowerKey) return {true, false, HFS_USER_AGENT[1]};
        if (HFS_PROXY_AUTHENTICATE[0] == lowerKey || HFS_PROXY_AUTHENTICATE[2] == lowerKey)
            return {true, false, HFS_PROXY_AUTHENTICATE[1]};
        if (HFS_RETRY_AFTER[0] == lowerKey || HFS_RETRY_AFTER[2] == lowerKey) return {true, false, HFS_RETRY_AFTER[1]};
        if (HFS_SERVER[0] == lowerKey || HFS_SERVER[2] == lowerKey) return {true, false, HFS_SERVER[1]};
        if (HFS_SUPPORTED[0] == lowerKey || HFS_SUPPORTED[2] == lowerKey) return {true, true, HFS_SUPPORTED[1]};
        if (HFS_ALLOW[0] == lowerKey || HFS_ALLOW[2] == lowerKey) return {true, false, HFS_ALLOW[1]};
        if (HFS_UNSUPPORTED[0] == lowerKey || HFS_UNSUPPORTED[2] == lowerKey) return {true, false, HFS_UNSUPPORTED[1]};
        if (HFS_WARNING[0] == lowerKey || HFS_WARNING[2] == lowerKey) return {true, true, HFS_WARNING[1]};
        if (HFS_WWW_AUTHENTICATE[0] == lowerKey || HFS_WWW_AUTHENTICATE[2] == lowerKey)
            return {true, false, HFS_WWW_AUTHENTICATE[1]};
        if (HFS_AUTHORIZATION[0] == lowerKey || HFS_AUTHORIZATION[2] == lowerKey) return {true, false, HFS_AUTHORIZATION[1]};
        if (HFS_SUBSCRIPTION_STATE[0] == lowerKey || HFS_SUBSCRIPTION_STATE[2] == lowerKey)
            return {true, false, HFS_SUBSCRIPTION_STATE[1]};

        // Return original keyFromPayload if no match found for custom headers
        return {false, false, keyFromPayload};
    }


    /// @brief Store the value in the header section. Performs from basic transforms/detection of bool, integer
    /// @param sipm The target sipmessage object
    /// @param key The key
    /// @param value The value
    /// @return Returns true if the store was successful.
    inline bool sip2json::storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value) noexcept(false)
    {
        auto targetKey = canonicalizeHeaderKey(key);

        // Check if we already have this header key in the sipmessage. If so, we need to handle it as a multi-line header.
        if (sipm["h"].contains(targetKey))
        {
            // If the header is already present, we need to handle it as a multi-line header.
            // We will store the values in an array.
            if (sipm["h"][targetKey].is_array())
                sipm["h"][targetKey].push_back(value);
            else
            {
                auto existing        = sipm["h"][targetKey];
                sipm["h"][targetKey] = nlohmann::json::array({existing, value});
            }
        }
        else if (targetKey.isMultiLine)
        {
            // These headers can be multi-line, so we store them as an array of values.
            sipm["h"][targetKey] = nlohmann::json::array({value});
        }
        else if (targetKey.canonicalKey == HFS_CONTENT_LENGTH[1])
        {
            try
            {
                long long len = std::stoll(value);
                if (len < 0 || len > 100 * 1024 * 1024)
                    throw invalid_document_error {std::format("{}:Invalid Content-Length value '{}'", __func__, value)};
                sipm["h"][targetKey] = static_cast<uint32_t>(len);
            }
            catch (const invalid_document_error&)
            {
                throw;
            }
            catch (const std::exception&)
            {
                throw invalid_document_error {std::format("{}:Invalid Content-Length value '{}'", __func__, value)};
            }
        }
        else if (targetKey.canonicalKey == HFS_EXPIRES[1])
        {
            try
            {
                long long val = std::stoll(value);
                if (val < 0) throw invalid_document_error {std::format("{}:Invalid Expires value '{}'", __func__, value)};
                sipm["h"][targetKey] = static_cast<uint32_t>(val);
            }
            catch (const invalid_document_error&)
            {
                throw;
            }
            catch (const std::exception&)
            {
                throw invalid_document_error {std::format("{}:Invalid Expires value '{}'", __func__, value)};
            }
        }
        else if (value.empty()) { sipm["h"][targetKey] = ""; }
        else
        {
            sipm["h"][targetKey] = value;
        }

        return true;
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

        // WARNING
        // The bufferStart must point to the start of the first sequence (excluding the CRLF) after the startline is processed!
        // Scan for the location of the header section end within the frame.
        // If we don't have one, then we should bail out.
        // Note that for response messages, it is likely that the bufferEnd will also be the headerEnd (no content).
        auto useCRLF             = true;
        auto headerDelimiterSize = ELEM_HEADERSECTIONDELIMITER.size();
        auto lineEndSize         = ELEM_NEWLINE.size();
        auto headerEnd =
                std::search(bufferStart, bufferEnd, ELEM_HEADERSECTIONDELIMITER.begin(), ELEM_HEADERSECTIONDELIMITER.end());
        if (headerEnd == bufferEnd)
        {
            useCRLF             = false;
            lineEndSize         = ELEM_NEWLINE_LF.size();
            headerDelimiterSize = ELEM_HEADERSECTIONDELIMITER_LF.size();
            // If not found, then search for the header without the CRLF and just the LF pair.
            headerEnd = std::search(
                    bufferStart, bufferEnd, ELEM_HEADERSECTIONDELIMITER_LF.begin(), ELEM_HEADERSECTIONDELIMITER_LF.end());
        }
        // Assert header end delimiter must exist!
        auto headerSectionSize = size_t(bufferEnd - headerEnd);
        if (headerSectionSize < headerDelimiterSize)
            throw incomplete_buffer_for_header_error {std::format("{}:Cannot find header section delimiter.", __func__).c_str()};

        while (!done)
        {
            // Scan for the first `:`
            auto hsep = std::search(bufferStart, headerEnd, ELEM_SEPARATOR.begin(), ELEM_SEPARATOR.end());
            if (hsep != headerEnd)
            {
                // Found the separator element.
                // Key is from bufferStart until the separator
                if (std::string key(bufferStart, hsep); !key.empty())
                {
                    std::string value {};
                    auto        hval = hsep; // Store the location of the value part of the header element.

                    // Next, let's look for the end of element
                    bufferStart = hsep += ELEM_SEPARATOR.size();

                    // Skip over the leading "space" if found.
                    if (*bufferStart == ' ') bufferStart = ++hsep;

                    // Process header value, handling folded headers (RFC 2822 header folding)
                    bool headerProcessed = false;
                    while (!headerProcessed)
                    {
                        auto hend = useCRLF ? search(hsep, headerEnd, ELEM_NEWLINE.begin(), ELEM_NEWLINE.end())
                                            : search(hsep, headerEnd, ELEM_NEWLINE_LF.begin(), ELEM_NEWLINE_LF.end());
                        if (hend != headerEnd)
                        {
                            // We found the `\r\n`;
                            // Next, check if this is a folded element
                            if ((headerEnd != (hend + lineEndSize)) &&
                                (hend + lineEndSize < headerEnd) && // ensure we don't read past the header end
                                ((*(hend + lineEndSize) == ' ') ||
                                 (*(hend + lineEndSize) == '\t'))) // peek ahead to see if we have.. folded indicator
                            {
                                // Yes, we have a folded item.
                                // build up the value..
                                value.append(hsep, hend);
                                // Advance to past the fold indicator
                                hsep = std::min(hend + lineEndSize + 1, headerEnd);
                                // Continue loop to process next folded line
                            }
                            else
                            {
                                value.append(hsep, hend);
                                found           = storeHeaderValue(sipm, key, value);
                                bufferStart     = hend += lineEndSize;
                                headerProcessed = true;
                            }
                        }
                        else
                        {
                            // reached the end; We're done
                            value.append(hsep, hend);
                            found           = storeHeaderValue(sipm, key, value);
                            bufferStart     = headerEnd + headerDelimiterSize;
                            done            = true;
                            headerProcessed = true;
                        }
                    }
                }
                else
                {
                    // Key is empty; we're done.
                    done = true;
                }
            }
            else
            {
                // End of buffer or Could not find separator; we're done.
                done = true;
            }
        }

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
        size_t                  decodedMessageCount {0};

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
                            if (sipm.getContentType() == CONTENT_TYPE_APP_SDP)
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
                            else if (!sipm.getContentType().empty())
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
