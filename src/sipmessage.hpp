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

#include "sip2json_response_codes.hpp"
#include "sip2json_utils.hpp"
#include "sip2json_exception.hpp"

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

namespace siddiqsoftware
{
	class sipmessage : public nlohmann::json
	{
		static const inline std::string MetaLibName		  = "sip2json";
		static const inline std::string MetaSchemaVersion = "0.1.0";
		static const inline std::string MetaParserVersion = "1.0.0";

	public:
		static const inline std::string MessageTypeRequest	= "request";
		static const inline std::string MessageTypeResponse = "response";

	private:
		/// @brief Creates a basic SIP Message content in json. This method is used by the createRequest and createResponse methods
		/// @param messageType Must be one of MessageTypeRequest or MessageTypeResponse
		/// @return json document with basic sections
		static sipmessage createRawMessage(const std::string& messageType)
		{
			return nlohmann::json {{"v", MetaSchemaVersion},
								   {"b", nullptr},
								   {"z", std::chrono::system_clock::now().time_since_epoch().count()},
								   {"h", nullptr}};
		}

	public:
		sipmessage() = default;
		sipmessage(const nlohmann::json& src)
		{
			if (!src.empty()) this->update(src);
		}

		sipmessage& operator=(const nlohmann::json& src)
		{
			if (!src.empty()) this->update(src);
			return *this;
		}

	public:
		/// @brief Returns the header object reference
		/// @return Return the header object reference
		inline auto&			headers() { return this->at("h"); };
		template <class T> auto header(const std::string& key, std::optional<T> defaultValue = {})
		{
			// Return the value or the default for the object.
			return headers().value(key, defaultValue.value_or(T {}));
		}

		inline uint32_t getContentLength() { return header<uint32_t>("Content-Length"); };
		inline uint32_t getExpires() { return header<uint32_t>("Expires"); };
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
		inline auto getCallID() { return header<std::string>("Call-ID"); };
		inline auto getMethod() { return this->value("/s/method"_json_pointer, ""); };
		inline auto getUri() { return this->value("/s/uri"_json_pointer, ""); };
		inline auto getStatusCode() { return this->value("/s/status"_json_pointer, 0); };
		inline auto getReason() { return this->value("/s/reason"_json_pointer, ""); };
		/// @brief Returns a reference to the body object
		/// @return Returns reference to the body element b
		inline auto& body() { return this->at("b"); };
		inline auto	 isMessageRequest() { return (this->value("/s/type"_json_pointer, "").compare(MessageTypeRequest) == 0); };
		inline auto	 isMessageResponse() { return (this->value("/s/type"_json_pointer, "").compare(MessageTypeResponse) == 0); };

		// mutators
	public:
		template <typename T> inline sipmessage& header(const std::string& key, const T& v)
		{
			headers()[key] = v;
			return *this;
		};

		template <typename T> inline sipmessage& header(const json_pointer& key, const T& v)
		{
			headers()[key] = v;
			return *this;
		};

		template <typename T> inline sipmessage& body(const json_pointer& key, const T& v)
		{
			body()[key] = v;
			return *this;
		};


	public:
		/// @brief Create a skeleton request message
		/// @param method One of the SIP method
		/// @param uri SIP URI
		/// @param callId Optional Call-ID
		/// @param cseq Optional CSeq
		/// @param src Optional base sipmessage
		/// @return sipmessage
		static sipmessage
		create(const std::string& method, const std::string& uri, const std::string& callId = {}, uint32_t cseq = 0)
		{
			auto sipm = createRawMessage(sipmessage::MessageTypeRequest);

			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// rl ==> "request-line" (request message type) and sl ==> "status-line" (response message type)
			sipm["s"] = {{"type", sipmessage::MessageTypeRequest}, {"method", method}, {"uri", uri}, {"version", SIPVER_20}};
			// message-headers
			if (!callId.empty()) sipm.header("Call-ID", callId);
			if (cseq > 0) sipm.header("CSeq", fmt::format("{} {}", cseq, method));
			sipm.header("Date", getRFC1123());

			return sipm;
		}


		/// @brief Create a skeleton response/status message
		/// @param statusCode Unsigned integer representing status code of the messages
		/// @param src Optional. Base sipmessage
		/// @return sipmessage
		static sipmessage create(uint32_t statusCode, std::optional<sipmessage> src = {})
		{
			auto sipm = src.value_or(createRawMessage(sipmessage::MessageTypeResponse));

			// We must clear these values in case we are updating an existing object.
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// sl ==> "status-line" (response message type)
			sipm["s"] = {{"type", sipmessage::MessageTypeResponse},
						 {"status", statusCode},
						 {"reason", getReasonPhrase(statusCode)},
						 {"version", SIPVER_20}};
			sipm.header("Date", getRFC1123());

			return sipm;
		}

	public:
		/// @brief Returns an envelope conforming to the CloudEvent spec. https://github.com/cloudevents/spec/blob/v1.0/spec.md
		/// @return json containing the CloudEvent spec.
		nlohmann::json to_cloudEvent()
		{
			sip2json_throw_if<invalid_document_error>((!this->contains("/h/Call-ID") && !this->contains("z")),
													  "{}:Missing Call-ID and ticks. Required elements.",
													  std::string_view(__func__));

			return nlohmann::json {{"specversion", "1.0"},
								   {"source", "sip2json"},
								   {"datacontenttype", "application/json+sip2json"},
								   {"time", getISO8601()},
								   {"subject", this->value("/s/type"_json_pointer, "")},
								   {"data", *this},
								   {"type", fmt::format("com.siddiqsoftware.sip2json.{}", this->value("/s/type"_json_pointer, ""))},
								   {"id", fmt::format("{}.{}", getCallID(), this->value("z", 0))}};
		}
	}; // class sipmessage
} // namespace siddiqsoftware