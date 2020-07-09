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
		static const inline std::string MessageTypeRequest	= "com.siddiqsoftware.sip2json.request";
		static const inline std::string MessageTypeResponse = "com.siddiqsoftware.sip2json.response";

	private:
		/// @brief Creates a basic SIP Message content in json. This method is used by the createRequest and createResponse methods
		/// @param messageType Must be one of MessageTypeRequest or MessageTypeResponse
		/// @return json document with basic sections
		static sipmessage createRawMessage(const std::string& messageType)
		{
			return nlohmann::json {{"type", messageType},
								   {"version", MetaSchemaVersion},
								   {"mb", nullptr},
								   {"ticks", std::chrono::system_clock::now().time_since_epoch().count()},
								   {"mh", nullptr}};
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

			sipm["type"] = sipmessage::MessageTypeRequest;
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// rl ==> "request-line" (request message type) and sl ==> "status-line" (response message type)
			sipm["rl"] = {{"method", method}, {"uri", uri}, {"version", SIPVER_20}};
			// message-headers
			if (!callId.empty()) sipm["/mh/Call-ID"_json_pointer] = callId;
			if (cseq > 0) sipm["/mh/CSeq"_json_pointer] = fmt::format("{} {}", cseq, method);
			sipm["/mh/Date"_json_pointer] = getRFC1123();

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
			sipm["type"] = sipmessage::MessageTypeResponse;
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// sl ==> "status-line" (response message type)
			sipm["sl"] = {{"status", statusCode}, {"reason", getReasonPhrase(statusCode)}, {"version", SIPVER_20}};
			sipm["/mh/Date"_json_pointer] = getRFC1123();

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
			sip2json_throw_if<invalid_document_error>((!this->contains("/mh/Call-ID") && !this->contains("ticks")),
													  "{}:Missing Call-ID and ticks. Required elements.",
													  __func__);

			return nlohmann::json {
					{"specversion", "1.0"},
					{"source", "sip2json"},
					{"datacontenttype", "application/json+sip2json"},
					{"time", getISO8601()},
					{"subject", this->value("type", "")},
					{"data", *this},
					{"type", this->value("type", "")},
					{"id", fmt::format("{}.{}", this->value("/mh/Call-ID"_json_pointer, ""), this->value("ticks", 0I64))}};
		}


	public:
		void setUser(const std::string& userName) {};

	public:
		inline const uint32_t getContentLength() { return this->value("/mh/Content-Length"_json_pointer, 0); };
		inline const uint32_t getExpires() { return this->value("/mh/Expires"_json_pointer, 0); };
		inline const auto	  getContentType()
		{
			// Special concession for some SIP servers which incorrectly encode this field.
			// First we try the Content-Type and default to looking up Content-type else return empty string.
			if (this->at("mh").contains("Content-Type"))
			{
				auto ct = this->at("mh").at("Content-Type");
				return ct.is_null() ? std::string {} : ct.get<std::string>();
			}
			else if (this->at("mh").contains("Content-type"))
			{
				auto ct = this->at("mh").at("Content-type");
				return ct.is_null() ? std::string {} : ct.get<std::string>();
			}

			return std::string {};
		};
		inline const auto getCallID() { return this->value("/mh/Call-ID"_json_pointer, ""); };
		inline const auto getMethod() { return this->value("/rl/method"_json_pointer, ""); };
		inline const auto getUri() { return this->value("/rl/uri"_json_pointer, ""); };
		inline const auto getStatusCode() { return this->value("/sl/status"_json_pointer, 0); };
		inline const auto getReason() { return this->value("/sl/reason"_json_pointer, ""); };
		inline auto		  getHeaders() { return this->at("mh"); };
		inline auto		  getBody() { return this->at("mb"); };
		inline auto		  isMessageTypeRequest() { return (this->value("type", "").compare(MessageTypeRequest) == 0); };
		inline auto		  isMessageTypeResponse() { return (this->value("type", "").compare(MessageTypeResponse) == 0); };
	}; // class sipmessage
} // namespace siddiqsoftware