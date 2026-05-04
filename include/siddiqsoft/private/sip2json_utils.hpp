/*
    A SIP Parser for Modern C++: Utilities and Helpers
    Version 1.0.0
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License

    Copyright (c) 2003-2020, Abdelkareem Siddiq
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
#include <chrono>
#include <random>
#include <sstream>
#include <optional>
#include <format>

#include "ctre.hpp"
#include "nlohmann/json.hpp"

namespace siddiqsoft
{
#pragma region Datetime helpers
    /// @brief Helper struct which runs your lambda when this object goes out of scope. Used to time expression scope.
    /// @tparam Fn Lambda of type void(long long delta) called upon destructor; must not throw.
    template <typename Fn> struct InvokeOnDestruct
    {
        Fn                                          callbackOnEnd;
        const std::chrono::system_clock::time_point ttxStart {std::chrono::system_clock::now()};

        /// @brief Gets the time delta between start/instantiation of this object and now
        /// @return long long type delta
        auto ttx() noexcept
        {
            const auto ttxNow = std::chrono::system_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(ttxNow - ttxStart).count();
        };

        // Must provide a callback!
        InvokeOnDestruct() = delete;

        /// @brief Constructor with callback that is to be invoked at destructor
        /// @param callback Callback must accept long long indicating the delta
        /// @return Creates the object
        InvokeOnDestruct(Fn&& callback) noexcept
            : callbackOnEnd {callback} { };

        // Invoke the callback and silently ignore the exceptions
        ~InvokeOnDestruct() noexcept
        {
            try
            {
                callbackOnEnd(ttx());
            }
            catch (...)
            {
            }
        };
    };


    /// @brief Create a string representation of the timepoint as RFC1123 spec
    /// @param tp Optional system_clock::timepoint; uses "now" if not provided
    /// @return String with your date/time as "Sun, 28 Jun 2020 23:29:00 GMT"
    template <class T = std::string>
    static T TimeAsRFC1123(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
    {
        // This cast is critical since the RFC1123 does not have a fractional portion!
        const auto rawtp = std::chrono::time_point_cast<std::chrono::seconds>(src.value_or(std::chrono::system_clock::now()));


        if constexpr (std::is_same_v<T, std::string>)
            return std::format("{0:%a, %d %h %Y %T GMT}", rawtp);
        else if constexpr (std::is_same_v<T, std::wstring>)
            return std::format(L"{0:%a, %d %h %Y %T GMT}", rawtp);
        else
            return T {};
    }

    /// @brief Creates a string representaiton of the date time in RFC3339 format with millisecond precision.
    /// @param tp Optional system_clock::timepoint; uses "now" if not provided
    /// @return String RFC3339 "2020-06-28T23:29:00.000Z"
    template <class T = std::string>
    static T TimeAsRFC3339(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
    {
        // This cast is critical since the RFC3339 only asks for milliseconds!
        const auto rawtp = std::chrono::time_point_cast<std::chrono::milliseconds>(src.value_or(std::chrono::system_clock::now()));

        if constexpr (std::is_same_v<T, std::string>)
            return std::format("{0:%Y-%m-%dT%H:%M:%S}Z", rawtp);
        else if constexpr (std::is_same_v<T, std::wstring>)
            return std::format(L"{0:%Y-%m-%dT%H:%M:%S}Z", rawtp);
        else
            return T {};
    }

    /// @brief Creates a string representaiton of the date time in ISO8601 format with millisecond precision. Alias for TimeAsRFC3339 method.
    /// @param tp Optional system_clock::timepoint; uses "now" if not provided
    /// @return String ISO8601 "2020-06-28T23:29:00.000Z"
    template <class T = std::string>
    static T TimeAsISO8601(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
    {
        // This cast is critical since the ISO8601 only asks for milliseconds!
        const auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(src.value_or(std::chrono::system_clock::now()));

        // NOTE: The resolution for %T includes siz-digits of microsecond detail!
        if constexpr (std::is_same_v<T, std::wstring>) { return std::format(L"{:%Y-%m-%dT%T}Z", tp); }

        return std::format("{:%Y-%m-%dT%T}Z", tp);
    }

#pragma endregion


    /// @brief Creates a pseudo random number generated UUID v4. It is best to use platform-specific method to ensure guid
    /// @return string 44 character of the format: 7792eaf4-456f-4d47-d93-863af0e0-a8b99b9b9988
    static std::string createCallId()
    {
        static std::random_device            rd;
        static std::mt19937_64               generator(rd());
        static std::uniform_int_distribution ud(0, 15);
        static std::uniform_int_distribution ud2(8, 11);

        std::stringstream sBuffer;

        sBuffer << std::hex;
        for (auto i = 0; i < 8; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 4; i++)
            sBuffer << ud(generator);
        sBuffer << "-4";
        for (auto i = 0; i < 3; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 3; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 8; i++)
            sBuffer << ud(generator);
        sBuffer << "-";
        for (auto i = 0; i < 12; i++)
            sBuffer << ud2(generator);
        return sBuffer.str();
    }


    // CAUTION; this is used as a reference to break out of the processing loop if the remaining buffer is less than the
    // size of this sample message.
    static std::string SIP_SAMPLE_MINIMAL_MESSAGE {
            "SIP/2.0 A B\r\nVia: SIP/2.0/TCP localhost\r\nCall-ID: A\r\nCSeq: 1 ACK\r\nFrom: sip:A\r\nTo: "
            "sip:A\r\nContact: A\r\nContent-Length: 0\r\n\r\n"};

    // Authorization Type
    static constexpr std::string_view AUTHORIZATION_CLEAR {"Clear"};
    static constexpr std::string_view AUTHORIZATION_BASIC {"Basic"};
    static constexpr std::string_view AUTHORIZATION_DIGEST {"Digest"};

    // Content-Type
    static constexpr std::string_view CONTENT_TYPE_TEXT_PLAIN {"text/plain"};
    static constexpr std::string_view CONTENT_TYPE_TEXT_HTML {"text/html"};
    static constexpr std::string_view CONTENT_TYPE_TEXT_XML {"text/xml"};
    static constexpr std::string_view CONTENT_TYPE_APP_SDP {"application/sdp"};
    static constexpr std::string_view CONTENT_TYPE_APP_XML {"application/xml"};
    static constexpr std::string_view CONTENT_TYPE_APP_PKCS7MIME {"application/pkcs7-mime"};
    static constexpr std::string_view CONTENT_TYPE_APP_XPRIVATE {"application/x-private"};
    static constexpr std::string_view CONTENT_TYPE_TEXT_X_METATEL1_PRESENCE {"text/x-metatel1.0-presence"};

    // Subscription State
    static constexpr std::string_view SUBSTATE_ACTIVE {"active"};
    static constexpr std::string_view SUBSTATE_PENDING {"pending"};
    static constexpr std::string_view SUBSTATE_TERMINATED {"terminated"};

    // TTL constants
    static constexpr int DEFAULT_SERVER_PORT {5060};
    static constexpr int DEFAULT_MAX_REGISTER_TTL {1 * 60 * 60};                        // 3600s
    static constexpr int DEFAULT_MAX_REGISTER_TTL_MS {DEFAULT_MAX_REGISTER_TTL * 1000}; // 1 hour in milliseconds
    static constexpr int DEFAULT_MIN_REGISTER_TTL {2 * 60};                             // 120s
    static constexpr int REGISTER_PERIOD_10MIN_SEC {10 * 60};                           // 600s = 10 minutes
    static constexpr int REGISTER_PERIOD_1MIN_SEC {60};                                 // 60s = 1 minute
    static constexpr int REGISTER_PERIOD_MIN_SEC {30};                                  // 30s
    static constexpr int REGISTER_PERIOD_10MIN_MS {REGISTER_PERIOD_10MIN_SEC * 1000};   // 600s = 10 minutes

    static constexpr std::string_view SIPVER_20 {"SIP/2.0"};

    static constexpr std::string_view METHOD_INVITE {"INVITE"};
    static constexpr std::string_view METHOD_ACK {"ACK"};
    static constexpr std::string_view METHOD_OPTIONS {"OPTIONS"};
    static constexpr std::string_view METHOD_BYE {"BYE"};
    static constexpr std::string_view METHOD_CANCEL {"CANCEL"};
    static constexpr std::string_view METHOD_REGISTER {"REGISTER"};
    static constexpr std::string_view METHOD_SUBSCRIBE {"SUBSCRIBE"};
    static constexpr std::string_view METHOD_NOTIFY {"NOTIFY"};
    static constexpr std::string_view METHOD_HEARTBEAT {"HEARTBEAT"};
    // Microsoft Extensions
    static constexpr std::string_view METHOD_MESSAGE {"MESSAGE"};
    static constexpr std::string_view METHOD_INFO {"INFO"};

    static constexpr std::string_view VIA_BRANCH_PREFIX {"z9hG4bK"};

    static constexpr std::string_view EMPTY_STD_STRING_VALUE {""};

    static constexpr std::string_view HF_FROM {"From"};
    static constexpr std::string_view HF_FROM_ALT {"f"};
    static constexpr std::string_view HF_TO {"To"};
    static constexpr std::string_view HF_TO_ALT {"t"};
    static constexpr std::string_view HF_PRIORTY {"Priority"};
    static constexpr std::string_view HF_CONTENT_ENCODING {"Content-Encoding"};
    static constexpr std::string_view HF_CONTENT_ENCODING_ALT {"e"};
    static constexpr std::string_view HF_CONTENT_LENGTH {"Content-Length"};
    static constexpr std::string_view HF_CONTENT_LENGTH_ALT {"L"};
    static constexpr std::string_view HF_CONTENT_TYPE {"Content-Type"};
    static constexpr std::string_view HF_CONTENT_TYPE2 {"Content-type"};
    static constexpr std::string_view HF_CONTENT_TYPE_ALT {"c"};
    static constexpr std::string_view HF_CALLID {"Call-ID"};
    static constexpr std::string_view HF_CALLID_ALT {"i"};
    static constexpr std::string_view HF_CSEQ {"CSeq"};
    static constexpr std::string_view HF_VIA {"Via"};
    static constexpr std::string_view HF_VIA_ALT {"v"};
    static constexpr std::string_view HF_ENCRYPTION {"Encryption"};
    static constexpr std::string_view HF_SUBJECT {"Subject"};
    static constexpr std::string_view HF_SUBJECT_ALT {"s"};
    static constexpr std::string_view HF_LOCATION {"Location"};
    static constexpr std::string_view HF_LOCATION_ALT {"Location"};
    static constexpr std::string_view HF_EXPIRES {"Expires"};
    static constexpr std::string_view HF_CONTACT {"Contact"};
    static constexpr std::string_view HF_CONTACT_ALT {"m"};
    static constexpr std::string_view HF_ACCEPT {"Accept"};
    static constexpr std::string_view HF_ACCEPT_ALT {"Accept"};
    static constexpr std::string_view HF_ACCEPT_ENCODING {"Accept-Encoding"};
    static constexpr std::string_view HF_ACCEPT_ENCODING_ALT {"Accept-Encoding"};
    static constexpr std::string_view HF_ACCEPT_LANGUAGE {"Accept-Language"};
    static constexpr std::string_view HF_ACCEPT_LANGUAGE_ALT {"Accept-Language"};
    static constexpr std::string_view HF_DATE {"Date"};
    static constexpr std::string_view HF_RECORD_ROUTE {"Record-Route"};
    static constexpr std::string_view HF_TIMESTAMP {"Timestamp"};
    static constexpr std::string_view HF_HIDE {"Hide"};
    static constexpr std::string_view HF_MAX_FORWARDS {"Max-Forwards"};
    static constexpr std::string_view HF_ORGANIZATION {"Organization"};
    static constexpr std::string_view HF_PROXY_AUTHORIZATION {"Proxy-Authorization"};
    static constexpr std::string_view HF_PROXY_REQUIRE {"Proxy-Require"};
    static constexpr std::string_view HF_ROUTE {"Route"};
    static constexpr std::string_view HF_REQUIRE {"Require"};
    static constexpr std::string_view HF_RESPONSE_KEY {"Response-Key"};
    static constexpr std::string_view HF_USER_AGENT {"User-Agent"};
    static constexpr std::string_view HF_PROXY_AUTHENTICATE {"Proxy-Authenticate"};
    static constexpr std::string_view HF_RETRY_AFTER {"Retry-After"};
    static constexpr std::string_view HF_SERVER {"Server"};
    static constexpr std::string_view HF_UNSUPPORTED {"Unsupported"};
    static constexpr std::string_view HF_WARNING {"Warning"};
    static constexpr std::string_view HF_WWW_AUTHENTICATE {"WWW-Authenticate"};
    static constexpr std::string_view HF_AUTHORIZATION {"Authorization"};

    // Subscribe/Notify header fields
    static constexpr std::string_view HF_SUBSCRIPTION_STATE {"Subscription-State"};

    // Parsing elements
    static constexpr std::string_view ELEM_SPACE {" "};
    static constexpr std::string_view ELEM_SEPERATOR {":"};
    static constexpr std::string_view ELEM_PADDEDSEPERATOR {": "};
    static constexpr std::string_view ELEM_TAGSEPERATOR {"{"};
    // Common elements over the wire (and WIN32)
    static constexpr std::string_view ELEM_NEWLINE {"\r\n"};
    static constexpr std::string_view ELEM_HEADERSECTIONDELIMITER {"\r\n\r\n"};
    static constexpr std::string_view ELEM_LWSP {"\r\n "};
    static constexpr std::string_view ELEM_LWSP1 {"\r\n\t"};
    static constexpr std::string_view ELEM_SDPBlockStart {" v=0\r\n"};
    // For UNIX systems
    static constexpr std::string_view ELEM_NEWLINE_LF {"\n"};
    static constexpr std::string_view ELEM_HEADERSECTIONDELIMITER_LF {"\n\n"};
    static constexpr std::string_view ELEM_LWSP_LF {"\n "};
    static constexpr std::string_view ELEM_LWSP1_LF {"\n\t"};
    static constexpr std::string_view ELEM_SDPBlockStart_LF {"v=0\n"};

    // Some common elements for building the SIP message
    static constexpr std::string_view SIP_ADDR_PREFIX {"sip:\\s"};

    // Helpers to parse the SIP buffer (CTRE compile-time regular expressions)
    static constexpr auto SIP_PATTERN_STARTLINE =
            ctll::fixed_string {"(MESSAGE|INFO|INVITE|ACK|OPTIONS|BYE|CANCEL|REGISTER|SUBSCRIBE|NOTIFY|SIP/2\\.0)\\s([^\\s]+)\\s([^\\n\\f\\r]*)[\r\n]*"};
    static constexpr auto SIP_PATTERN_BODY_RE =
            ctll::fixed_string {"([vosiuepcbtzkma])=([^\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_ALINE_RE =
            ctll::fixed_string {"^([^:\r\n]*):(.*)$"};
    static constexpr auto SIP_PATTERN_BODY_ILINE_RE =
            ctll::fixed_string {"^(.+) \\(([^\\)]*)\\) ([^\\s\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_CLINE_RE =
            ctll::fixed_string {"^(.+) (.+) ([^\\s\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_OLINE_RE =
            ctll::fixed_string {"([^\\s]+) (\\d+) (\\d+) (\\w+) (\\w+) ([^\\s]+)"};
} // namespace siddiqsoft