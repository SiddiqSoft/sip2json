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

#include "sip2json_response_codes.hpp"
#include "sip2json_utils.hpp"

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
		/// @brief Create a skeleton request message
		/// @param method One of the SIP method
		/// @param uri SIP URI
		/// @param callId Optional Call-ID
		/// @param cseq Optional CSeq
		/// @param src Optional base sipmessage
		/// @return sipmessage
		static sipmessage
		createRequest(const std::string& method, const std::string& uri, const std::string& callId = {}, uint32_t cseq = 0)
		{
			auto sipm = createRawMessage(sipmessage::MessageTypeRequest);

			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// rl ==> "request-line" (request message type) and sl ==> "status-line" (response message type)
			sipm["s"] = {{"type", sipmessage::MessageTypeRequest}, {"method", method}, {"uri", uri}, {"version", SIPVER_20}};
			// message-headers
			if (!callId.empty()) sipm["/h/Call-ID"_json_pointer] = callId;
			if (cseq > 0) sipm["/h/CSeq"_json_pointer] = fmt::format("{} {}", cseq, method);
			sipm["/h/Date"_json_pointer] = getRFC1123();

			return sipm;
		}


		/// @brief Create a skeleton response/status message
		/// @param statusCode Unsigned integer representing status code of the messages
		/// @param src Optional. Base sipmessage
		/// @return sipmessage
		static sipmessage createResponse(uint32_t statusCode, std::optional<sipmessage> src = {})
		{
			auto sipm = src.value_or(createRawMessage(sipmessage::MessageTypeResponse));

			// We must clear these values in case we are updating an existing object.
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// sl ==> "status-line" (response message type)
			sipm["s"]					 = {{"type", sipmessage::MessageTypeResponse},
							{"status", statusCode},
							{"reason", getReasonPhrase(statusCode)},
							{"version", SIPVER_20}};
			sipm["/h/Date"_json_pointer] = getRFC1123();

			return sipm;
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

		/// @brief Returns an envelope conforming to the CloudEvent spec. https://github.com/cloudevents/spec/blob/v1.0/spec.md
		/// @return json containing the CloudEvent spec.
		nlohmann::json to_cloudEvent()
		{
			sip2json_throw_if<invalid_document_error>((!this->contains("/h/Call-ID") && !this->contains("z")),
													  "{}:Missing Call-ID and ticks. Required elements.",
													  __func__);

			return nlohmann::json {
					{"specversion", "1.0"},
					{"source", "sip2json"},
					{"datacontenttype", "application/json+sip2json"},
					{"time", getISO8601()},
					{"subject", this->value("/s/type"_json_pointer, "")},
					{"data", *this},
					{"type", fmt::format("com.siddiqsoftware.sip2json.{}", this->value("/s/type"_json_pointer, ""))},
					{"id", fmt::format("{}.{}", this->value("/h/Call-ID"_json_pointer, ""), this->value("z", 0I64))}};
		}

	public:
		void setUser(const std::string& userName) {};

	public:
		inline const uint32_t getContentLength() { return this->value("/h/Content-Length"_json_pointer, 0); };
		inline const uint32_t getExpires() { return this->value("/h/Expires"_json_pointer, 0); };
		inline const auto	  getContentType()
		{
			// Special concession for some SIP servers which incorrectly encode this field.
			// First we try the Content-Type and default to looking up Content-type else return empty string.
			if (this->at("h").contains("Content-Type"))
			{
				auto ct = this->at("h").at("Content-Type");
				return ct.is_null() ? std::string {} : ct.get<std::string>();
			}
			else if (this->at("h").contains("Content-type"))
			{
				auto ct = this->at("h").at("Content-type");
				return ct.is_null() ? std::string {} : ct.get<std::string>();
			}

			return std::string {};
		};
		inline const auto getCallID() { return this->value("/h/Call-ID"_json_pointer, ""); };
		inline const auto getMethod() { return this->value("/s/method"_json_pointer, ""); };
		inline const auto getUri() { return this->value("/s/uri"_json_pointer, ""); };
		inline const auto getStatusCode() { return this->value("/s/status"_json_pointer, 0); };
		inline const auto getReason() { return this->value("/s/reason"_json_pointer, ""); };
		inline auto		  getHeaders() { return this->at("h"); };
		inline auto		  getBody() { return this->at("b"); };
		inline auto isMessageTypeRequest() { return (this->value("/s/type"_json_pointer, "").compare(MessageTypeRequest) == 0); };
		inline auto isMessageTypeResponse() { return (this->value("/s/type"_json_pointer, "").compare(MessageTypeResponse) == 0); };
	}; // class sipmessage
} // namespace siddiqsoftware