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
	public:
		static const inline std::string MessageTypeRequest	= "sip2json.request";
		static const inline std::string MessageTypeResponse = "sip2json.response";

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