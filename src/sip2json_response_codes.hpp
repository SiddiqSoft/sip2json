/*
	A SIP Parser for Modern C++: Error Code Definitions
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

#include <algorithm>
#include <string>
#include <regex>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>

#include <format>
#include "nlohmann/json.hpp"

namespace siddiqsoft
{
#pragma region SIP Response Codes
    // Source: https://en.wikipedia.org/wiki/List_of_SIP_response_codes
    static const inline std::map<uint32_t, std::string> SIPResponseCodes {{0, "NotSet"},
                                                                          // 1xx—Provisional Responses
                                                                          {100, "Trying"},
                                                                          {180, "Ringing"},
                                                                          {181, "Call is Being Forwarded"},
                                                                          {182, "Queued"},
                                                                          {183, "Session Progress"},
                                                                          {199, "Early Dialog Terminated"},
                                                                          // 2xx—Successful Responses
                                                                          {200, "OK"},
                                                                          {202, "Accepted"},
                                                                          {204, "No Notification"},
                                                                          // 3xx—Redirection Responses
                                                                          {300, "Multiple Choices"},
                                                                          {301, "Moved Permanently"},
                                                                          {302, "Moved Temporarily"},
                                                                          {305, "Use Proxy"},
                                                                          {380, "Alternative Service"},
                                                                          // 4xx—Client Failure Responses
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
                                                                          // 5xx—Server Failure Responses
                                                                          {500, "Internal Server Error"},
                                                                          {501, "Not Implemented"},
                                                                          {502, "Bad Gateway"},
                                                                          {503, "Service Unavailable"},
                                                                          {504, "Server Time-out"},
                                                                          {505, "Version Not Supported"},
                                                                          {513, "Message Too Large"},
                                                                          {555, "Push Notification Service Not Supported"},
                                                                          {580, "Precondition Failure"},
                                                                          // 6xx—Global Failure Responses
                                                                          {603, "Decline"},
                                                                          {604, "Does Not Exist Anywhere"},
                                                                          {606, "Not Acceptable"},
                                                                          {607, "Unwanted"},
                                                                          {608, "Rejected"}};
    static const std::string& getReasonPhrase(uint32_t statusCode) { return SIPResponseCodes.at(statusCode); }
#pragma endregion
} // namespace siddiqsoft