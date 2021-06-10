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

#include <regex>
#include <string>
#include <chrono>
#include <random>
#include <sstream>
#include <optional>
#include <format>

#include "nlohmann/json.hpp"

namespace siddiqsoft
{
#pragma region Datetime helpers
	/// @brief Helper struct which runs your lambda when this object goes out of scope. Used to time expression scope.
	/// @tparam Fn Lambda of type void(long long delta) called upon destructor; must not throw.
	template <typename Fn> struct InvokeOnDestruct
	{
		Fn											callbackOnEnd;
		const std::chrono::system_clock::time_point ttxStart {std::chrono::system_clock::now()};

		/// @brief Gets the time delta between start/instantiation of this object and now
		/// @return long long type delta
		auto ttx() noexcept
		{
			const auto ttxNow = std::chrono::system_clock::now();
			return std::chrono::duration_cast<std::chrono::milliseconds>(ttxNow - ttxStart).count();
		};

		InvokeOnDestruct() = delete;

		/// @brief Constructor with callback that is to be invoked at destructor
		/// @param callback Callback must accept long long indicating the delta
		/// @return Creates the object
		InvokeOnDestruct(Fn&& callback) noexcept
			: callbackOnEnd {callback} {};

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
		const auto rawtp   = src.value_or(std::chrono::system_clock::now());
		auto	   rawtime = std::chrono::system_clock::to_time_t(rawtp);
		tm		   timeInfo {};

		// Get the UTC time packet.
		auto ec = gmtime_s(&timeInfo, &rawtime);

		if constexpr (std::is_same_v<T, std::string>)
		{
			// HTTP-date as per RFC 7231:  Tue, 01 Nov 1994 08:12:31 GMT
			// Note that since we are getting the UTC time we should not use the %z or %Z in the strftime format
			// as it returns the local timezone and not GMT.
			char buff[sizeof "Tue, 01 Nov 1994 08:12:31 GMT"] {};
			if (ec != EINVAL) strftime(buff, sizeof(buff), "%a, %d %h %Y %T GMT", &timeInfo);

			return buff;
		}
		else if constexpr (std::is_same_v<T, std::wstring>)
		{
			// HTTP-date as per RFC 7231:  Tue, 01 Nov 1994 08:12:31 GMT
			// Note that since we are getting the UTC time we should not use the %z or %Z in the strftime format
			// as it returns the local timezone and not GMT.
			wchar_t buff[sizeof L"Tue, 01 Nov 1994 08:12:31 GMT"] {};
			if (ec != EINVAL) wcsftime(buff, sizeof(buff), L"%a, %d %h %Y %T GMT", &timeInfo);
			return buff;
		}

		return T {};
	}

