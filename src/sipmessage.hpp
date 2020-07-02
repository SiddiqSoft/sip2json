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

			return (this->contains("/mh/Content-Type"_json_pointer)) ? this->value("/mh/Content-Type"_json_pointer, "")
																	 : this->value("/mh/Content-type"_json_pointer, "");
		};
		inline const auto getCallID() { return this->value("/mh/Call-ID"_json_pointer, ""); };
	};
} // namespace siddiqsoftware