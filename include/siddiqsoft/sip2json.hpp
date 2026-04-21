/*
    A SIP Parser for Modern C++
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

#include <exception>
#ifndef SIP2JSON_HPP
#define SIP2JSON_HPP

#include <algorithm>
#include <string>
#include <regex>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>
#include <functional>
#include <optional>
#include <format>

#include "nlohmann/json.hpp"

#include "private/sip2json_exception.hpp"
#include "private/sip2json_response_codes.hpp"
#include "private/sip2json_utils.hpp"

#include "sipmessage.hpp"


namespace siddiqsoft
{
#pragma region SIP match patterns
#pragma endregion

    /// @brief SIP message encoder and decoder utility class
    class sip2json
    {
    public:
#pragma region Parsing helpers
    private:
        /// @brief Parse the start line
        /// @param sipm Destination sipmessage
        /// @param bufferStart Start of the stream.
        /// @param bufferEnd End of the stream
        /// @return true/false depending on the state of the decode of start line.
        static bool
        parseStartLine(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
        {
            using namespace std;

            auto useCRLF              = true;
            auto lineEndDelimiterSize = ELEM_NEWLINE.size();

            match_results<string::iterator> matchStartLine;

            auto found = regex_search(bufferStart, bufferEnd, matchStartLine, SIP_PATTERN_STARTLINE);
            // Did we find a message..?
            if (found && (matchStartLine.size() >= 3))
            {
                // The regex is very precise and there is no chance we will end up here
                // with an ill-formed (or unsupported) start-line.
                if (SIPVER_20.compare(matchStartLine[3]) == 0)
                {
                    sipm["s"s] = {{"type"s, SIPMessageType::request},
                                  {"method"s, matchStartLine[1]},
                                  {"uri"s, matchStartLine[2]},
                                  {"version"s, matchStartLine[3]}};
                }
                else if (SIPVER_20.compare(matchStartLine[1]) == 0)
                {
                    sipm["s"s] = {{"type"s, SIPMessageType::response},
                                  {"reason"s, matchStartLine[3]},
                                  {"status"s, stoi(matchStartLine[2].str())},
                                  {"version"s, matchStartLine[1]}};
                }

                // Offset the start to the point after the start-line. Make sure to skip over any prefix!
                // We may have junk or left-over crud at the start (especially if we're using text files)
                bufferStart += (matchStartLine.length() + matchStartLine.prefix().length());
            }
            else { throw invalid_startline_error {std::format("{} - SIP Startline not found.", __func__)}; }

            return found;
        }


        /// @brief Store the value in the header section. Performs from basic transforms/detection of bool, integer
        /// @param sipm The target sipmessage object
        /// @param key The key
        /// @param value The value
        /// @return Returns true if the store was successful.
        static bool storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value)
        {
            if (key.find(HF_VIA) == 0)
            {
                // Via is an array
                sipm["h"][HF_VIA].push_back(value);
            }
            else if (key.compare("uthorization") == 0)
            {
                // Some encoders send "uthorization" instead of "Authorization"
                sipm["h"][HF_AUTHORIZATION] = value;
            }
            else if (key.compare(HF_CONTENT_TYPE) == 0)
            {
                // Some encoders send Content-type instead of the standard Content-Type; here we need to normalize it.
                sipm["h"][HF_CONTENT_TYPE] = value;
            }
            else if (key.find(HF_CONTENT_LENGTH) == 0) { sipm["h"][key] = std::stoi(value); }
            else if (key.find(HF_EXPIRES) == 0) { sipm["h"][key] = std::stoi(value); }
            // This helper causes issues when the payload may contain "true" or "false" as a string value not intended as boolean
            // This approach allows the client the ultimate authority for decoding the data.
            //else if (value.find("true") == 0)
            //{
            //	sipm["h"][key] = true;
            //}
            //else if (value.find("false") == 0)
            //{
            //	sipm["h"][key] = false;
            //}
            else if (value.empty()) { sipm["h"][key] = ""; }
            else { sipm["h"][key] = value; }

            return true;
        }


        /// @brief Decode headers within the stream
        /// @param sipm Destination sipmessage
        /// @param bufferStart Start of the buffer. Just past the end of the start line section (tip of the header section).
        /// @param bufferEnd End of the stream
        /// @return true/false depending on the state of the decode of headers.
        static bool
        parseHeaders(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
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

                    label_recummulate_to_unfold_buffer:
                        auto hend = useCRLF ? search(hsep, headerEnd, ELEM_NEWLINE.begin(), ELEM_NEWLINE.end())
                                            : search(hsep, headerEnd, ELEM_NEWLINE_LF.begin(), ELEM_NEWLINE_LF.end());
                        if (hend != headerEnd)
                        {
                            // We found the `\r\n`;
                            // Next, check if this is a folded element
                            if ((headerEnd != (hend + lineEndSize + 1)) &&
                                ((*(hend + lineEndSize + 1) == ' ') ||
                                 (*(hend + lineEndSize + 1) == '\t'))) // peek ahead to see if we have.. folded indicator
                            {
                                // Yes, we have a folded item.
                                // build up the value..
                                value.append(hsep, hend);
                                // Advance to past the fold portion
                                hsep = hend + lineEndSize + 1;
                                // Go back to find the next section..
                                goto label_recummulate_to_unfold_buffer;
                            }
                            else
                            {
                                value.append(hsep, hend);
                                found       = storeHeaderValue(sipm, key, value);
                                bufferStart = hend += lineEndSize;
                            }
                        }
                        else
                        {
                            // reached the end; We're done
                            value.append(hsep, hend);
                            found       = storeHeaderValue(sipm, key, value);
                            bufferStart = headerEnd + headerDelimiterSize;
                            done        = true;
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


        /// @brief Decode a SDP message blocks
        /// @param sipm Destination sipmessage
        /// @param bufferStart Start of the buffer. Just past the end of the header section (tip of the content section).
        /// @param bufferEnd End of the content area (not the end of the stream)
        /// @return true/false depending on the state of the decode of SDP blocks.
        static bool
        parseBodySDP(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
        {
            using namespace std;

            match_results<string::iterator> matcher;
            bool                            found = false;

            // NOTE: bufferStart points to the location past the very first v=0 as this is the signal of the start
            // of the body. Therefore, we start with blockIndex = 0 and then increment everytime we encounter next v=0
            int32_t blockIndex = -1;

            while ((bufferStart < bufferEnd) && regex_search(bufferStart, bufferEnd, matcher, SIP_PATTERN_BODY))
            {
                if (matcher.size() == 3)
                {
                    auto key   = matcher[1].str();
                    auto value = matcher[2].str();

                    found = true;
                    if (key == "v"s)
                    {
                        // First element; increment blockIndex.
                        // Add the next element to a new SDP object.
                        blockIndex++; // the first match will increment this to "0"
                        sipm["b"s]["sdp"s][blockIndex][key] = 0;
                    }
                    else if (key == "a"s)
                    {
                        // attribute lines: https://en.wikipedia.org/wiki/Session_Description_Protocol#Attributes
                        match_results<string::iterator> alineMatcher;

                        if (regex_search(value.begin(), value.end(), alineMatcher, SIP_PATTERN_BODY_ALINE) &&
                            alineMatcher.size() >= 3)
                        {
                            // This is the form where a=attribute:value
                            nlohmann::json::json_pointer pkey(
                                    std::format("/b/sdp/{}/{}/{}", blockIndex, key, alineMatcher[1].str()));

                            // We may get multiple items for the same "key" such as `a=rtpmap:x` and `a=rtpmap:y`
                            // In this case we should start an array
                            if (sipm.contains(pkey) && !sipm[pkey].is_array())
                            {
                                auto previousValue = sipm[pkey]; // make a copy!
                                sipm[pkey]         = {previousValue, alineMatcher[2].str()};
                                //nlohmann::json::json_pointer pkeyUpOneLevel(std::format("/b/sdp/{}/{}", blockIndex, key));
                                //if (sipm[pkeyUpOneLevel].erase(alineMatcher[1].str()) == 1)
                                //{
                                //	// Push the first item
                                //	sipm[pkey].push_back(previousValue);
                                //	// Push the current item
                                //	sipm[pkey].push_back(alineMatcher[2].str());
                                //}
                                //else
                                //{
                                //	sip2json_throw<unsupported_contenttype_error>(
                                //			"{}:Failed removing {} from sipmessage.", __func__, string(pkey));
                                //}
                            }
                            else if (sipm[pkey].is_array())
                                sipm[pkey].push_back(alineMatcher[2].str());
                            else if (alineMatcher.length() > 0)
                                sipm[pkey] = alineMatcher[2].str();
                            else
                                sipm[pkey] = nullptr;
                        }
                        else if (!value.empty())
                        {
                            // This is the form where a=flag
                            // We matched a=key without the `:` or the "value" so we should store the value with nullptr
                            nlohmann::json::json_pointer pkey(std::format("/b/sdp/{}/{}/{}", blockIndex, key, value));
                            sipm[pkey] = true;
                        }
                    }
                    else
                    {
                        nlohmann::json::json_pointer pkey(std::format("/b/sdp/{}/{}", blockIndex, key));

                        if (key == "c"s)
                        {
                            match_results<string::iterator> clineMatcher;
                            if (regex_search(value.begin(), value.end(), clineMatcher, SIP_PATTERN_BODY_CLINE) &&
                                clineMatcher.size() >= 3)
                            {
                                sipm[pkey] = nlohmann::json {
                                        {"type"s, clineMatcher[1]}, {"subtype"s, clineMatcher[2]}, {"dn"s, clineMatcher[3]}};
                            }
                            else if (!value.empty()) { sipm[pkey] = value; }
                        }
                        else if (key == "o"s)
                        {
                            match_results<string::iterator> olineMatcher;
                            if (regex_search(value.begin(), value.end(), olineMatcher, SIP_PATTERN_BODY_OLINE) &&
                                olineMatcher.size() >= 6)
                            {
                                sipm[pkey] = nlohmann::json {{"user"s, olineMatcher[1]},
                                                             {"t1"s, olineMatcher[2]},
                                                             {"t2"s, olineMatcher[3]},
                                                             {"type"s, olineMatcher[4]},
                                                             {"subtype"s, olineMatcher[5]},
                                                             {"host"s, olineMatcher[6]}};
                            }
                            else if (!value.empty()) { sipm[pkey] = value; }
                        }
                        else if (key.compare("i") == 0)
                        {
                            // Identity and number and type of call.
                            match_results<string::iterator> ilineMatcher;
                            if (regex_search(value.begin(), value.end(), ilineMatcher, SIP_PATTERN_BODY_ILINE) &&
                                ilineMatcher.size() >= 3)
                            {
                                auto iName = ilineMatcher[1].str();
                                // Set the name but check to ensure that if we have a " in the name that we strip it..
                                sipm[pkey] = nlohmann::json {
                                        {"name"s, iName.starts_with("\""s) ? iName.substr(1, iName.length() - 2) : iName},
                                        {"dn"s, ilineMatcher[2]},
                                        {"type"s, ilineMatcher[3]}};
                            }
                            else if (!value.empty()) { sipm[pkey] = value; }
                            else { sipm[pkey] = ""; }
                        }
                        else if (key.compare("t"s) == 0)
                        {
                            // timing
                            uint32_t ts = 0, te = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(WINDOWS) || defined(WIN32)
                            if (::sscanf_s(value.c_str(), "%d %d", &ts, &te) > 0)
#else
                            if (std::sscanf(value.c_str(), "%d %d", &ts, &te) > 0)
#endif
                            {
                                sipm[pkey].push_back(ts);
                                sipm[pkey].push_back(te);
                            }
                        }
                        else if (!key.empty() && value.empty()) { sipm[pkey] = ""; }
                        else if (!key.empty()) { sipm[pkey] = value; }
                    }
                }
                // Offset the start to the point after the start-line.
                bufferStart += matcher.length() + ELEM_NEWLINE.size();
            }

            return found;
        }
#pragma endregion

    public:
        /// @brief Given a buffer, parse each message and invoke the callback with the decoded sipmessage object.
        /// @param bufferStart Start of the buffer (modified by call to this method).
        /// @param bufferEnd End of the buffer
        /// @param parseCallback Callback which takes a reference to the sipmessage just decoded. If present, the return is empty vector.
        /// @param errorCallback Optional callback to handle the error on the parse.
        /// @return Returns the remaininng contents of the buffer.
        [[nodiscard("Remaining contents of the buffer")]] static std::string& parseAsync(
                std::string&                      frameBuffer,
                std::function<void(sipmessage&&)> parseCallback,
                std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>>
                        errorCallback = {}) noexcept
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
                catch (const invalid_startline_error& e)
                {
                    // If the very first one has issues then we should quit.
                    if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
                    break;
                }
                catch (const unsupported_contenttype_error& e)
                {
                    // If the very first one has issues then we should quit.
                    if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
                    break;
                }
                catch (const incomplete_buffer_for_parse_error& e)
                {
                    // If the very first one has issues then we should quit.
                    if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
                    break;
                }
                catch (const incomplete_buffer_for_header_error& e)
                {
                    if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
                    break;
                }
                catch (const incomplete_buffer_for_content_error& e)
                {
                    if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
                    break;
                }
                catch (const std::exception& e)
                {
                    // Just in case catch-all
                    sip2json_exception ex(e);

                    if (errorCallback.has_value()) errorCallback.value()(ex, bufferStart, bufferEnd);
                    break;
                }
                catch (...)
                {
                    // Just in case catch-all
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

            return frameBuffer;
        }

        /// @brief Given a buffer, parse as many frames and return the vector of messages. Re-Throws only if there was not possible to decode even a single message. Stops parsing on any additional exception.
        /// @param bufferStart Start of the buffer (modified by call to this method).
        /// @param bufferEnd End of the buffer
        /// @return If parseCallback is provided then the return vector is empty otherwise vector of sipmessage decoded within the stream.
        [[nodiscard]] static std::vector<sipmessage> parse(std::string::iterator&       bufferStart,
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
        [[nodiscard]] static sipmessage parseFromBuffer(std::string::iterator&       bufferStart,
                                                        const std::string::iterator& bufferEnd) noexcept(false)
        {
            auto       previousBufferStart = bufferStart; // save the value so we can reset if we end up with exception.
            sipmessage sipm;
#if defined(DEBUG) || defined(_DEBUG)
            InvokeOnDestruct timeTaken {[&](long long delta)
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


        /// @brief Serializes the sipmessage document
        /// @param sipm Source sipmessage
        /// @return Return serialized sipmessage
        static std::string serialize(sipmessage& sipm) noexcept(false)
        {
            using namespace std;

            static const std::string supportedMethods {
                    "MESSAGE|INFO|INVITE|ACK|OPTIONS|BYE|CANCEL|REGISTER|SUBSCRIBE|NOTIFY|SIP/2.0"s};
            std::string buffer {};
            std::string contentType {};

            // Reserve the size of a typical SIP Message. Typical message size of 3K
            buffer.reserve(3 * 1024);

            // Assert: non-empty json document
            if (sipm.size() == 0) throw empty_message_error {std::format("{}:sipm is empty.", __func__)};
            // Assert: non-empty json document; starting with v1.9 we have a meta element for diagnostics; this is to be treated as "empty".
            if (sipm.contains("meta") && sipm.size() == 1)
                throw empty_message_error {std::format("{}:sipm is empty (except for meta).", __func__)};
            // Assert: Method is one of the supported items
            if (supportedMethods.find(sipm.getMethod()) == std::string::npos)
                throw invalid_document_error {std::format("{}:Unsupported method:{}", __func__, sipm.getMethod())};
            // Assert: Header must exist
            if (!sipm.contains("h"s)) throw invalid_document_error {std::format("{}:sipm does not contain `h`eaders.", __func__)};

            if (sipm.isMessageRequest())
            {
                // Request Line
                std::format_to(std::back_inserter(buffer), "{} {} SIP/2.0\r\n", sipm.getMethod(), sipm.getUri());
            }
            else if (sipm.isMessageResponse())
            {
                // Status Line
                std::format_to(std::back_inserter(buffer), "SIP/2.0 {} {}\r\n", sipm.getStatusCode(), sipm.getReason());
            }
            else
            {
                throw invalid_document_error {std::format(
                        "{}:sipm /type is neither `SIPMessageType::request` nor `SIPMessageType::response`.", __func__)};
            }

            // Encode the body first so we can get the content-length properly.
            auto body = serializeSDP(sipm);
            sipm.setHeader("Content-Length"s, body.length());

            // Headers
            if (auto mh = sipm.headers(); mh.size() > 0)
            {
                //TODO: This will not care about the order of the serialized headers. The json library does not care about order.
                for (auto& [key, val] : sipm.headers().items())
                {
                    if (contentType.empty() && (key.compare(HF_CONTENT_TYPE) == 0) && val.is_string()) contentType = val;

                    if (val.is_null())
                    {
                        // For null entries, put a blank entry. This is the same as our decode
                        std::format_to(std::back_inserter(buffer), "{}: \r\n", key.c_str());
                    }
                    else if (val.is_number_unsigned())
                    {
                        std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<uint64_t>());
                    }
                    else if (val.is_number_integer() || val.is_number())
                    {
                        std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<int64_t>());
                    }
                    else if (val.is_number_float())
                    {
                        std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<float>());
                    }
                    else if (val.is_string())
                    {
                        std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, val.get<std::string>().c_str());
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
                            std::format_to(std::back_inserter(buffer), "{}: {}\r\n", key, iv.get<std::string>().c_str());
                        }
                    }
                    //else
                    //{ // unsupported/unknown
                    //    std::format_to(std::back_inserter(buffer), "{}: {{}}\r\n"s, key, val);
                    //}
                };

                // End of the message header section
                buffer += ELEM_NEWLINE;
            }

            // Add the body
            buffer += body;

            // At the end we must have a complete and true serialized (ready for the wire) sip message
            return buffer;
        }

    private:
        /// @brief Serializes the SDP content
        /// @param sipm sipmessage object
        /// @return string representing the sdp
        static std::string serializeSDP(sipmessage& sipm) noexcept(false)
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
                    else { throw invalid_document_error {std::format("{}:sipm `b`ody does not have sdp element.", __func__)}; }
                }
                else
                {
                    // This should not be an error; there are live SIP messages where the client sets the Content-Type
                    // but also sets the Content-Length to `0` so we should avoid encoding anything.
                    //sip2json_throw<invalid_document_error>("{}:sipm does not have b.", __func__);
                }
            }
            else if ((contentType.compare(CONTENT_TYPE_TEXT_PLAIN) == 0) && (sipm.contains("b"s) && sipm.body().is_string()))
            {
                buffer += sipm.body();
            }

            return buffer;
        }


        /// @brief Helper to serialize the SDP element with custom decode
        /// @param sdpBlock The SDP block from the SDP array
        /// @param element The element: o, s, i, c, t, m, a. When returning a= the code builds CRLF terminators.
        /// @return Returns the sdp element as string.
        static std::string serializeSDPelement(nlohmann::json& sdpBlock, const std::string& element)
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
                                    //std::format_to(std::back_inserter(ret), "a={}:{}\r\n"s, kv, i.value().get<std::string>());
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
                    if (element == "t"s) { return std::format("{} {}", item[0].get<uint32_t>(), item[1].get<uint32_t>()); }
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
    }; // class sip2json


    // References
    // SIP Messages: https://tools.ietf.org/html/rfc3261#section-7
    // SDP Message format: https://en.wikipedia.org/wiki/Session_Description_Protocol
    // SIP Response Codes: https://en.wikipedia.org/wiki/List_of_SIP_response_codes
    // JSON Library: https://nlohmann.github.io/json/
} // namespace siddiqsoft

#endif
