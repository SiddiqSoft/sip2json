/*
	A SIP Parser for Modern C++
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
#include <variant>
#include <regex>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>

#define FMT_HEADER_ONLY 1
#include "fmt/chrono.h"
#include "nlohmann/json.hpp"

#include "sip2json_response_codes.hpp"
#include "sip2json_utils.hpp"
#include "sip2json_exception.hpp"


namespace siddiqsoftware
{
	enum class SIPMessageType
	{
		notspecified,
		request	 = 1,
		response = 2
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(SIPMessageType,
								 {{SIPMessageType::request, "request"},
								  {SIPMessageType::response, "response"},
								  {SIPMessageType::notspecified, "notspecified"}});

	class sipmessage : public nlohmann::json
	{
		static const inline std::string MetaLibName		  = "sip2json";
		static const inline std::string MetaSchemaVersion = "1.0.1";
		static const inline std::string MetaParserVersion = "1.11.1";

	public:
		sipmessage()
		{
			using namespace std;
			// Overwrite the source object's values
			(*this)["meta"s] = {{"version"s, fmt::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
								{"time"s, TimeAsISO8601()},
								{"ttx"s, 0}};
		}

		sipmessage(const nlohmann::json& src) { this->update(src); }
		sipmessage(nlohmann::json&& src) { nlohmann::json((*this)).operator=(std::move(src)); }

		sipmessage(sipmessage&&) = default;
		sipmessage(const sipmessage& src) { this->update(nlohmann::json(src)); }

		sipmessage& operator=(sipmessage&& src) = default;
		sipmessage& operator					=(nlohmann::json&& src) { nlohmann::json((*this)).operator=(std::move(src)); }

		/// @brief Instantiates request message given method and uri with option callId and cseq
		/// @param method One of the supported SIP methods
		/// @param uri Request URI
		/// @param callId Optional CallId
		/// @param cseq Optional Cseq; the string value is build using this parameter and the method
		/// @return
		sipmessage(const std::string& method, const std::string& uri, const std::string& callId = {}, uint32_t cseq = 0)
		{
			using namespace std;

			update({{"s"s, {{"type"s, SIPMessageType::request}, {"method"s, method}, {"uri"s, uri}, {"version"s, SIPVER_20}}},
					{"b"s, nullptr},
					{"meta"s,
					 {{"version"s, fmt::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
					  {"time"s, TimeAsISO8601()},
					  {"ttx"s, 0}}},
					{"h"s,
					 {{"User-Agent"s, fmt::format("{}/{} (schema:{})"s, MetaLibName, MetaParserVersion, MetaSchemaVersion)},
					  {"Date"s, TimeAsRFC1123()}}}});

			// request-line: METHOD Request-URI SIP/2.0
			// message-headers
			if (!callId.empty()) setHeader("Call-ID"s, callId);
			if (cseq > 0) setHeader("CSeq"s, fmt::format("{} {}"s, cseq, method));
		}

		/// @brief Instantiates a response message from scratch or optionally from existing sipmessage request
		/// @param statusCode Status Code for this message, the reason is built using map
		/// @param src Optional sipmessage object of type request
		/// @return
		sipmessage(uint32_t statusCode, const sipmessage& src)
		{
			using namespace std;

			update(src);

			// Overwrite the source object's values
			(*this)["meta"s] = {{"version"s, fmt::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
								{"time"s, TimeAsISO8601()},
								{"ttx"s, 0}};

			//// We must clear these values in case we are updating an existing object.
			//erase("s"s);
			// "status-line" (Status Reason Version)
			(*this)["s"s] = {{"type"s, SIPMessageType::response},
							 {"status"s, statusCode},
							 {"reason"s, getReasonPhrase(statusCode)},
							 {"version"s, SIPVER_20}};

			(*this)["h"s]["User-Agent"s] = fmt::format("{}/{} (schema:{})"s, MetaLibName, MetaParserVersion, MetaSchemaVersion);
			setHeader("Date"s, TimeAsRFC1123());
		}


		/// @brief Instantiates a response message from scratch or optionally from existing sipmessage request
		/// @param statusCode Status Code for this message, the reason is built using map
		/// @param src Optional sipmessage object of type request
		/// @return
		sipmessage(uint32_t statusCode)
		{
			using namespace std;

			update(nlohmann::json {
					{"s"s,
					 {{"type"s, SIPMessageType::response},
					  {"status"s, statusCode},
					  {"reason"s, getReasonPhrase(statusCode)},
					  {"version"s, SIPVER_20}}},
					{"b"s, nullptr},
					{"meta"s,
					 {{"version"s, fmt::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
					  {"time"s, TimeAsISO8601()},
					  {"ttx"s, 0}}},
					{"h"s,
					 {{"User-Agent"s, fmt::format("{}/{} (schema:{})"s, MetaLibName, MetaParserVersion, MetaSchemaVersion)},
					  {"Date"s, TimeAsRFC1123()}}}});
		}


	public:
		/// @brief Returns the header object reference
		/// @return Return the header object reference
		inline auto&			headers() { return this->at("h"); };
		template <class T> auto getHeader(const std::string& key, std::optional<T> defaultValue = {})
		{
			// Return the value or the default for the object.
			return (*this)["h"].value(key, defaultValue.value_or(T {}));
		};
		inline auto& setUserAgent(const std::string& ua)
		{
			if (!ua.empty())
				setHeader("User-Agent", fmt::format("{}/{} (schema:{}) {}", MetaLibName, MetaParserVersion, MetaSchemaVersion, ua));
			else
				setHeader("User-Agent", fmt::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion));
			return *this;
		};
		inline auto		getUserAgent() { return getHeader<std::string>("User-Agent"); };
		inline uint32_t getContentLength() { return getHeader<uint32_t>("Content-Length"); };
		inline uint32_t getExpires() { return getHeader<uint32_t>("Expires"); };
		inline auto		getContentType()
		{
			// Special concession for some SIP servers which incorrectly encode this field.
			// First we try the Content-Type and default to looking up Content-type else return empty string.
			if (headers().contains("Content-Type"))
			{
				auto ct = headers().at("Content-Type");
				return ct.is_null() ? std::string {} : ct.get<std::string>();
			}
			else if (headers().contains("Content-type"))
			{
				auto ct = headers().at("Content-type");
				return ct.is_null() ? std::string {} : ct.get<std::string>();
			}

			return std::string {};
		};
		inline auto getCallID() { return getHeader<std::string>("Call-ID"); };
		inline auto getMethod() { return this->value("/s/method"_json_pointer, ""); };
		inline auto getUri() { return this->value("/s/uri"_json_pointer, ""); };
		inline auto getStatusCode() { return this->value("/s/status"_json_pointer, 0); };
		inline auto getReason() { return this->value("/s/reason"_json_pointer, ""); };

		/// @brief Returns a reference to the body object. This method should be used to change the body contents to text/plain or non-SDP content-type.
		/// @return Returns reference to the body element b
		inline auto& body() { return this->at("b"); };

		inline auto isMessageRequest()
		{
			return (this->value("/s/type"_json_pointer, SIPMessageType::notspecified) == SIPMessageType::request);
		};

		inline auto isMessageResponse()
		{
			return (this->value("/s/type"_json_pointer, SIPMessageType::notspecified) == SIPMessageType::response);
		};

		// mutators
	public:
		/// @brief Sets a header key-value
		/// @tparam T Type of object; this is typically inferred by the compiler.
		/// @param key The header name
		/// @param v The header value.
		/// @return Self.
		template <typename T> inline sipmessage& setHeader(const std::string& key, const T& v)
		{
			(*this)["h"][key] = v;
			return *this;
		};

		/// @brief Set element within the body to the given value.
		/// @tparam T Type of object; this is typically inferred by the compiler.
		/// @param key The key within the body section.
		/// @param v The value. Json, string (for text/plain)
		/// @return Self
		template <typename T> inline sipmessage& body(const json_pointer& key, const T& v)
		{
			(*this)["b"][key] = v;
			return *this;
		};
	}; // class sipmessage
} // namespace siddiqsoftware