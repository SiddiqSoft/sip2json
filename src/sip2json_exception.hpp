/*
  A SIP Parser for Modern C++
  Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

#pragma once

#include <exception>
#include <string>

#include "nlohmann/json.hpp"
#include "fmt/format.h"

namespace siddiqsoftware
{
	enum sip2jsonErrorCodes
	{
		ok,
		incomplete_buffer_for_parse,
		incomplete_buffer_for_content,
		incomplete_buffer_for_header,
		invalid_startline,
		unsupported_contenttype,
		invalid_document,
		empty_message,
		unknown = -1
	};


	NLOHMANN_JSON_SERIALIZE_ENUM(sip2jsonErrorCodes,
								 {unknown, nullptr},
								 {ok, "ok"},
								 {incomplete_buffer_for_parse, "incomplete_buffer_for_parse"},
								 {incomplete_buffer_for_content, "incomplete_buffer_for_content"},
								 {incomplete_buffer_for_header, "incomplete_buffer_for_header"},
								 {invalid_startline, "invalid_startline"},
								 {unsupported_contenttype, "unsupported_contenttype"},
								 {invalid_document, "invalid_document"},
								 {empty_message, "empty_message"});


	/// @brief Runtime exception class. Helper to easily classify the various error states during parsing.
	/// @tparam ...Targs
	template <typename... Targs> class sip2json_exception : public std::runtime_error
	{
	private:
		sip2jsonErrorCodes errCode = sip2jsonErrorCodes::unknown;

	public:
		sip2json_exception(sip2jsonErrorCodes ec, const std::string& formatSpec, Targs... args)
			: std::runtime_error(fmt::format(formatSpec, args...))
			, errCode(ec)
		{
		}

			 operator uint32_t() const { return errCode; }
		bool is_ok() { return errCode == ok; }
		bool is_incomplete_buffer_for_parse() { return errCode == incomplete_buffer_for_parse; }
		bool is_incomplete_buffer_for_content() { return errCode == incomplete_buffer_for_content; }
		bool is_incomplete_buffer_for_header() { return errCode == incomplete_buffer_for_header; }
		bool is_invalid_startline() { return errCode == invalid_startline; }
		bool is_unsupported_contenttype() { return errCode == unsupported_contenttype; }
	};
} // namespace siddiqsoftware