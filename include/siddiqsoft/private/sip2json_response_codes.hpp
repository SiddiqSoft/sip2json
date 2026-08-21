/*
    A SIP Parser for Modern C++: Error Code Definitions
    Version 2.5.x
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

#include <algorithm>
#include <cstdint>
#include <string>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>

#include <format>
#include <string_view>
#include "nlohmann/json.hpp"

namespace siddiqsoft
{
#pragma region SIP Response Codes
    static const uint32_t INVALID_SIP_RESPONSE_CODE = 999;

    /// @brief Comprehensive map of SIP response codes and their reason phrases.
    /// @details Includes all standard SIP response codes defined in RFC 3261 and related RFCs.
    /// Sources: RFC 3261, RFC 3265, RFC 3311, RFC 3326, RFC 3455, RFC 3608, RFC 4028, RFC 4320,
    /// RFC 5057, RFC 5360, RFC 5366, RFC 5373, RFC 6050
    /// https://en.wikipedia.org/wiki/List_of_SIP_response_codes
    static const inline std::map<uint32_t, std::string> SIPResponseCodes {{0, "NotSet"},
                                                                          // 1xx - Provisional Responses
                                                                          {100, "Trying"},
                                                                          {180, "Ringing"},
                                                                          {181, "Call is Being Forwarded"},
                                                                          {182, "Queued"},
                                                                          {183, "Session Progress"},
                                                                          {199, "Early Dialog Terminated"},
                                                                          // 2xx - Successful Responses
                                                                          {200, "OK"},
                                                                          {202, "Accepted"},
                                                                          {204, "No Notification"},
                                                                          // 3xx - Redirection Responses
                                                                          {300, "Multiple Choices"},
                                                                          {301, "Moved Permanently"},
                                                                          {302, "Moved Temporarily"},
                                                                          {305, "Use Proxy"},
                                                                          {380, "Alternative Service"},
                                                                          // 4xx - Client Failure Responses
                                                                          {400, "Bad Request"},
                                                                          {401, "Unauthorized"},
                                                                          {402, "Payment Required"},
                                                                          {403, "Forbidden"},
                                                                          {404, "Not Found"},
                                                                          {405, "Method Not Allowed"},
                                                                          {406, "Not Acceptable"},
                                                                          {407, "Proxy Authentication Required"},
                                                                          {408, "Request Timeout"},
                                                                          {409, "Conflict"},
                                                                          {410, "Gone"},
                                                                          {411, "Length Required"},
                                                                          {412, "Conditional Request Failed"},
                                                                          {413, "Request Entity Too Large"},
                                                                          {414, "Request-URI Too Long"},
                                                                          {415, "Unsupported Media Type"},
                                                                          {416, "Unsupported URI Scheme"},
                                                                          {417, "Unknown Resource-Priority"},
                                                                          {420, "Bad Extension"},
                                                                          {421, "Extension Required"},
                                                                          {422, "Session Interval Too Small"},
                                                                          {423, "Interval Too Brief"},
                                                                          {424, "Bad Location Information"},
                                                                          {428, "Use Identity Header"},
                                                                          {429, "Provide Referrer Identity"},
                                                                          {430, "Flow Failed"},
                                                                          {433, "Anonymity Disallowed"},
                                                                          {436, "Bad Identity-Info"},
                                                                          {437, "Unsupported Certificate"},
                                                                          {438, "Invalid Identity Header"},
                                                                          {439, "First Hop Lacks Outbound Support"},
                                                                          {440, "Max-Breadth Exceeded"},
                                                                          {469, "Bad Info Package"},
                                                                          {470, "Consent Needed"},
                                                                          {480, "Temporarily Unavailable"},
                                                                          {481, "Call/Transaction Does Not Exist"},
                                                                          {482, "Loop Detected"},
                                                                          {483, "Too Many Hops"},
                                                                          {484, "Address Incomplete"},
                                                                          {485, "Ambiguous"},
                                                                          {486, "Busy Here"},
                                                                          {487, "Request Terminated"},
                                                                          {488, "Not Acceptable Here"},
                                                                          {489, "Bad Event"},
                                                                          {491, "Request Pending"},
                                                                          {493, "Undecipherable"},
                                                                          {494, "Security Agreement Required"},
                                                                          {495, "Invalid Message Digest"},
                                                                          {496, "Invalid Authorization Scheme"},
                                                                          {497, "Key Expired"},
                                                                          {498, "Signature Mismatch"},
                                                                          {499, "Authentication Timeout"},
                                                                          // 5xx - Server Failure Responses
                                                                          {500, "Internal Server Error"},
                                                                          {501, "Not Implemented"},
                                                                          {502, "Bad Gateway"},
                                                                          {503, "Service Unavailable"},
                                                                          {504, "Server Time-out"},
                                                                          {505, "Version Not Supported"},
                                                                          {506, "Message Too Large"},
                                                                          {513, "Message Too Large"},
                                                                          {555, "Push Notification Service Not Supported"},
                                                                          {580, "Precondition Failure"},
                                                                          // 6xx - Global Failure Responses
                                                                          {600, "Busy Everywhere"},
                                                                          {603, "Decline"},
                                                                          {604, "Does Not Exist Anywhere"},
                                                                          {606, "Not Acceptable"},
                                                                          {607, "Unwanted"},
                                                                          {608, "Rejected"},
                                                                          {609, "Feature Not Implemented"},
                                                                          // 999 - Internal invalid; return empty string
                                                                          {INVALID_SIP_RESPONSE_CODE, ""}
                                                                        };

    /// @brief Retrieves the reason phrase for a given SIP status code.
    /// @param statusCode The SIP status code to look up.
    /// @return A const reference to the reason phrase string.
    /// @throws std::out_of_range if the status code is not found in the map.
    static const std::string& getReasonPhrase(uint32_t statusCode)
    {
        if (SIPResponseCodes.contains(statusCode)) { return SIPResponseCodes.at(statusCode); }

        // Drop-through.. we did not find the status code.
        // This will return an empty string to allow the downstream clients to
        // easily detect invalid/unavailable codes without performing a string compare.
        return SIPResponseCodes.at(INVALID_SIP_RESPONSE_CODE);
    }
#pragma endregion
} // namespace siddiqsoft
