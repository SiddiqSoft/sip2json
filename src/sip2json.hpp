/*
  A SIP Parser for Modern C++
  Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
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

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

namespace siddiqsoftware
{
#pragma region SIP match patterns
#pragma endregion


	class sip2json
	{
#pragma region Datetime helpers
	public:
		/// @brief Create a string representation of the timepoint as RFC1123 spec
		/// @param tp Optional system_clock::timepoint; uses "now" if not provided
		/// @return String with your date/time as "Sun, 28 Jun 2020 23:29:00 GMT"
		static std::string getRFC1123(std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now()) noexcept(false)
		{
			return fmt::format("{:%a, %d %b %Y %T} GMT", fmt::gmtime(std::chrono::system_clock::to_time_t(tp)));
		}


		/// @brief Creates a string representaiton of the date time in ISO8601 format with millisecond precision.
		/// @param tp Optional system_clock::timepoint; uses "now" if not provided
		/// @return String ISO8601 "2020-06-28T23:29:00.000Z"
		static std::string getISO8601(std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now()) noexcept(false)
		{
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
			return fmt::format("{:%Y-%m-%dT%T}.{:03}Z", fmt::gmtime(std::chrono::system_clock::to_time_t(tp)), ms);
		}
#pragma endregion

	public:
		/// @brief Creates a pseudo random number generated UUID v4. It is best to use platform-specific method to ensure guid
		/// @return string 44 character of the format: 7792eaf4-456f-4d47-d93-863af0e0-a8b99b9b9988
		static std::string createCallId()
		{
			static std::random_device			 rd;
			static std::mt19937_64				 generator(rd());
			static std::uniform_int_distribution ud(0, 15);
			static std::uniform_int_distribution ud2(8, 11);

			std::stringstream					 rets;

			rets << std::hex;
			for (auto i = 0; i < 8; i++)
				rets << ud(generator);
			rets << "-";
			for (auto i = 0; i < 4; i++)
				rets << ud(generator);
			rets << "-4";
			for (auto i = 0; i < 3; i++)
				rets << ud(generator);
			rets << "-";
			for (auto i = 0; i < 3; i++)
				rets << ud(generator);
			rets << "-";
			for (auto i = 0; i < 8; i++)
				rets << ud(generator);
			rets << "-";
			for (auto i = 0; i < 12; i++)
				rets << ud2(generator);
			return rets.str();
		}

#pragma region SIPMessage helpers
	private:
		/// @brief Creates a basic SIP Message content in json. This method is used by the createRequest and createResponse methods
		/// @return json with basic "sections": mh and mb
		static nlohmann::json createRawMessage()
		{
			nlohmann::json sipm;

			// message-headers
			sipm["/mh/To"_json_pointer]				= nullptr;
			sipm["/mh/From"_json_pointer]			= nullptr;
			sipm["/mh/CSeq"_json_pointer]			= nullptr;
			sipm["/mh/Call-ID"_json_pointer]		= nullptr;
			sipm["/mh/Max-Forwards"_json_pointer]	= 1;
			sipm["/mh/Via"_json_pointer]			= nullptr;
			sipm["/mh/Content-Length"_json_pointer] = 0;
			sipm["/mh/Content-Type"_json_pointer]	= nullptr;
			sipm["/mh/Date"_json_pointer]			= getRFC1123();
			sipm["/mh/User-Agent"_json_pointer]		= "sip2json";
			sipm["/mh/Authorization"_json_pointer]	= nullptr;

			// message-body
			sipm["mb"] = nullptr;

			return std::move(sipm);
		}

	public:
		static nlohmann::json createRequest(const std::string_view& method,
											const std::string_view& uri,
											const std::string_view& callId = {},
											uint32_t				cseq   = 1)
		{
			// A generic message is created with common mh and empty mb
			nlohmann::json sipm = createRawMessage();

			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			sipm["type"] = "request";

			// rl ==> "request-line" (request message type) and sl ==> "status-line" (response message type)
			sipm["rl"] = fmt::format("{} {} SIP/2.0", method, uri);

			// message-headers
			sipm["/mh/Call-ID"_json_pointer] = callId.empty() ? createCallId() : callId;
			sipm["/mh/CSeq"_json_pointer]	 = fmt::format("{} {}", cseq, method);

			return std::move(sipm);
		}


		static nlohmann::json createResponse(uint32_t statusCode, const std::string_view& reasonPhrase)
		{
			// A generic message is created with common mh and empty mb
			nlohmann::json sipm = createRawMessage();

			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			sipm["type"] = "response";

			// sl ==> "status-line" (response message type)
			sipm["sl"] = fmt::format("SIP/2.0 {} {}", statusCode, reasonPhrase);


			return std::move(sipm);
		}
#pragma endregion

		/// @brief De-serialize the SIP message (if present)
		/// @param bufferStart iterator to the start of the buffer the client expects a SIP message.
		/// @param bufferEnd iterator to the end of the buffer the client expects a SIP message.
		/// @return A json ojbect representing the first SIP message if one is found in the range.
		static nlohmann::json parseFromBuffer(std::string::iterator& bufferStart, std::string::iterator& bufferEnd) noexcept(false)
		{
			nlohmann::json retSipMessage;

			return std::move(retSipMessage);
		}
	};


	// References
	// SIP Messages: https://tools.ietf.org/html/rfc3261#section-7
	// JSON Library: https://nlohmann.github.io/json/
	// FMT Library : https://fmt.dev/latest/index.html
} // namespace siddiqsoftware