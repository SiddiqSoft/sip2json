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
	enum class sip2jsonErrors : uint32_t
	{
		ok = std::numeric_limits<uint32_t>::min(),
		/* parse errors */
		incomplete_buffer_for_parse,
		incomplete_buffer_for_content,
		incomplete_buffer_for_header,
		invalid_startline,
		unsupported_contenttype,
		/* serialization errors */
		invalid_document,
		invalid_document_unsupported_method,
		invalid_document_unsupported_content,
		empty_message,
		unknown = std::numeric_limits<uint32_t>::max()
	};


	NLOHMANN_JSON_SERIALIZE_ENUM(sip2jsonErrors,
								 {unknown, nullptr},
								 {ok, "ok"},
								 /* parse errors */
								 {incomplete_buffer_for_parse, "incomplete_buffer_for_parse"},
								 {incomplete_buffer_for_content, "incomplete_buffer_for_content"},
								 {incomplete_buffer_for_header, "incomplete_buffer_for_header"},
								 {invalid_startline, "invalid_startline"},
								 {unsupported_contenttype, "unsupported_contenttype"},
								 /* serialization errors */
								 {invalid_document, "invalid_document"},
								 {invalid_document_unsupported_method, "invalid_document_unsupported_method"},
								 {invalid_document_unsupported_content, "invalid_document_unsupported_content"},
								 {empty_message, "empty_message"});


	class incomplete_buffer_for_parse_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::incomplete_buffer_for_parse;

	public:
		incomplete_buffer_for_parse_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	class incomplete_buffer_for_content_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::incomplete_buffer_for_content;

	public:
		incomplete_buffer_for_content_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	class incomplete_buffer_for_header_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::incomplete_buffer_for_header;

	public:
		incomplete_buffer_for_header_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	class invalid_startline_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::invalid_startline;

	public:
		invalid_startline_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	class unsupported_contenttype_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::unsupported_contenttype;

	public:
		unsupported_contenttype_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	class invalid_document_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::invalid_document;

	public:
		invalid_document_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	class empty_message_error : public std::runtime_error
	{
	public:
		sip2jsonErrors errCode = sip2jsonErrors::empty_message;

	public:
		empty_message_error(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};


	/// @brief Create and throw a sip2json_error object.
	/// @tparam ...Args Automatically deduced template argument
	/// @param ec Error Code (type of error to be created)
	/// @param formatSpec fmt::format spec
	/// @param ...args fmt::format arguments
	/// @return Throws an object sip2json_error object.
	template <class E, typename... Args> void sip2json_throw(const std::string& formatSpec, Args... args) noexcept(false)
	{
		auto e = E(fmt::format(formatSpec, args...));
		throw e;
	}


	template <class E, typename... Args>
	void sip2json_throw_if(bool predicate, const std::string& formatSpec, Args... args) noexcept(false)
	{
		if (predicate)
		{
			auto e = E(fmt::format(formatSpec, args...));
			throw e;
		}
	}

} // namespace siddiqsoftware