	/// @brief Creates a string representaiton of the date time in RFC3339 format with millisecond precision.
	/// @param tp Optional system_clock::timepoint; uses "now" if not provided
	/// @return String RFC3339 "2020-06-28T23:29:00.000Z"
	template <class T = std::string>
	static T TimeAsRFC3339(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
	{
		const auto rawtp   = src.value_or(std::chrono::system_clock::now());
		auto	   rawtime = std::chrono::system_clock::to_time_t(rawtp);
		tm		   timeInfo {};
		// We need to get the fractional milliseconds from the raw time point.
		auto msTime = std::chrono::duration_cast<std::chrono::milliseconds>(rawtp.time_since_epoch()).count() % 1000;
		// Get the UTC time packet.
		auto ec = gmtime_s(&timeInfo, &rawtime);

		if constexpr (std::is_same_v<T, std::string>)
		{
			// https://en.wikipedia.org/wiki/ISO_8601
			// yyyy-mm-ddThh:mm:ss.mmmZ
			char buff[sizeof "yyyy-mm-ddThh:mm:ss.0000000Z"] {};

			if (ec != EINVAL) strftime(buff, sizeof(buff), "%FT%T", &timeInfo);
			return std::format("{}.{:03}Z", buff, msTime);
		}
		else if constexpr (std::is_same_v<T, std::wstring>)
		{
			// https://en.wikipedia.org/wiki/ISO_8601
			// yyyy-mm-ddThh:mm:ss.mmmZ
			wchar_t buff[sizeof L"yyyy-mm-ddThh:mm:ss.0000000Z"] {};
			if (ec != EINVAL) wcsftime(buff, sizeof(buff), L"%FT%T", &timeInfo);
			return std::format(L"{}.{:03}Z", buff, msTime);
		}

		return T {};
	}

	/// @brief Creates a string representaiton of the date time in ISO8601 format with millisecond precision. Alias for TimeAsRFC3339 method.
	/// @param tp Optional system_clock::timepoint; uses "now" if not provided
	/// @return String ISO8601 "2020-06-28T23:29:00.000Z"
	template <class T = std::string>
	static T TimeAsISO8601(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
	{
		const auto tp = src.value_or(std::chrono::system_clock::now());
		
		// NOTE: The resolution for %T includes siz-digits of microsecond detail!
		if constexpr (std::is_same_v<T, std::wstring>) { return std::format(L"{:%Y-%m-%dT%T}Z", tp); }

		return std::format("{:%Y-%m-%dT%T}Z", tp);
	}

#pragma endregion


	/// @brief Creates a pseudo random number generated UUID v4. It is best to use platform-specific method to ensure guid
	/// @return string 44 character of the format: 7792eaf4-456f-4d47-d93-863af0e0-a8b99b9b9988
	static std::string createCallId()
	{
		static std::random_device			 rd;
		static std::mt19937_64				 generator(rd());
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
	static const int DEFAULT_SERVER_PORT {5060};
	static const int DEFAULT_MAX_REGISTER_TTL {1 * 60 * 60};						// 3600s
	static const int DEFAULT_MAX_REGISTER_TTL_MS {DEFAULT_MAX_REGISTER_TTL * 1000}; //	1 hour in milliseconds
	static const int DEFAULT_MIN_REGISTER_TTL {2 * 60};								// 120s
	static const int REGISTER_PERIOD_10MIN_SEC {10 * 60};							// 600s = 10 minutes.
	static const int REGISTER_PERIOD_1MIN_SEC {60};									// 60s = 1 minutes.
	static const int REGISTER_PERIOD_MIN_SEC {30};									// 30s
	static const int REGISTER_PERIOD_10MIN_MS {REGISTER_PERIOD_10MIN_SEC * 1000};	// 600s = 10 minutes.

	static const std::string SIPVER_20 {"SIP/2.0"};

	static const std::string METHOD_INVITE {"INVITE"};
	static const std::string METHOD_ACK {"ACK"};
	static const std::string METHOD_OPTIONS {"OPTIONS"};
	static const std::string METHOD_BYE {"BYE"};
	static const std::string METHOD_CANCEL {"CANCEL"};
	static const std::string METHOD_REGISTER {"REGISTER"};
	static const std::string METHOD_SUBSCRIBE {"SUBSCRIBE"};
	static const std::string METHOD_NOTIFY {"NOTIFY"};
	static const std::string METHOD_HEARTBEAT {"HEARTBEAT"};
	// Microsoft Extensions
	static const std::string METHOD_MESSAGE {"MESSAGE"};
	static const std::string METHOD_INFO {"INFO"};

	static const std::string VIA_BRANCH_PREFIX {"z9hG4bK"};

	static const std::string EMPTY_STD_STRING_VALUE {""};

	static const std::string HF_FROM {"From"};
	static const std::string HF_FROM_ALT {"f"};
	static const std::string HF_TO {"To"};
	static const std::string HF_TO_ALT {"t"};
	static const std::string HF_PRIORTY {"Priority"};
	static const std::string HF_CONTENT_ENCODING {"Content-Encoding"};
	static const std::string HF_CONTENT_ENCODING_ALT {"e"};
	static const std::string HF_CONTENT_LENGTH {"Content-Length"};
	static const std::string HF_CONTENT_LENGTH_ALT {"L"};
	static const std::string HF_CONTENT_TYPE {"Content-Type"};
	static const std::string HF_CONTENT_TYPE2 {"Content-type"};
	static const std::string HF_CONTENT_TYPE_ALT {"c"};
	static const std::string HF_CALLID {"Call-ID"};
	static const std::string HF_CALLID_ALT {"i"};
	static const std::string HF_CSEQ {"CSeq"};
	static const std::string HF_VIA {"Via"};
	static const std::string HF_VIA_ALT {"v"};
	static const std::string HF_ENCRYPTION {"Encryption"};
	static const std::string HF_SUBJECT {"Subject"};
	static const std::string HF_SUBJECT_ALT {"s"};
	static const std::string HF_LOCATION {"Location"};
	static const std::string HF_LOCATION_ALT {"Location"};
	static const std::string HF_EXPIRES {"Expires"};
	static const std::string HF_CONTACT {"Contact"};
	static const std::string HF_CONTACT_ALT {"m"};
	static const std::string HF_ACCEPT {"Accept"};
	static const std::string HF_ACCEPT_ALT {"Accept"};
	static const std::string HF_ACCEPT_ENCODING {"Accept-Encoding"};
	static const std::string HF_ACCEPT_ENCODING_ALT {"Accept-Encoding"};
	static const std::string HF_ACCEPT_LANGUAGE {"Accept-Language"};
	static const std::string HF_ACCEPT_LANGUAGE_ALT {"Accept-Language"};
	static const std::string HF_DATE {"Date"};
	static const std::string HF_RECORD_ROUTE {"Record-Route"};
	static const std::string HF_TIMESTAMP {"Timestamp"};
	static const std::string HF_HIDE {"Hide"};
	static const std::string HF_MAX_FORWARDS {"Max-Forwards"};
	static const std::string HF_ORGANIZATION {"Organization"};
	static const std::string HF_PROXY_AUTHORIZATION {"Proxy-Authorization"};
	static const std::string HF_PROXY_REQUIRE {"Proxy-Require"};
	static const std::string HF_ROUTE {"Route"};
	static const std::string HF_REQUIRE {"Require"};
	static const std::string HF_RESPONSE_KEY {"Response-Key"};
	static const std::string HF_USER_AGENT {"User-Agent"};
	static const std::string HF_PROXY_AUTHENTICATE {"Proxy-Authenticate"};
	static const std::string HF_RETRY_AFTER {"Retry-After"};
	static const std::string HF_SERVER {"Server"};
	static const std::string HF_UNSUPPORTED {"Unsupported"};
	static const std::string HF_WARNING {"Warning"};
	static const std::string HF_WWW_AUTHENTICATE {"WWW-Authenticate"};
	static const std::string HF_AUTHORIZATION {"Authorization"};

	// Subscribe/Notify header fields.
	static const std::string HF_SUBSCRIPTION_STATE {"Subscription-State"};

	//	Parsing elements
	static const std::string ELEM_SPACE {" "};
	static const std::string ELEM_SEPERATOR {":"};
	static const std::string ELEM_PADDEDSEPERATOR {": "};
	static const std::string ELEM_TAGSEPERATOR {"{"};
	static const std::string ELEM_NEWLINE {"\r\n"};
	static const std::string ELEM_HEADERSECTIONDELIMITER {"\r\n\r\n"};
	static const std::string ELEM_LWSP {"\r\n "};
	static const std::string ELEM_LWSP1 {"\r\n\t"};
	static const std::string ELEM_SDPBlockStart = "v=0\r\n";


	//	Some common elements for builing the SIP message
	static const std::string SIP_ADDR_PREFIX {"sip:"};

	// Helpers to parse the SIP buffer
	static const std::regex SIP_PATTERN_STARTLINE {"^(MESSAGE|INFO|INVITE|ACK|OPTIONS|BYE|CANCEL|REGISTER|SUBSCRIBE|NOTIFY|SIP/"
												   "2.0)\\s{1,1}([^\\s]+)\\s{1,1}([^\\r\\n]*)"};
	static const std::regex SIP_PATTERN_CONTENT_LENGTH {"^Content-Length:\\s{1,1}(\\d+)\\s*(\\r\\n|\\n)"};
	static const std::regex SIP_PATTERN_CONTENT_TYPE {"^Content-type:\\s{1,1}([a-z|A-Z|\\-|/]+)\\s*(\\r\\n|\\n)"};
	static const std::regex SIP_PATTERN_HEADER {"([^:\\s]*)\\s*:\\s*([^\\r\\n]*)[\\x0A\\x0D]*"};

	static const std::regex SIP_PATTERN_BODY {"([vosiuepcbtzkma]{1})=([^\\r\\n]*)"};
	static const std::regex SIP_PATTERN_BODY_ALINE {"^([^:|\\r\\n]*)[:]{1}(.*$)[\\r\\n]?|(.*)[\\r\\n]"};
	static const std::regex SIP_PATTERN_BODY_ILINE {"^(.*) \\(([^\\)]*)\\) ([^\\s|\\r\\n]*)"};
	static const std::regex SIP_PATTERN_BODY_CLINE {"^(.*) (.*) ([^\\s|\\r\\n]*)"};
	static const std::regex SIP_PATTERN_BODY_OLINE {"([^\\s]*) (\\d*) (\\d*) (\\w*) (\\w*) ([^\\s]*)"};
} // namespace siddiqsoft