/*
    A SIP Parser for Modern C++: Protocol and Framing Constants
    Version 3
    https://github.com/siddiqsoft/sip2json/

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

namespace siddiqsoft {
    // Top-Level Message JSON Section Keys
    inline const std::string JSON_KEY_STARTLINE {"s"};
    inline const std::string JSON_KEY_HEADERS {"h"};
    inline const std::string JSON_KEY_BODY {"b"};
    inline const std::string JSON_KEY_META {"meta"};

    // Start-Line JSON Field Keys
    inline const std::string JSON_KEY_TYPE {"type"};
    inline const std::string JSON_KEY_METHOD {"method"};
    inline const std::string JSON_KEY_URI {"uri"};
    inline const std::string JSON_KEY_VERSION {"version"};
    inline const std::string JSON_KEY_STATUS {"status"};
    inline const std::string JSON_KEY_REASON {"reason"};

    // Meta JSON Field Keys
    inline const std::string JSON_KEY_ID {"id"};
    inline const std::string JSON_KEY_TIME {"time"};
    inline const std::string JSON_KEY_TTX {"ttx"};

    // SDP JSON Key
    inline const std::string JSON_KEY_SDP {"sdp"};

    // URI Schemes
    inline const std::string URI_SCHEME_SIP {"sip:"};
    inline const std::string URI_SCHEME_SIPS {"sips:"};

    // CAUTION; this is used as a reference to break out of the processing loop if the remaining buffer is less than the
    // size of this sample message.
    inline const std::string SIP_SAMPLE_MINIMAL_MESSAGE {
            "SIP/2.0 A B\r\nVia: SIP/2.0/TCP localhost\r\nCall-ID: A\r\nCSeq: 1 ACK\r\nFrom: sip:A\r\nTo: "
            "sip:A\r\nContact: A\r\nContent-Length: 0\r\n\r\n"};

    // Authorization Type
    inline const std::string AUTHORIZATION_CLEAR {"Clear"};
    inline const std::string AUTHORIZATION_BASIC {"Basic"};
    inline const std::string AUTHORIZATION_DIGEST {"Digest"};

    // Content-Type
    inline const std::string CONTENT_TYPE_TEXT_PLAIN {"text/plain"};
    inline const std::string CONTENT_TYPE_TEXT_HTML {"text/html"};
    inline const std::string CONTENT_TYPE_TEXT_XML {"text/xml"};
    inline const std::string CONTENT_TYPE_APP_SDP {"application/sdp"};
    inline const std::string CONTENT_TYPE_APP_XML {"application/xml"};
    inline const std::string CONTENT_TYPE_APP_PKCS7MIME {"application/pkcs7-mime"};
    inline const std::string CONTENT_TYPE_APP_XPRIVATE {"application/x-private"};
    inline const std::string CONTENT_TYPE_TEXT_X_METATEL1_PRESENCE {"text/x-metatel1.0-presence"};

    // Subscription State
    inline const std::string SUBSTATE_ACTIVE {"active"};
    inline const std::string SUBSTATE_PENDING {"pending"};
    inline const std::string SUBSTATE_TERMINATED {"terminated"};

    // TTL constants
    inline constexpr int DEFAULT_SERVER_PORT {5060};
    inline constexpr int DEFAULT_MAX_REGISTER_TTL {1 * 60 * 60};                        // 3600s
    inline constexpr int DEFAULT_MAX_REGISTER_TTL_MS {DEFAULT_MAX_REGISTER_TTL * 1000}; // 1 hour in milliseconds
    inline constexpr int DEFAULT_MIN_REGISTER_TTL {2 * 60};                             // 120s
    inline constexpr int REGISTER_PERIOD_10MIN_SEC {10 * 60};                           // 600s = 10 minutes
    inline constexpr int REGISTER_PERIOD_1MIN_SEC {60};                                 // 60s = 1 minute
    inline constexpr int REGISTER_PERIOD_MIN_SEC {30};                                  // 30s
    inline constexpr int REGISTER_PERIOD_10MIN_MS {REGISTER_PERIOD_10MIN_SEC * 1000};   // 600s = 10 minutes

    inline const std::string SIPVER_20 {"SIP/2.0"};

    inline const std::string METHOD_INVITE {"INVITE"};
    inline const std::string METHOD_ACK {"ACK"};
    inline const std::string METHOD_OPTIONS {"OPTIONS"};
    inline const std::string METHOD_BYE {"BYE"};
    inline const std::string METHOD_CANCEL {"CANCEL"};
    inline const std::string METHOD_REGISTER {"REGISTER"};
    inline const std::string METHOD_SUBSCRIBE {"SUBSCRIBE"};
    inline const std::string METHOD_NOTIFY {"NOTIFY"};
    inline const std::string METHOD_MESSAGE {"MESSAGE"};
    inline const std::string METHOD_INFO {"INFO"};
    inline const std::string METHOD_REFER {"REFER"};
    inline const std::string METHOD_PUBLISH {"PUBLISH"};
    inline const std::string METHOD_UPDATE {"UPDATE"};
    inline const std::string METHOD_PRACK {"PRACK"};

    inline const std::string VIA_BRANCH_PREFIX {"z9hG4bK"};

    inline const std::string EMPTY_STD_STRING_VALUE {""};

    // Parsing elements
    inline const std::string ELEM_SPACE {" "};
    inline const std::string ELEM_SEPARATOR {":"};
    inline const std::string ELEM_PADDED_SEPARATOR {": "};
    inline const std::string ELEM_TAG_SEPARATOR {"{"};
    // Common elements over the wire (and WIN32)
    inline const std::string ELEM_NEWLINE {"\r\n"};
    inline const std::string ELEM_HEADERSECTIONDELIMITER {"\r\n\r\n"};
    inline const std::string ELEM_LWSP {"\r\n "};
    inline const std::string ELEM_LWSP1 {"\r\n\t"};
    inline const std::string ELEM_SDPBlockStart {" v=0\r\n"};
    // For UNIX systems
    inline const std::string ELEM_NEWLINE_LF {"\n"};
    inline const std::string ELEM_HEADERSECTIONDELIMITER_LF {"\n\n"};
    inline const std::string ELEM_LWSP_LF {"\n "};
    inline const std::string ELEM_LWSP1_LF {"\n\t"};
    inline const std::string ELEM_SDPBlockStart_LF {"v=0\n"};

    // Some common elements for building the SIP message
    inline const std::string SIP_ADDR_PREFIX {"sip:\\s"};

    inline constexpr std::string_view SIP_VALID_METHODS[] = {"INVITE",
                                                             "ACK",
                                                             "OPTIONS",
                                                             "BYE",
                                                             "CANCEL",
                                                             "REGISTER",
                                                             "SUBSCRIBE",
                                                             "NOTIFY",
                                                             "REFER",
                                                             "PUBLISH",
                                                             "UPDATE",
                                                             "PRACK",
                                                             "INFO",
                                                             "MESSAGE"};

} // namespace siddiqsoft

#endif
