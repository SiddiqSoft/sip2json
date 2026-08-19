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
    inline bool sip2json::parseStartLine(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
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

    /// @brief Store the value in the header section. Performs from basic transforms/detection of bool, integer
    /// @param sipm The target sipmessage object
    /// @param key The key
    /// @param value The value
    /// @return Returns true if the store was successful.
    inline bool sip2json::storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value) noexcept(false)
    {
        if (key.compare(HF_VIA) == 0)
        {
            // Via is an array
            sipm["h"][HF_VIA].push_back(value);
        }
        else if (key.compare("uthorization") == 0)
        {
            // Some encoders send "uthorization" instead of "Authorization"
            sipm["h"][HF_AUTHORIZATION] = value;
        }
        else if (key.compare(HF_CONTENT_TYPE) == 0 || key.compare(HF_CONTENT_TYPE2) == 0)
        {
            // Some encoders send Content-type instead of the standard Content-Type; here we normalize it.
            sipm["h"][HF_CONTENT_TYPE] = value;
        }
        else if (key.compare(HF_CONTENT_LENGTH) == 0)
        {
            try
            {
                long long len = std::stoll(value);
                if (len < 0 || len > 100 * 1024 * 1024)
                    throw invalid_document_error {std::format("{}:Invalid Content-Length value '{}'", __func__, value)};
                sipm["h"][key] = static_cast<uint32_t>(len);
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
        else if (key.compare(HF_EXPIRES) == 0)
        {
            try
            {
                long long val = std::stoll(value);
                if (val < 0)
                    throw invalid_document_error {std::format("{}:Invalid Expires value '{}'", __func__, value)};
                sipm["h"][key] = static_cast<uint32_t>(val);
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
        else if (value.empty()) { sipm["h"][key] = ""; }
        else
        {
            sipm["h"][key] = value;
        }

        return true;
    }

    /// @brief Decode headers within the stream
    /// @param sipm Destination sipmessage
    /// @param bufferStart Start of the buffer. Just past the end of the start line section (tip of the header section).
    /// @param bufferEnd End of the stream
    /// @return true/false depending on the state of the decode of headers.
    inline bool sip2json::parseHeaders(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
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
            throw incomplete_buffer_for_header_error {
                    std::format("{}:Cannot find header section delimiter.", __func__).c_str()};

        while (!done)
        {
            // Scan for the first `:`
            auto hsep = std::search(bufferStart, headerEnd, ELEM_SEPERATOR.begin(), ELEM_SEPERATOR.end());
            if (hsep != headerEnd)
            {
                // Found the separator element.
                // Key is from bufferStart until the separator
                if (std::string key(bufferStart, hsep); !key.empty())
                {
                    std::string value {};
                    auto        hval = hsep; // Store the location of the value part of the header element.

                    // Next, let's look for the end of element
                    bufferStart = hsep += ELEM_SEPERATOR.size();

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
