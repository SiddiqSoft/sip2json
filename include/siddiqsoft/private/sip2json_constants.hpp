/*
    A SIP Parser for Modern C++: Protocol and Framing Constants
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

#ifndef SIP2JSON_CONSTANTS_HPP
#define SIP2JSON_CONSTANTS_HPP

#include <string>
#include <string_view>

#include "ctre.hpp"

namespace siddiqsoft
{
    // CAUTION; this is used as a reference to break out of the processing loop if the remaining buffer is less than the
    // size of this sample message.
    static std::string SIP_SAMPLE_MINIMAL_MESSAGE {
            "SIP/2.0 A B\r\nVia: SIP/2.0/TCP localhost\r\nCall-ID: A\r\nCSeq: 1 ACK\r\nFrom: sip:A\r\nTo: "
            "sip:A\r\nContact: A\r\nContent-Length: 0\r\n\r\n"};

    // Authorization Type
    static const std::string AUTHORIZATION_CLEAR {"Clear"};
    static const std::string AUTHORIZATION_BASIC {"Basic"};
    static const std::string AUTHORIZATION_DIGEST {"Digest"};

    // Content-Type
    static const std::string CONTENT_TYPE_TEXT_PLAIN {"text/plain"};
    static const std::string CONTENT_TYPE_TEXT_HTML {"text/html"};
    static const std::string CONTENT_TYPE_TEXT_XML {"text/xml"};
    static const std::string CONTENT_TYPE_APP_SDP {"application/sdp"};
    static const std::string CONTENT_TYPE_APP_XML {"application/xml"};
    static const std::string CONTENT_TYPE_APP_PKCS7MIME {"application/pkcs7-mime"};
    static const std::string CONTENT_TYPE_APP_XPRIVATE {"application/x-private"};
    static const std::string CONTENT_TYPE_TEXT_X_METATEL1_PRESENCE {"text/x-metatel1.0-presence"};

    // Subscription State
    static const std::string SUBSTATE_ACTIVE {"active"};
    static const std::string SUBSTATE_PENDING {"pending"};
    static const std::string SUBSTATE_TERMINATED {"terminated"};

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

    // Parsing elements
    static const std::string ELEM_SPACE {" "};
    static const std::string ELEM_SEPARATOR {":"};
    static const std::string ELEM_PADDED_SEPARATOR {": "};
    static const std::string ELEM_TAG_SEPARATOR {"{"};
    // Common elements over the wire (and WIN32)
    static const std::string ELEM_NEWLINE {"\r\n"};
    static const std::string ELEM_HEADERSECTIONDELIMITER {"\r\n\r\n"};
    static const std::string ELEM_LWSP {"\r\n "};
    static const std::string ELEM_LWSP1 {"\r\n\t"};
    static const std::string ELEM_SDPBlockStart {" v=0\r\n"};
    // For UNIX systems
    static const std::string ELEM_NEWLINE_LF {"\n"};
    static const std::string ELEM_HEADERSECTIONDELIMITER_LF {"\n\n"};
    static const std::string ELEM_LWSP_LF {"\n "};
    static const std::string ELEM_LWSP1_LF {"\n\t"};
    static const std::string ELEM_SDPBlockStart_LF {"v=0\n"};

    // Some common elements for building the SIP message
    static const std::string SIP_ADDR_PREFIX {"sip:\\s"};

    // Helpers to parse the SIP buffer (CTRE compile-time regular expressions)
    // Disallow any greedy consumption of the ending as it silently causes exceptions and slows down parsing!
    // This regex expression supports CRLF and LF
    static constexpr auto SIP_PATTERN_STARTLINE =
            ctll::fixed_string {"(MESSAGE|INFO|INVITE|ACK|OPTIONS|BYE|CANCEL|REGISTER|SUBSCRIBE|NOTIFY|SIP/"
                                "2\\.0)\\s([^\\s]+)\\s([^\\n\\f\\r]*)[\r\n|\n]"};
    static constexpr auto SIP_PATTERN_BODY_RE       = ctll::fixed_string {"([vosiuepcbtzkma])=([^\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_ALINE_RE = ctll::fixed_string {"^([^:\r\n]*):(.*)$"};
    static constexpr auto SIP_PATTERN_BODY_ILINE_RE = ctll::fixed_string {"^(.+) \\(([^\\)]*)\\) ([^\\s\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_CLINE_RE = ctll::fixed_string {"^(.+) (.+) ([^\\s\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_OLINE_RE = ctll::fixed_string {"([^\\s]+) (\\d+) (\\d+) (\\w+) (\\w+) ([^\\s]+)"};
} // namespace siddiqsoft

#endif
