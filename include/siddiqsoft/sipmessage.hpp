/*
    A SIP Parser for Modern C++
    Version 1.15.0
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License

    Copyright (c) 2003-2024, Abdelkareem Siddiq
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
#ifndef SIPMESSAGE_HPP
#define SIPMESSAGE_HPP

#include <algorithm>
#include <string>
#include <variant>
#include <regex>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>

#include <format>
#include "nlohmann/json.hpp"

#include "private/sip2json_response_codes.hpp"
#include "private/sip2json_utils.hpp"
#include "private/sip2json_exception.hpp"


namespace siddiqsoft
{
    enum class SIPMessageType
    {
        notspecified,
        request  = 1,
        response = 2
    };


    NLOHMANN_JSON_SERIALIZE_ENUM(SIPMessageType,
                                 {{SIPMessageType::request, "request"},
                                  {SIPMessageType::response, "response"},
                                  {SIPMessageType::notspecified, "notspecified"}});


    class sipmessage : public nlohmann::json
    {
        static constexpr std::string_view MetaLibName {"sip2json"};
        static constexpr std::string_view MetaSchemaVersion {"1.0.2"};
        static constexpr std::string_view MetaParserVersion {"1.15"};

    public:
        sipmessage()
        {
            using namespace std;
            // Overwrite the source object's values
            (*this)["meta"s] = {{"version"s, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                                {"time"s, TimeAsISO8601()},
                                {"ttx"s, 0}};
        }

        sipmessage(const nlohmann::json& src) { this->update(src); }
        sipmessage(nlohmann::json&& src) { nlohmann::json((*this)).operator=(std::move(src)); }

        sipmessage(sipmessage&&) = default;
        sipmessage(const sipmessage& src) { this->update(nlohmann::json(src)); }

        sipmessage& operator=(sipmessage&& src) = default;
        sipmessage& operator=(nlohmann::json&& src)
        {
            nlohmann::json((*this)).operator=(std::move(src));
            return *this;
        }


        explicit operator nlohmann::json&() { return static_cast<nlohmann::json&>(*this); }


        /// @brief Instantiates request message given method and uri with option callId and cseq
        /// @param method One of the supported SIP methods
        /// @param uri Request URI
        /// @param callId Optional CallId
        /// @param cseq Optional Cseq; the string value is build using this parameter and the method
        sipmessage(const std::string& method, const std::string& uri, const std::string& callId = {}, uint32_t cseq = 0)
        {
            using namespace std;

            update({{"s"s, {{"type"s, SIPMessageType::request}, {"method"s, method}, {"uri"s, uri}, {"version"s, SIPVER_20}}},
                    {"b"s, nullptr},
                    {"meta"s,
                     {{"version"s, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {"time"s, TimeAsISO8601()},
                      {"ttx"s, 0}}},
                    {"h"s,
                     {{"User-Agent"s, std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {"Date"s, TimeAsRFC1123()}}}});

            // request-line: METHOD Request-URI SIP/2.0
            // message-headers
            if (!callId.empty()) setHeader("Call-ID"s, callId);
            if (cseq > 0) setHeader("CSeq"s, std::format("{} {}", cseq, method));
        }

        /// @brief Instantiates a response message from scratch or optionally from existing sipmessage request
        /// @param statusCode Status Code for this message, the reason is built using map
        /// @param src Optional sipmessage object of type request
        sipmessage(uint32_t statusCode, const sipmessage& src)
        {
            using namespace std;

            update(src);

            // Overwrite the source object's values
            (*this)["meta"s] = {{"version"s, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                                {"time"s, TimeAsISO8601()},
                                {"ttx"s, 0}};

            //// We must clear these values in case we are updating an existing object.
            //erase("s"s);
            // "status-line" (Status Reason Version)
            (*this)["s"s] = {
                    {"type", "response"}, {"status", statusCode}, {"reason", getReasonPhrase(statusCode)}, {"version", SIPVER_20}};

            (*this)["h"s]["User-Agent"s] = std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion);
            setHeader("Date"s, TimeAsRFC1123());
        }


        /// @brief Instantiates a response message from scratch or optionally from existing sipmessage request
        /// @param statusCode Status Code for this message, the reason is built using map
        /// @param src Optional sipmessage object of type request
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
                     {{"version"s, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {"time"s, TimeAsISO8601()},
                      {"ttx"s, 0}}},
                    {"h"s,
                     {{"User-Agent"s, std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {"Date"s, TimeAsRFC1123()}}}});
        }


    public:
        /// @brief Returns the header object reference
        /// @return Return the header object reference
        inline auto&            headers() { return this->at("h"); };
        template <class T> auto getHeader(const std::string& key, std::optional<T> defaultValue = {})
        {
            // Return the value or the default for the object.
            return (*this)["h"].value(key, defaultValue.value_or(T {}));
        };
        inline auto& setUserAgent(const std::string& ua)
        {
            if (!ua.empty())
                setHeader("User-Agent", std::format("{}/{} (schema:{}) {}", MetaLibName, MetaParserVersion, MetaSchemaVersion, ua));
            else
                setHeader("User-Agent", std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion));
            return *this;
        };
        inline auto     getUserAgent() { return getHeader<std::string>("User-Agent"); };
        inline uint32_t getContentLength() { return getHeader<uint32_t>("Content-Length"); };
        inline uint32_t getExpires() { return getHeader<uint32_t>("Expires"); };
        inline auto     getContentType()
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
        inline auto& body() noexcept(false) { return this->at("b"); };


        /// @brief Checks if we have the "b" body element
        /// @return True if the sipmessage contains the body element
        inline bool hasBody() { return this->contains("b"); }


        /// @brief Get body element (relative to /b). Throws if body does not exist.
        /// @tparam T Type
        /// @param jp The key as json_pointer
        /// @param defaultValue The default value
        /// @return The item found or the default value.
        template <typename T> T getBodyElement(const nlohmann::json::json_pointer& jp, const T& defaultValue)
        {
            return this->at("b").value<T>(jp, defaultValue);
        };

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


        /// @brief Sets the header elements from the source json object which is merge patched
        /// @param arg source json object
        /// @return the sipmessage
        inline sipmessage& setHeader(const nlohmann::json& arg)
        {
            if (!this->contains("h"))
                (*this)["h"] = arg;
            else
                (*this)["h"].merge_patch(arg);
            return *this;
        };


        /// @brief Set element within the body to the given value.
        /// @tparam T Type of object; this is typically inferred by the compiler.
        /// @param key The key within the body section.
        /// @param v The value. Json, string (for text/plain)
        /// @return Self
        template <typename T> inline sipmessage& setBody(const nlohmann::json::json_pointer& key, const T& v)
        {
            (*this)["b"][key] = v;
            return *this;
        };


        /// @brief Sets the sip elements from the source json object which is updated. Previous keys are replaced!
        /// @param arg source json object must be /sdp/0/...
        /// @return the sipmessage
        inline sipmessage& setBody(const nlohmann::json& arg)
        {
            if (!this->contains("b"))
                (*this)["b"] = arg;
            else
                (*this).at("b").update(arg);

            return *this;
        };

    }; // class sipmessage
} // namespace siddiqsoft


static std::ostream& operator<<(std::ostream& os, const siddiqsoft::SIPMessageType& mt)
{
    switch (mt)
    {
    case siddiqsoft::SIPMessageType::request: os << "request"; break;
    case siddiqsoft::SIPMessageType::response: os << "response"; break;
    default: os << "unknown";
    }

    return os;
}

static std::ostream& operator<<(std::ostream& os, const siddiqsoft::sip2jsonErrors& errs)
{
    switch (errs)
    {
    case siddiqsoft::sip2jsonErrors::ok: os << "ok";
    case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse: os << "incomplete_buffer_for_parse";
    case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content: os << "incomplete_buffer_for_content";
    case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header: os << "incomplete_buffer_for_header";
    case siddiqsoft::sip2jsonErrors::invalid_startline: os << "invalid_startline";
    case siddiqsoft::sip2jsonErrors::unsupported_contenttype: os << "unsupported_contenttype";
    case siddiqsoft::sip2jsonErrors::missing_required_element: os << "missing_required_element";
    case siddiqsoft::sip2jsonErrors::invalid_document: os << "invalid_document";
    case siddiqsoft::sip2jsonErrors::invalid_document_unsupported_method: os << "invalid_document_unsupported_method";
    case siddiqsoft::sip2jsonErrors::invalid_document_unsupported_content: os << "invalid_document_unsupported_content";
    case siddiqsoft::sip2jsonErrors::empty_message: os << "empty_message";
    default: os << "unknown";
    }

    return os;
}


template <> struct std::formatter<siddiqsoft::SIPMessageType> : std::formatter<std::string>
{
    auto format(const siddiqsoft::SIPMessageType& mt, std::format_context& ctx) const
    {
        if (mt == siddiqsoft::SIPMessageType::request)
            return std::formatter<std::string>::format("request", ctx);
        else if (mt == siddiqsoft::SIPMessageType::response)
            return std::formatter<std::string>::format("response", ctx);
        return std::formatter<std::string>::format("unknown", ctx);
    }
};


template <> struct std::formatter<siddiqsoft::sipmessage> : std::formatter<std::string>
{
    auto format(const siddiqsoft::sipmessage& msg, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(msg.dump(), ctx);
    }
};

#endif
