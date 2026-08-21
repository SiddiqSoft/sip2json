/*
    A SIP Parser for Modern C++: Protocol and Framing Constants
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

#ifndef SIP2JSON_CONSTANTS_HPP
#define SIP2JSON_CONSTANTS_HPP

#include <string>
#include <string_view>

#include "ctre.hpp"

namespace siddiqsoft
{
    // CAUTION; this is used as a reference to break out of the processing loop if the remaining buffer is less than the
    // size of this sample message.
    static constexpr std::string_view SIP_SAMPLE_MINIMAL_MESSAGE {
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
    static constexpr std::string_view METHOD_MESSAGE {"MESSAGE"};
    static constexpr std::string_view METHOD_INFO {"INFO"};
    static constexpr std::string_view METHOD_REFER {"REFER"};
    static constexpr std::string_view METHOD_PUBLISH {"PUBLISH"};
    static constexpr std::string_view METHOD_UPDATE {"UPDATE"};
    static constexpr std::string_view METHOD_PRACK {"PRACK"};

    static constexpr std::string_view VIA_BRANCH_PREFIX {"z9hG4bK"};

    static constexpr std::string_view EMPTY_STD_STRING_VALUE {""};

    // Parsing elements
    static constexpr std::string_view ELEM_SPACE {" "};
    static constexpr std::string_view ELEM_SEPARATOR {":"};
    static constexpr std::string_view ELEM_PADDED_SEPARATOR {": "};
    static constexpr std::string_view ELEM_TAG_SEPARATOR {"{"};
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
    // Disallow any greedy consumption of the ending as it silently causes exceptions and slows down parsing!
    // This regex expression supports CRLF and LF
    static constexpr auto SIP_PATTERN_STARTLINE =
            ctll::fixed_string {"(MESSAGE|INFO|INVITE|ACK|OPTIONS|BYE|CANCEL|REGISTER|SUBSCRIBE|NOTIFY|REFER|PUBLISH|UPDATE|PRACK|SIP/"
                                "2\\.0)\\s([^\\s]+)\\s([^\\n\\f\\r]*)[\r\n|\n]"};
    static constexpr auto SIP_PATTERN_BODY_RE       = ctll::fixed_string {"([vosiuepcbtzkma])=([^\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_ALINE_RE = ctll::fixed_string {"^([^:\r\n]*):(.*)$"};
    static constexpr auto SIP_PATTERN_BODY_ILINE_RE = ctll::fixed_string {"^(.+) \\(([^\\)]*)\\) ([^\\s\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_CLINE_RE = ctll::fixed_string {"^(.+) (.+) ([^\\s\r\n]*)"};
    static constexpr auto SIP_PATTERN_BODY_OLINE_RE = ctll::fixed_string {"([^\\s]+) (\\d+) (\\d+) (\\w+) (\\w+) ([^\\s]+)"};
} // namespace siddiqsoft

#endif
