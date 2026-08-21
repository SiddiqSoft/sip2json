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
#include "private/sip2json_header_keys.hpp"


namespace siddiqsoft
{
    /// @brief Enumeration representing the type of SIP message.
    /// @details Distinguishes between SIP request messages and SIP response messages.
    enum class SIPMessageType
    {
        notspecified, ///< Message type not specified or unknown
        request  = 1, ///< SIP request message (e.g., INVITE, REGISTER, BYE)
        response = 2  ///< SIP response message (e.g., 200 OK, 404 Not Found)
    };


    NLOHMANN_JSON_SERIALIZE_ENUM(SIPMessageType,
                                 {{SIPMessageType::request, "request"},
                                  {SIPMessageType::response, "response"},
                                  {SIPMessageType::notspecified, "notspecified"}});


    /// @brief Represents a SIP message with JSON serialization support.
    /// @details The sipmessage class extends nlohmann::json to provide a structured representation
    /// of SIP (Session Initiation Protocol) messages. It supports both request and response messages,
    /// with automatic metadata tracking, header management, and body content handling.
    /// The internal structure uses abbreviated keys: "s" for status/start line, "h" for headers,
    /// "b" for body, and "meta" for metadata.
    class sipmessage : public nlohmann::json
    {
        static const inline std::string MetaLibName {"sip2json"};      ///< Library name for metadata
        static const inline std::string MetaSchemaVersion {"1.0.2"};   ///< Schema version for metadata
        static const inline std::string MetaParserVersion {"2.4"};    ///< Parser version for metadata

