/*
    A SIP Parser for Modern C++: Error Code Definitions
    Version 3
    https://github.com/siddiqsoft/sip2json/

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

#include <cstdint>
#include <string_view>

namespace siddiqsoft {
#pragma region SIP Response Codes
    inline constexpr uint32_t INVALID_SIP_RESPONSE_CODE = 999;

    /// @brief Retrieves the reason phrase for a given SIP status code.
    /// @details Includes all standard SIP response codes defined in RFC 3261 and related RFCs.
    /// Sources: RFC 3261, RFC 3265, RFC 3311, RFC 3326, RFC 3455, RFC 3608, RFC 4028, RFC 4320,
    /// RFC 5057, RFC 5360, RFC 5366, RFC 5373, RFC 6050
    /// https://en.wikipedia.org/wiki/List_of_SIP_response_codes
    /// @param statusCode The SIP status code to look up.
    /// @return A std::string_view representing the reason phrase, or empty string_view if unknown.
    constexpr std::string_view getReasonPhrase(uint32_t statusCode) noexcept
    {
        switch (statusCode) {
        case 0: return "NotSet";
        // 1xx - Provisional Responses
        case 100: return "Trying";
        case 180: return "Ringing";
        case 181: return "Call is Being Forwarded";
        case 182: return "Queued";
        case 183: return "Session Progress";
        case 199: return "Early Dialog Terminated";
        // 2xx - Successful Responses
        case 200: return "OK";
        case 202: return "Accepted";
        case 204: return "No Notification";
        // 3xx - Redirection Responses
        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Moved Temporarily";
        case 305: return "Use Proxy";
        case 380: return "Alternative Service";
        // 4xx - Client Failure Responses
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Conditional Request Failed";
        case 413: return "Request Entity Too Large";
        case 414: return "Request-URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Unsupported URI Scheme";
        case 417: return "Unknown Resource-Priority";
        case 420: return "Bad Extension";
        case 421: return "Extension Required";
        case 422: return "Session Interval Too Small";
        case 423: return "Interval Too Brief";
        case 424: return "Bad Location Information";
        case 428: return "Use Identity Header";
        case 429: return "Provide Referrer Identity";
        case 430: return "Flow Failed";
        case 433: return "Anonymity Disallowed";
        case 436: return "Bad Identity-Info";
        case 437: return "Unsupported Certificate";
        case 438: return "Invalid Identity Header";
        case 439: return "First Hop Lacks Outbound Support";
        case 440: return "Max-Breadth Exceeded";
        case 469: return "Bad Info Package";
        case 470: return "Consent Needed";
        case 480: return "Temporarily Unavailable";
        case 481: return "Call/Transaction Does Not Exist";
        case 482: return "Loop Detected";
        case 483: return "Too Many Hops";
        case 484: return "Address Incomplete";
        case 485: return "Ambiguous";
        case 486: return "Busy Here";
        case 487: return "Request Terminated";
        case 488: return "Not Acceptable Here";
        case 489: return "Bad Event";
        case 491: return "Request Pending";
        case 493: return "Undecipherable";
        case 494: return "Security Agreement Required";
        case 495: return "Invalid Message Digest";
        case 496: return "Invalid Authorization Scheme";
        case 497: return "Key Expired";
        case 498: return "Signature Mismatch";
        case 499: return "Authentication Timeout";
        // 5xx - Server Failure Responses
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Server Time-out";
        case 505: return "Version Not Supported";
        case 506: return "Message Too Large";
        case 513: return "Message Too Large";
        case 555: return "Push Notification Service Not Supported";
        case 580: return "Precondition Failure";
        // 6xx - Global Failure Responses
        case 600: return "Busy Everywhere";
        case 603: return "Decline";
        case 604: return "Does Not Exist Anywhere";
        case 606: return "Not Acceptable";
        case 607: return "Unwanted";
        case 608: return "Rejected";
        case 609: return "Feature Not Implemented";
        // 999 or unknown / not found
        default: return "";
        }
    }
#pragma endregion
} // namespace siddiqsoft
