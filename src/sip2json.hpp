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
			//sipm["/mh/Content-Type"_json_pointer]	= nullptr;
			sipm["/mh/Date"_json_pointer]			= getRFC1123();
			sipm["/mh/User-Agent"_json_pointer]		= "sip2json";
			sipm["/mh/Authorization"_json_pointer]	= nullptr;

			// message-body
			sipm["mb"] = nullptr;

			return sipm;
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

			return sipm;
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


			return sipm;
		}
#pragma endregion

	public:
		static std::string serialize(nlohmann::json& sipm)
		{
			std::string buffer {};
			std::string contentType {};

			if (sipm.size() == 0) throw std::invalid_argument(fmt::format("{}:sipm is empty.", __func__));


			if (sipm.value("/type"_json_pointer, std::string {}).compare("request") == 0)
			{
				// Request Line
				buffer = fmt::format("{}\r\n", sipm.value("/rl"_json_pointer, std::string {}));
			}
			else if (sipm.value("/type"_json_pointer, std::string {}).compare("response") == 0)
			{
				// Status Line
				buffer = fmt::format("{}\r\n", sipm.value("/sl"_json_pointer, std::string {}));
			}
			else
			{
				throw std::invalid_argument(fmt::format("{}:sipm /type is neither `request` nor `response`.", __func__));
			}

			// Headers
			auto mh = sipm.at("/mh"_json_pointer);
			if (mh.size() > 0)
			{
				for (auto& [key, value] : mh.items())
				{
					if (key.compare("Content-Type") == 0) contentType = value;

					if (value.is_null())
					{ /* do nothing; skip field. */
					}
					else if (value.is_number_unsigned())
					{
						buffer += fmt::format("{}: {}\r\n", key, value.get<uint64_t>());
					}
					else if (value.is_number_integer())
					{
						buffer += fmt::format("{}: {}\r\n", key, value.get<int64_t>());
					}
					else if (value.is_number_float())
					{
						buffer += fmt::format("{}: {}\r\n", key, value.get<float>());
					}
					else
					{
						buffer += fmt::format("{}: {}\r\n", key, value);
					}
				};

				// End of the message header section
				buffer += "\r\n";
			}
			else
			{
				throw std::invalid_argument(fmt::format("{}:sipm does not contain mh.", __func__));
			}

			// Body
			// NOTE: we extract the contentType value during the header serialization.
			if (contentType.compare("application/sdp") == 0)
			{
				if (sipm.contains("/mb"_json_pointer))
				{
					if (sipm.contains("/mb/sdp"_json_pointer))
					{
						// the sdp is stored as an array of objects
						auto sdp = sipm.at("/mb/sdp"_json_pointer);
						for (auto& item : sdp)
						{
							buffer += "v=0\r\n"; // always write this out at the start of each SDP element
						}
					}
					else
					{
						throw std::invalid_argument(fmt::format("{}:sipm mb does not have sdp element.", __func__));
					}
				}
				else
				{
					throw std::invalid_argument(fmt::format("{}:sipm does not have mb.", __func__));
				}
			}

			return buffer;
		}


	public:
		/// @brief De-serialize the *first* SIP message (if present) from the buffer. Repeated calls to this method will extract the remaining messages.
		/// @param dest An existing json object; existing items will be replaced.
		/// @param bufferStart iterator to the start of the buffer the client expects a SIP message.
		/// @param bufferEnd iterator to the end of the buffer the client expects a SIP message.
		/// @return true/false depending on whether the buffer contained a sipmessage
		static bool
		parseFromBuffer(nlohmann::json& dest, std::string::iterator& bufferStart, std::string::iterator& bufferEnd) noexcept(false)
		{
			return false;
		}
	};


	// References
	// SIP Messages: https://tools.ietf.org/html/rfc3261#section-7
	// JSON Library: https://nlohmann.github.io/json/
	// FMT Library : https://fmt.dev/latest/index.html
} // namespace siddiqsoftware