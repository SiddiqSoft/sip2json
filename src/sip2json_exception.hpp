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

#include <exception>
#include <string>

#include <format>
#include "nlohmann/json.hpp"


namespace siddiqsoft
{
    enum class sip2jsonErrors : uint32_t
    {
        ok = 0,
        /* parse errors */
        incomplete_buffer_for_parse,
        incomplete_buffer_for_content,
        incomplete_buffer_for_header,
        invalid_startline,
        unsupported_contenttype,
        missing_required_element,
        /* serialization errors */
        invalid_document,
        invalid_document_unsupported_method,
        invalid_document_unsupported_content,
        empty_message,
        unknown = 0xFFFFFFFF
    };


    NLOHMANN_JSON_SERIALIZE_ENUM(sip2jsonErrors,
                                 {{sip2jsonErrors::ok, "ok"},
                                  {sip2jsonErrors::incomplete_buffer_for_parse, "incomplete_buffer_for_parse"},
                                  {sip2jsonErrors::incomplete_buffer_for_content, "incomplete_buffer_for_content"},
                                  {sip2jsonErrors::incomplete_buffer_for_header, "incomplete_buffer_for_header"},
                                  {sip2jsonErrors::invalid_startline, "invalid_startline"},
                                  {sip2jsonErrors::unsupported_contenttype, "unsupported_contenttype"},
                                  {sip2jsonErrors::missing_required_element, "missing_required_element"},
                                  {sip2jsonErrors::invalid_document, "invalid_document"},
                                  {sip2jsonErrors::invalid_document_unsupported_method, "invalid_document_unsupported_method"},
                                  {sip2jsonErrors::invalid_document_unsupported_content, "invalid_document_unsupported_content"},
                                  {sip2jsonErrors::empty_message, "empty_message"},
                                  {sip2jsonErrors::unknown, "unknown"}});

    class sip2json_exception : public std::runtime_error
    {
    public:
        sip2jsonErrors errCode = sip2jsonErrors::unknown;

        sip2json_exception(const std::string& msg)
            : std::runtime_error(msg)
        {
        }

        sip2json_exception(const std::exception& e)
            : std::runtime_error(e.what())
        {
        }
    };

    class missing_required_element : public sip2json_exception
    {
    public:
        missing_required_element(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::missing_required_element;
        }
    };


    class incomplete_buffer_for_parse_error : public sip2json_exception
    {
    public:
        incomplete_buffer_for_parse_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::incomplete_buffer_for_parse;
        }
    };


    class incomplete_buffer_for_content_error : public sip2json_exception
    {
    public:
        incomplete_buffer_for_content_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::incomplete_buffer_for_content;
        }
    };


    class incomplete_buffer_for_header_error : public sip2json_exception
    {
    public:
        incomplete_buffer_for_header_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::incomplete_buffer_for_header;
        }
    };


    class invalid_startline_error : public sip2json_exception
    {
    public:
        invalid_startline_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::invalid_startline;
        }
    };


    class unsupported_contenttype_error : public sip2json_exception
    {
    public:
        unsupported_contenttype_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::unsupported_contenttype;
        }
    };


    class invalid_document_error : public sip2json_exception
    {
    public:
        invalid_document_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::invalid_document;
        }
    };


    class empty_message_error : public sip2json_exception
    {
    public:
        empty_message_error(const std::string& msg)
            : sip2json_exception(msg)
        {
            errCode = sip2jsonErrors::empty_message;
        }
    };


    /// @brief Create and throw a sip2json_error object.
    /// @tparam ...Args Automatically deduced template argument
    /// @param ec Error Code (type of error to be created)
    /// @param formatSpec std::format spec
    /// @param ...args std::format arguments
    /// @return Throws an object sip2json_error object.
    template <class E, typename... Args> void sip2json_throw(const std::string& formatSpec, Args... args) noexcept(false)
    {
        auto e = E(std::format(formatSpec, args...));
        throw e;
    }


    template <class E, typename... Args>
    void sip2json_throw_if(bool predicate, const std::string& formatSpec, Args... args) noexcept(false)
    {
        if (predicate)
        {
            auto e = E(std::format(formatSpec, args...));
            throw e;
        }
    }

} // namespace siddiqsoft


template <> struct std::formatter<siddiqsoft::sip2jsonErrors> : std::formatter<std::string>
{
    auto format(siddiqsoft::sip2jsonErrors e, std::format_context& ctx)
    {
        switch (e)
        {
        case siddiqsoft::sip2jsonErrors::ok: return std::formatter<std::string>::format("ok", ctx);
        case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse:
            return std::formatter<std::string>::format("incomplete_buffer_for_parse", ctx);
        case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content:
            return std::formatter<std::string>::format("incomplete_buffer_for_content", ctx);
        case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header:
            return std::formatter<std::string>::format("incomplete_buffer_for_header", ctx);
        case siddiqsoft::sip2jsonErrors::invalid_startline: return std::formatter<std::string>::format("invalid_startline", ctx);
        case siddiqsoft::sip2jsonErrors::unsupported_contenttype:
            return std::formatter<std::string>::format("unsupported_contenttype", ctx);
        case siddiqsoft::sip2jsonErrors::missing_required_element:
            return std::formatter<std::string>::format("missing_required_element", ctx);
        case siddiqsoft::sip2jsonErrors::invalid_document: return std::formatter<std::string>::format("invalid_document", ctx);
        case siddiqsoft::sip2jsonErrors::invalid_document_unsupported_method:
            return std::formatter<std::string>::format("invalid_document_unsupported_method", ctx);
        case siddiqsoft::sip2jsonErrors::invalid_document_unsupported_content:
            return std::formatter<std::string>::format("invalid_document_unsupported_content", ctx);
        case siddiqsoft::sip2jsonErrors::empty_message: return std::formatter<std::string>::format("empty_message", ctx);
        }

        return std::formatter<std::string>::format("unknown", ctx);
    }
};
