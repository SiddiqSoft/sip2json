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

#include "sip2json_response_codes.h"
#include "sip2json_utils.h"

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

namespace siddiqsoftware
{
#pragma region SIP match patterns
#pragma endregion


	class sip2json
	{
	public:
		static const inline std::string_view MetaLibName	   = "sip2json";
		static const inline std::string_view MetaSchemaVersion = "0.1.0";
		static const inline std::string_view MetaParserVersion = "1.0.0";

		static const inline std::string_view MessageTypeRequest	 = "sip2json.request";
		static const inline std::string_view MessageTypeResponse = "sip2json.response";

		static const inline std::string_view SIPVersion				  = "SIP/2.0";
		static const inline std::string_view SIPLineTerminator		  = "\r\n";
		static const inline std::string_view SIPHeaderBlockTerminator = "\r\n\r\n";
		static const inline std::string_view SIPSDPBlockStart		  = "v=0\r\n";


#pragma region SIPMessage helpers
	private:
		/// @brief Creates a basic SIP Message content in json. This method is used by the createRequest and createResponse methods
		/// @param messageType Must be one of MessageTypeRequest or MessageTypeResponse
		/// @return json document with basic sections
		static nlohmann::json createRawMessage(const std::string_view& messageType)
		{
			return nlohmann::json {{"type", messageType},
								   {"version", MetaSchemaVersion},
								   {"mb", nullptr},
								   {"mh",
									{{"Call-ID", nullptr},
									 {"Date", getRFC1123()},
									 {"To", nullptr},
									 {"From", nullptr},
									 {"CSeq", nullptr},
									 {"Content-Length", 0},
									 {"Content-Type", nullptr},
									 {"User-Agent", fmt::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
									 {"Max-Forwards", 0},
									 {"Via", nullptr},
									 {"Authorization", nullptr}}}};
		}

	public:
		static nlohmann::json createRequest(const std::string_view& method,
											const std::string_view& uri,
											const std::string_view& callId = {},
											uint32_t				cseq   = 0,
											nlohmann::json&			sipm   = createRawMessage(MessageTypeRequest))
		{
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// rl ==> "request-line" (request message type) and sl ==> "status-line" (response message type)
			sipm.erase("sl");
			sipm["/rl/method"_json_pointer]	 = method;
			sipm["/rl/uri"_json_pointer]	 = uri;
			sipm["/rl/version"_json_pointer] = SIPVersion;
			// message-headers
			if (!callId.empty()) sipm["/mh/Call-ID"_json_pointer] = callId;
			if (cseq > 0) sipm["/mh/CSeq"_json_pointer] = fmt::format("{} {}", cseq, method);

			return sipm;
		}


		static nlohmann::json createResponse(uint32_t statusCode, nlohmann::json& sipm = createRawMessage(MessageTypeResponse))
		{
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// sl ==> "status-line" (response message type)
			sipm.erase("rl");
			sipm["/sl/status"_json_pointer]	 = statusCode;
			sipm["/sl/reason"_json_pointer]	 = getReasonPhrase(statusCode);
			sipm["/sl/version"_json_pointer] = SIPVersion;

			return sipm;
		}
#pragma endregion

	public:
		static std::string serialize(nlohmann::json& sipm)
		{
			std::string buffer {};
			std::string contentType {};

			if (sipm.size() == 0) throw std::invalid_argument(fmt::format("{}:sipm is empty.", __func__));


			if (sipm.value("/type"_json_pointer, std::string {}).compare(MessageTypeRequest) == 0)
			{
				// Request Line
				buffer = fmt::format("{} {} SIP/2.0\r\n",
									 sipm.value("/rl/method"_json_pointer, std::string {}),
									 sipm.value("/rl/uri"_json_pointer, std::string {}));
			}
			else if (sipm.value("/type"_json_pointer, std::string {}).compare(MessageTypeResponse) == 0)
			{
				// Status Line
				buffer = fmt::format("SIP/2.0 {} {}\r\n",
									 sipm.value("/sl/status"_json_pointer, 0),
									 sipm.value("/sl/reason"_json_pointer, std::string {}));
			}
			else
			{
				throw std::invalid_argument(
						fmt::format("{}:sipm /type is neither `{}` nor `{}`.", __func__, MessageTypeRequest, MessageTypeResponse));
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
	// SIP Response Codes: https://en.wikipedia.org/wiki/List_of_SIP_response_codes
	// JSON Library: https://nlohmann.github.io/json/
	// FMT Library : https://fmt.dev/latest/index.html
} // namespace siddiqsoftware