    public:
        /// @brief Default constructor initializing an empty SIP message with metadata.
        /// @details Creates a new sipmessage with default metadata including version, timestamp, and TTX counter.
        sipmessage() : nlohmann::json({
            {JSON_KEY_META, {
                {JSON_KEY_VERSION, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                {JSON_KEY_TIME, TimeAsISO8601()},
                {JSON_KEY_TTX, 0}
            }}
        }) {}

        /// @brief Copy constructor from nlohmann::json object.
        /// @param src The source JSON object to copy from.
        explicit sipmessage(const nlohmann::json& src) : nlohmann::json(src) {}

        /// @brief Move constructor from nlohmann::json object.
        /// @param src The source JSON object to move from.
        explicit sipmessage(nlohmann::json&& src) noexcept : nlohmann::json(std::move(src)) {}

        /// @brief Move constructor.
        sipmessage(sipmessage&&) noexcept = default;

        /// @brief Copy constructor from another sipmessage.
        /// @param src The source sipmessage to copy from.
        sipmessage(const sipmessage& src) : nlohmann::json(static_cast<const nlohmann::json&>(src)) {}

        /// @brief Copy assignment operator from another sipmessage.
        /// @param src The source sipmessage to copy from.
        /// @return Reference to this sipmessage.
        sipmessage& operator=(const sipmessage& src)
        {
            nlohmann::json::operator=(static_cast<const nlohmann::json&>(src));
            return *this;
        }

        /// @brief Copy assignment operator from nlohmann::json.
        /// @param src The source JSON object to copy from.
        /// @return Reference to this sipmessage.
        sipmessage& operator=(const nlohmann::json& src)
        {
            nlohmann::json::operator=(src);
            return *this;
        }

        /// @brief Move assignment operator.
        sipmessage& operator=(sipmessage&& src) noexcept = default;

        /// @brief Move assignment operator from nlohmann::json.
        /// @param src The source JSON object to move from.
        /// @return Reference to this sipmessage.
        sipmessage& operator=(nlohmann::json&& src) noexcept
        {
            nlohmann::json::operator=(std::move(src));
            return *this;
        }

        /// @brief Destructor.
        ~sipmessage() = default;


        /// @brief Instantiates request message given method and uri with option callId and cseq
        /// @param method One of the supported SIP methods
        /// @param uri Request URI
        /// @param callId Optional CallId
        /// @param cseq Optional Cseq; the string value is build using this parameter and the method
        /// @brief Instantiates request message given method and uri with option callId and cseq
        /// @param method One of the supported SIP methods
        /// @param uri Request URI
        /// @param callId Optional CallId
        /// @param cseq Optional Cseq; the string value is build using this parameter and the method
        sipmessage(const std::string& method, const std::string& uri, const std::string& callId = {}, uint32_t cseq = 0)
        {
            using namespace std;

            update({{JSON_KEY_STARTLINE, {{JSON_KEY_TYPE, SIPMessageType::request}, {JSON_KEY_METHOD, method}, {JSON_KEY_URI, uri}, {JSON_KEY_VERSION, SIPVER_20}}},
                    {JSON_KEY_BODY, nullptr},
                    {JSON_KEY_META,
                     {{JSON_KEY_VERSION, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {JSON_KEY_TIME, TimeAsISO8601()},
                      {JSON_KEY_TTX, 0}}},
                    {JSON_KEY_HEADERS,
                     {{HF_USER_AGENT, std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {HF_DATE, TimeAsRFC1123()}}}});

            // request-line: METHOD Request-URI SIP/2.0
            // message-headers
            if (!callId.empty()) setHeader(HF_CALLID, callId);
            if (cseq > 0) setHeader(HF_CSEQ, std::format("{} {}", cseq, method));
        }

        /// @brief Instantiates a response message from scratch or optionally from existing sipmessage request
        /// @param statusCode Status Code for this message, the reason is built using map
        /// @param src Optional sipmessage object of type request
        sipmessage(uint32_t statusCode, const sipmessage& src)
        {
            using namespace std;

            update(src);

            // Overwrite the source object's values
            (*this)[JSON_KEY_META] = {{JSON_KEY_VERSION, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                                {JSON_KEY_TIME, TimeAsISO8601()},
                                {JSON_KEY_TTX, 0}};

            // "status-line" (Status Reason Version)
            (*this)[JSON_KEY_STARTLINE] = {
                    {JSON_KEY_TYPE, SIPMessageType::response}, {JSON_KEY_STATUS, statusCode}, {JSON_KEY_REASON, getReasonPhrase(statusCode)}, {JSON_KEY_VERSION, SIPVER_20}};

            (*this)[JSON_KEY_HEADERS][HF_USER_AGENT] = std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion);
            setHeader(HF_DATE, TimeAsRFC1123());
        }


        /// @brief Instantiates a response message from scratch or optionally from existing sipmessage request
        /// @param statusCode Status Code for this message, the reason is built using map
        /// @param src Optional sipmessage object of type request
        sipmessage(uint32_t statusCode)
        {
            using namespace std;

            update(nlohmann::json {
                    {JSON_KEY_STARTLINE,
                     {{JSON_KEY_TYPE, SIPMessageType::response},
                      {JSON_KEY_STATUS, statusCode},
                      {JSON_KEY_REASON, getReasonPhrase(statusCode)},
                      {JSON_KEY_VERSION, SIPVER_20}}},
                    {JSON_KEY_BODY, nullptr},
                    {JSON_KEY_META,
                     {{JSON_KEY_VERSION, std::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {JSON_KEY_TIME, TimeAsISO8601()},
                      {JSON_KEY_TTX, 0}}},
                    {JSON_KEY_HEADERS,
                     {{HF_USER_AGENT, std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion)},
                      {HF_DATE, TimeAsRFC1123()}}}});
        }


    public:
        /// @brief Returns the header object reference.
        /// @details Provides direct access to the headers section ("h") of the SIP message.
        /// @return Reference to the header object.
        /// @return Reference to the header object.
        auto& headers() { return this->at(JSON_KEY_HEADERS); }

        /// @brief Returns the header object const reference.
        /// @details Provides const access to the headers section ("h") of the SIP message.
        /// @return Const reference to the header object.
        const auto& headers() const { return this->at(JSON_KEY_HEADERS); }

        /// @brief Retrieves a header value by key with optional default value.
        /// @tparam T The type of the header value to retrieve.
        /// @param key The header name to look up.
        /// @param defaultValue Optional default value if the header is not found.
        /// @return The header value or the default value if not found.
        template <class T> auto getHeader(const std::string& key, std::optional<T> defaultValue = {}) const
        {
            return (*this)[JSON_KEY_HEADERS].value(key, defaultValue.value_or(T {}));
        }

        /// @brief Retrieves a header value by key with optional default value.
        /// @tparam T The type of the header value to retrieve.
        /// @param key The header name to look up.
        /// @param defaultValue Optional default value if the header is not found.
        /// @return The header value or the default value if not found.
        template <class T> auto getHeader(const char* key, std::optional<T> defaultValue = {}) const
        {
            return (*this)[JSON_KEY_HEADERS].value(key, defaultValue.value_or(T {}));
        }

        /// @brief Retrieves a header value by key with optional default value.
        /// @tparam T The type of the header value to retrieve.
        /// @param key The header name to look up.
        /// @param defaultValue Optional default value if the header is not found.
        /// @return The header value or the default value if not found.
        template <class T> auto getHeader(std::string_view key, std::optional<T> defaultValue = {}) const
        {
            return (*this)[JSON_KEY_HEADERS].value(std::string {key}, defaultValue.value_or(T {}));
        }

        /// @brief Sets or updates the User-Agent header.
        /// @details Automatically formats the User-Agent header with library name, version, and schema information.
        /// @param ua Optional additional user agent string to append.
        /// @return Reference to this sipmessage for method chaining.
        auto& setUserAgent(const std::string& ua = {})
        {
            if (!ua.empty())
                setHeader(HF_USER_AGENT, std::format("{}/{} (schema:{}) {}", MetaLibName, MetaParserVersion, MetaSchemaVersion, ua));
            else
                setHeader(HF_USER_AGENT, std::format("{}/{} (schema:{})", MetaLibName, MetaParserVersion, MetaSchemaVersion));
            return *this;
        }

        /// @brief Retrieves the User-Agent header value.
        /// @return The User-Agent header string.
        auto getUserAgent() const { return getHeader<std::string>(HF_USER_AGENT); }

        /// @brief Retrieves the Content-Length header value.
        /// @return The Content-Length as a 32-bit unsigned integer.
        uint32_t getContentLength() const { return getHeader<uint32_t>(HF_CONTENT_LENGTH); }

        /// @brief Retrieves the Expires header value.
        /// @return The Expires value as a 32-bit unsigned integer.
        uint32_t getExpires() const { return getHeader<uint32_t>(HF_EXPIRES); }

        /// @brief Retrieves the Content-Type header value.
        /// @details Handles case-insensitive lookup for Content-Type and Content-type headers
        /// to accommodate non-compliant SIP servers.
        /// @return The Content-Type header string, or empty string if not found.
        auto getContentType() const
        {
            if (headers().contains(HF_CONTENT_TYPE))
            {
                auto ct = headers().at(HF_CONTENT_TYPE);
                return ct.is_null() ? std::string {} : ct.get<std::string>();
            }
            else if (headers().contains("Content-type"))
            {
                auto ct = headers().at("Content-type");
                return ct.is_null() ? std::string {} : ct.get<std::string>();
            }
            else if (headers().contains("content-type"))
            {
                auto ct = headers().at("content-type");
                return ct.is_null() ? std::string {} : ct.get<std::string>();
            }
            else if (headers().contains("c"))
            {
                auto ct = headers().at("c");
                return ct.is_null() ? std::string {} : ct.get<std::string>();
            }

            return std::string {};
        }

        /// @brief Retrieves the Call-ID header value.
        /// @return The Call-ID header string.
        auto getCallID() const { return getHeader<std::string>(HF_CALLID); }

        /// @brief Retrieves the SIP method from a request message.
        /// @details Only applicable to request messages (e.g., INVITE, REGISTER, BYE).
        /// @return The SIP method string, or empty string if not a request.
        auto getMethod() const { return this->value("/s/method"_json_pointer, ""); }

        /// @brief Retrieves the Request-URI from a request message.
        /// @details Only applicable to request messages.
        /// @return The Request-URI string, or empty string if not a request.
        auto getUri() const { return this->value("/s/uri"_json_pointer, ""); }

        /// @brief Zero-copy view accessor for SIP method.
        /// @return std::string_view pointing to internal string storage.
        std::string_view getMethodView() const
        {
            static const auto ptr = "/s/method"_json_pointer;
            if (this->contains(ptr))
            {
                const auto& v = (*this)[ptr];
                if (v.is_string()) return v.get_ref<const std::string&>();
            }
            return {};
        }

        /// @brief Zero-copy view accessor for Request-URI.
        /// @return std::string_view pointing to internal string storage.
        std::string_view getUriView() const
        {
            static const auto ptr = "/s/uri"_json_pointer;
            if (this->contains(ptr))
            {
                const auto& v = (*this)[ptr];
                if (v.is_string()) return v.get_ref<const std::string&>();
            }
            return {};
        }

        /// @brief Zero-copy view accessor for Reason phrase.
        /// @return std::string_view pointing to internal string storage.
        std::string_view getReasonView() const
        {
            static const auto ptr = "/s/reason"_json_pointer;
            if (this->contains(ptr))
            {
                const auto& v = (*this)[ptr];
                if (v.is_string()) return v.get_ref<const std::string&>();
            }
            return {};
        }

        /// @brief Zero-copy view accessor for Call-ID header.
        /// @return std::string_view pointing to internal string storage.
        std::string_view getCallIDView() const
        {
            if (headers().contains(HFS_CALLID[1]))
            {
                const auto& cid = headers().at(HFS_CALLID[1]);
                if (cid.is_string()) return cid.get_ref<const std::string&>();
            }
            return {};
        }

        /// @brief Retrieves the status code from a response message.
        /// @details Only applicable to response messages (e.g., 200, 404, 500).
        /// @return The status code as a 32-bit unsigned integer, or 0 if not a response.
        auto getStatusCode() const { return this->value("/s/status"_json_pointer, 0); }

        /// @brief Retrieves the reason phrase from a response message.
        /// @details Only applicable to response messages (e.g., "OK", "Not Found").
        /// @return The reason phrase string, or empty string if not a response.
        auto getReason() const { return this->value("/s/reason"_json_pointer, ""); }

        /// @brief Returns a reference to the body object.
        /// @details Provides direct access to the body section ("b") of the SIP message.
        /// This method should be used to change the body contents to text/plain or non-SDP content-type.
        /// @return Reference to the body element.
        /// @throws std::out_of_range if the body element does not exist.
        auto& body() { return this->at(JSON_KEY_BODY); }

        /// @brief Returns a const reference to the body object.
        /// @details Provides const access to the body section ("b") of the SIP message.
        /// @return Const reference to the body element.
        /// @throws std::out_of_range if the body element does not exist.
        const auto& body() const { return this->at(JSON_KEY_BODY); }

        /// @brief Checks if the message contains a body element.
        /// @details Verifies the presence of the "b" (body) element in the SIP message.
        /// @return True if the sipmessage contains the body element, false otherwise.
        bool hasBody() const { return this->contains(JSON_KEY_BODY); }

        /// @brief Retrieves a body element using a JSON pointer with a default value.
        /// @details Accesses nested elements within the body section using JSON pointer notation.
        /// @tparam T The type of the body element to retrieve.
        /// @param jp The JSON pointer path relative to the body (/b).
        /// @param defaultValue The default value to return if the element is not found.
        /// @return The body element value or the default value if not found.
        /// @throws std::out_of_range if the body element does not exist.
        template <typename T> T getBodyElement(const nlohmann::json::json_pointer& jp, const T& defaultValue) const
        {
            return this->at(JSON_KEY_BODY).value<T>(jp, defaultValue);
        }

        /// @brief Checks if this message is a SIP request.
        /// @details Examines the message type field to determine if this is a request message.
        /// @return True if the message type is request, false otherwise.
        bool isMessageRequest() const
        {
            return (this->value("/s/type"_json_pointer, SIPMessageType::notspecified) == SIPMessageType::request);
        }

        /// @brief Checks if this message is a SIP response.
        /// @details Examines the message type field to determine if this is a response message.
        /// @return True if the message type is response, false otherwise.
        bool isMessageResponse() const
        {
            return (this->value("/s/type"_json_pointer, SIPMessageType::notspecified) == SIPMessageType::response);
        }

        // mutators
    public:
        /// @brief Sets a header key-value
        /// @tparam T Type of object; this is typically inferred by the compiler.
        /// @param key The header name
        /// @param v The header value.
        /// @return Self.
        template <typename T> inline sipmessage& setHeader(const std::string& key, const T& v)
        {
            (*this)[JSON_KEY_HEADERS][key] = v;
            return *this;
        };

        /// @brief Sets a header key-value
        /// @tparam T Type of object; this is typically inferred by the compiler.
        /// @param key The header name
        /// @param v The header value.
        /// @return Self.
        template <typename T> inline sipmessage& setHeader(const char* key, const T& v)
        {
            (*this)[JSON_KEY_HEADERS][key] = v;
            return *this;
        };

        /// @brief Sets a header key-value
        /// @tparam T Type of object; this is typically inferred by the compiler.
        /// @param key The header name
        /// @param v The header value.
        /// @return Self.
        template <typename T> inline sipmessage& setHeader(std::string_view key, const T& v)
        {
            (*this)[JSON_KEY_HEADERS][std::string{key}] = v;
            return *this;
        };


        /// @brief Sets the header elements from the source json object which is merge patched
        /// @param arg source json object
        /// @return the sipmessage
        inline sipmessage& setHeader(const nlohmann::json& arg)
        {
            if (!this->contains(JSON_KEY_HEADERS))
                (*this)[JSON_KEY_HEADERS] = arg;
            else
                (*this)[JSON_KEY_HEADERS].merge_patch(arg);
            return *this;
        };


        /// @brief Set element within the body to the given value.
        /// @tparam T Type of object; this is typically inferred by the compiler.
        /// @param key The key within the body section.
        /// @param v The value. Json, string (for text/plain)
        /// @return Self
        template <typename T> inline sipmessage& setBody(const nlohmann::json::json_pointer& key, const T& v)
        {
            (*this)[JSON_KEY_BODY][key] = v;
            return *this;
        };


        /// @brief Sets the sip elements from the source json object which is updated. Previous keys are replaced!
        /// @param arg source json object must be /sdp/0/...
        /// @return the sipmessage
        inline sipmessage& setBody(const nlohmann::json& arg)
        {
            if (!this->contains(JSON_KEY_BODY))
                (*this)[JSON_KEY_BODY] = arg;
            else
                (*this).at(JSON_KEY_BODY).update(arg);

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
    case siddiqsoft::sip2jsonErrors::ok: os << "ok"; break;
    case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse: os << "incomplete_buffer_for_parse"; break;
    case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content: os << "incomplete_buffer_for_content"; break;
    case siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header: os << "incomplete_buffer_for_header"; break;
    case siddiqsoft::sip2jsonErrors::invalid_startline: os << "invalid_startline"; break;
    case siddiqsoft::sip2jsonErrors::unsupported_contenttype: os << "unsupported_contenttype"; break;
    case siddiqsoft::sip2jsonErrors::missing_required_element: os << "missing_required_element"; break;
    case siddiqsoft::sip2jsonErrors::invalid_document: os << "invalid_document"; break;
    case siddiqsoft::sip2jsonErrors::invalid_document_unsupported_method: os << "invalid_document_unsupported_method"; break;
    case siddiqsoft::sip2jsonErrors::invalid_document_unsupported_content: os << "invalid_document_unsupported_content"; break;
    case siddiqsoft::sip2jsonErrors::empty_message: os << "empty_message"; break;
    default: os << "unknown"; break;
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
