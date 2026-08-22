/*
    A SIP Parser for Modern C++: Header Key Sets and Canonicalization
    Version 2.5.x
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

#ifndef SIP2JSON_HEADER_KEYS_HPP
#define SIP2JSON_HEADER_KEYS_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace siddiqsoft
{

    /// @brief Computes 64-bit FNV-1a hash with inline case-folding over input string.
    constexpr uint64_t hash_header_key(const char* s, size_t len) noexcept
    {
        uint64_t h = 14695981039346656037ULL;
        for (size_t i = 0; i < len; ++i)
        {
            char c = s[i];
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
            h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
            h *= 1099511628211ULL;
        }
        return h;
    }

    constexpr uint64_t hash_header_key(std::string_view sv) noexcept
    { return hash_header_key(sv.data(), sv.size()); }

    /// @brief The HeaderKeySet class expresses a set of defaults for the SIP headers.
    /// It also allows for custom headers to be defined and used in the parser.
    class HeaderKeySet
    {
    private:
        std::string m_lowercase {};
        std::string m_canonical {};
        std::string m_abbreviation {};

    public:
        uint64_t hashValue {0};
        bool     isCanonical {false};
        bool     isMultiLine {false};
        bool     isCustom {false};

        constexpr HeaderKeySet() = default;

        HeaderKeySet(uint64_t           hashVal,
                     const std::string& lower,
                     const std::string& canon,
                     const std::string& abbrev    = {},
                     bool               canonFlag = true,
                     bool               multiFlag = false)
            : m_lowercase(lower)
            , m_canonical(canon)
            , m_abbreviation(abbrev)
            , hashValue(hashVal)
            , isCanonical(canonFlag)
            , isMultiLine(multiFlag)
            , isCustom(false)
        {
        }

        HeaderKeySet(const std::string& customKey)
            : m_lowercase(customKey)
            , m_canonical(customKey)
            , m_abbreviation({})
            , hashValue(0) // no hash value for custom headers, as they are not predefined
            , isCanonical(false)
            , isMultiLine(false)
            , isCustom(true)
        {
        }

        [[nodiscard]] inline const std::string& canonical() const { return m_canonical; }
        [[nodiscard]] inline const std::string& lower() const { return m_lowercase; }
        [[nodiscard]] inline const std::string& alt() const { return m_abbreviation; }
        [[nodiscard]] inline uint64_t           hash() const { return hashValue; }
    };

    // Static const std::string definitions for canonical header keys to prevent temporary std::string allocations in nlohmann::json lookups
    static inline const std::string HF_FROM {"From"};
    static inline const std::string HF_TO {"To"};
    static inline const std::string HF_PRIORITY {"Priority"};
    static inline const std::string HF_CONTENT_ENCODING {"Content-Encoding"};
    static inline const std::string HF_CONTENT_LENGTH {"Content-Length"};
    static inline const std::string HF_CONTENT_TYPE {"Content-Type"};
    static inline const std::string HF_CALLID {"Call-ID"};
    static inline const std::string HF_CSEQ {"CSeq"};
    static inline const std::string HF_VIA {"Via"};
    static inline const std::string HF_ENCRYPTION {"Encryption"};
    static inline const std::string HF_SUBJECT {"Subject"};
    static inline const std::string HF_LOCATION {"Location"};
    static inline const std::string HF_EXPIRES {"Expires"};
    static inline const std::string HF_CONTACT {"Contact"};
    static inline const std::string HF_ACCEPT {"Accept"};
    static inline const std::string HF_ACCEPT_ENCODING {"Accept-Encoding"};
    static inline const std::string HF_ACCEPT_LANGUAGE {"Accept-Language"};
    static inline const std::string HF_DATE {"Date"};
    static inline const std::string HF_RECORD_ROUTE {"Record-Route"};
    static inline const std::string HF_TIMESTAMP {"Timestamp"};
    static inline const std::string HF_HIDE {"Hide"};
    static inline const std::string HF_MAX_FORWARDS {"Max-Forwards"};
    static inline const std::string HF_ORGANIZATION {"Organization"};
    static inline const std::string HF_PROXY_AUTHORIZATION {"Proxy-Authorization"};
    static inline const std::string HF_PROXY_REQUIRE {"Proxy-Require"};
    static inline const std::string HF_ROUTE {"Route"};
    static inline const std::string HF_REQUIRE {"Require"};
    static inline const std::string HF_RESPONSE_KEY {"Response-Key"};
    static inline const std::string HF_USER_AGENT {"User-Agent"};
    static inline const std::string HF_PROXY_AUTHENTICATE {"Proxy-Authenticate"};
    static inline const std::string HF_RETRY_AFTER {"Retry-After"};
    static inline const std::string HF_SERVER {"Server"};
    static inline const std::string HF_SUPPORTED {"Supported"};
    static inline const std::string HF_ALLOW {"Allow"};
    static inline const std::string HF_UNSUPPORTED {"Unsupported"};
    static inline const std::string HF_WARNING {"Warning"};
    static inline const std::string HF_WWW_AUTHENTICATE {"WWW-Authenticate"};
    static inline const std::string HF_AUTHORIZATION {"Authorization"};
    static inline const std::string HF_SUBSCRIPTION_STATE {"Subscription-State"};

    // This allows us to compare incoming header keys against a set of known variations, making the parser more robust and flexible.
    static const HeaderKeySet HFS_FROM {hash_header_key("from"), "from", HF_FROM, "f", true, false};
    static const HeaderKeySet HFS_TO {hash_header_key("to"), "to", HF_TO, "t", true, false};
    static const HeaderKeySet HFS_PRIORITY {hash_header_key("priority"), "priority", HF_PRIORITY, {}, true, false};
    static const HeaderKeySet HFS_CONTENT_ENCODING {hash_header_key("content-encoding"),
                                                    "content-encoding",
                                                    HF_CONTENT_ENCODING,
                                                    "e",
                                                    true,
                                                    false};
    static const HeaderKeySet HFS_CONTENT_LENGTH {hash_header_key("content-length"),
                                                  "content-length",
                                                  HF_CONTENT_LENGTH,
                                                  "l",
                                                  true,
                                                  false};
    static const HeaderKeySet HFS_CONTENT_TYPE {hash_header_key("content-type"), "content-type", HF_CONTENT_TYPE, "c", true, false};
    static const HeaderKeySet HFS_CALLID {hash_header_key("call-id"), "call-id", HF_CALLID, "i", true, false};
    static const HeaderKeySet HFS_CSEQ {hash_header_key("cseq"), "cseq", HF_CSEQ, {}, true, false};
    static const HeaderKeySet HFS_VIA {hash_header_key("via"), "via", HF_VIA, "v", true, true};
    static const HeaderKeySet HFS_ENCRYPTION {hash_header_key("encryption"), "encryption", HF_ENCRYPTION, {}, true, false};
    static const HeaderKeySet HFS_SUBJECT {hash_header_key("subject"), "subject", HF_SUBJECT, "s", true, false};
    static const HeaderKeySet HFS_LOCATION {hash_header_key("location"), "location", HF_LOCATION, {}, true, false};
    static const HeaderKeySet HFS_EXPIRES {hash_header_key("expires"), "expires", HF_EXPIRES, {}, true, false};
    static const HeaderKeySet HFS_CONTACT {hash_header_key("contact"), "contact", HF_CONTACT, "m", true, false};
    static const HeaderKeySet HFS_ACCEPT {hash_header_key("accept"), "accept", HF_ACCEPT, {}, true, true};
    static const HeaderKeySet HFS_ACCEPT_ENCODING {hash_header_key("accept-encoding"),
                                                   "accept-encoding",
                                                   HF_ACCEPT_ENCODING,
                                                   {},
                                                   true,
                                                   false};
    static const HeaderKeySet HFS_ACCEPT_LANGUAGE {hash_header_key("accept-language"),
                                                   "accept-language",
                                                   HF_ACCEPT_LANGUAGE,
                                                   {},
                                                   true,
                                                   false};
    static const HeaderKeySet HFS_DATE {hash_header_key("date"), "date", HF_DATE, {}, true, false};
    static const HeaderKeySet HFS_RECORD_ROUTE {hash_header_key("record-route"), "record-route", HF_RECORD_ROUTE, {}, true, true};
    static const HeaderKeySet HFS_TIMESTAMP {hash_header_key("timestamp"), "timestamp", HF_TIMESTAMP, {}, true, false};
    static const HeaderKeySet HFS_HIDE {hash_header_key("hide"), "hide", HF_HIDE, {}, true, false};
    static const HeaderKeySet HFS_MAX_FORWARDS {hash_header_key("max-forwards"), "max-forwards", HF_MAX_FORWARDS, {}, true, false};
    static const HeaderKeySet HFS_ORGANIZATION {hash_header_key("organization"), "organization", HF_ORGANIZATION, {}, true, false};
    static const HeaderKeySet HFS_PROXY_AUTHORIZATION {hash_header_key("proxy-authorization"),
                                                       "proxy-authorization",
                                                       HF_PROXY_AUTHORIZATION,
                                                       {},
                                                       true,
                                                       false};
    static const HeaderKeySet HFS_PROXY_REQUIRE {hash_header_key("proxy-require"),
                                                 "proxy-require",
                                                 HF_PROXY_REQUIRE,
                                                 {},
                                                 true,
                                                 false};
    static const HeaderKeySet HFS_ROUTE {hash_header_key("route"), "route", HF_ROUTE, {}, true, true};
    static const HeaderKeySet HFS_REQUIRE {hash_header_key("require"), "require", HF_REQUIRE, {}, true, false};
    static const HeaderKeySet HFS_RESPONSE_KEY {hash_header_key("response-key"), "response-key", HF_RESPONSE_KEY, {}, true, false};
    static const HeaderKeySet HFS_USER_AGENT {hash_header_key("user-agent"), "user-agent", HF_USER_AGENT, {}, true, false};
    static const HeaderKeySet HFS_PROXY_AUTHENTICATE {hash_header_key("proxy-authenticate"),
                                                      "proxy-authenticate",
                                                      HF_PROXY_AUTHENTICATE,
                                                      {},
                                                      true,
                                                      false};
    static const HeaderKeySet HFS_RETRY_AFTER {hash_header_key("retry-after"), "retry-after", HF_RETRY_AFTER, {}, true, false};
    static const HeaderKeySet HFS_SERVER {hash_header_key("server"), "server", HF_SERVER, {}, true, false};
    static const HeaderKeySet HFS_SUPPORTED {hash_header_key("supported"), "supported", HF_SUPPORTED, "k", true, true};
    static const HeaderKeySet HFS_ALLOW {hash_header_key("allow"), "allow", HF_ALLOW, {}, true, false};
    static const HeaderKeySet HFS_UNSUPPORTED {hash_header_key("unsupported"), "unsupported", HF_UNSUPPORTED, {}, true, false};
    static const HeaderKeySet HFS_WARNING {hash_header_key("warning"), "warning", HF_WARNING, {}, true, true};
    static const HeaderKeySet HFS_WWW_AUTHENTICATE {hash_header_key("www-authenticate"),
                                                    "www-authenticate",
                                                    HF_WWW_AUTHENTICATE,
                                                    {},
                                                    true,
                                                    false};
    static const HeaderKeySet HFS_AUTHORIZATION {hash_header_key("authorization"),
                                                 "authorization",
                                                 HF_AUTHORIZATION,
                                                 "uthorization",
                                                 true,
                                                 false};
    static const HeaderKeySet HFS_SUBSCRIPTION_STATE {hash_header_key("subscription-state"),
                                                      "subscription-state",
                                                      HF_SUBSCRIPTION_STATE,
                                                      {},
                                                      true,
                                                      false};
    static const HeaderKeySet HFS_EMPTY {0, "", "", {}, false, false};

    inline const HeaderKeySet& canonicalizeHeaderKey(const std::string& keyFromPayload)
    {
        if (keyFromPayload.empty()) return HFS_EMPTY;

        // Fast-path: Custom headers starting with X- / x- / X_ / x_
        if (keyFromPayload.size() >= 2 && (keyFromPayload[0] == 'X' || keyFromPayload[0] == 'x') &&
            (keyFromPayload[1] == '-' || keyFromPayload[1] == '_'))
        {
            thread_local HeaderKeySet customKey;
            customKey = HeaderKeySet(keyFromPayload);
            return customKey;
        }

        // compute the hash of the header key for fast comparison
        // this approach saves the time needed to convert the header key to lowercase and compare strings directly!
        uint64_t h = hash_header_key(keyFromPayload.data(), keyFromPayload.size());

        // WARNING:
        // DO NOT replace the string literals with constant variables as this will
        // break the constexpr evaluation and the switch statement will not work as intended.
        // the hash_header_key is constexpr, case-insensitive (lowercased).
        switch (h)
        {
        case hash_header_key("from"):
        case hash_header_key("f"): return HFS_FROM;
        case hash_header_key("to"):
        case hash_header_key("t"): return HFS_TO;
        case hash_header_key("priority"): return HFS_PRIORITY;
        case hash_header_key("content-encoding"):
        case hash_header_key("e"): return HFS_CONTENT_ENCODING;
        case hash_header_key("content-length"):
        case hash_header_key("l"): return HFS_CONTENT_LENGTH;
        case hash_header_key("content-type"):
        case hash_header_key("c"): return HFS_CONTENT_TYPE;
        case hash_header_key("call-id"):
        case hash_header_key("i"): return HFS_CALLID;
        case hash_header_key("cseq"): return HFS_CSEQ;
        case hash_header_key("via"):
        case hash_header_key("v"): return HFS_VIA;
        case hash_header_key("encryption"): return HFS_ENCRYPTION;
        case hash_header_key("subject"):
        case hash_header_key("s"): return HFS_SUBJECT;
        case hash_header_key("location"): return HFS_LOCATION;
        case hash_header_key("expires"): return HFS_EXPIRES;
        case hash_header_key("contact"):
        case hash_header_key("m"): return HFS_CONTACT;
        case hash_header_key("accept"): return HFS_ACCEPT;
        case hash_header_key("accept-encoding"): return HFS_ACCEPT_ENCODING;
        case hash_header_key("accept-language"): return HFS_ACCEPT_LANGUAGE;
        case hash_header_key("date"): return HFS_DATE;
        case hash_header_key("record-route"): return HFS_RECORD_ROUTE;
        case hash_header_key("timestamp"): return HFS_TIMESTAMP;
        case hash_header_key("hide"): return HFS_HIDE;
        case hash_header_key("max-forwards"): return HFS_MAX_FORWARDS;
        case hash_header_key("organization"): return HFS_ORGANIZATION;
        case hash_header_key("proxy-authorization"): return HFS_PROXY_AUTHORIZATION;
        case hash_header_key("proxy-require"): return HFS_PROXY_REQUIRE;
        case hash_header_key("route"): return HFS_ROUTE;
        case hash_header_key("require"): return HFS_REQUIRE;
        case hash_header_key("response-key"): return HFS_RESPONSE_KEY;
        case hash_header_key("user-agent"): return HFS_USER_AGENT;
        case hash_header_key("proxy-authenticate"): return HFS_PROXY_AUTHENTICATE;
        case hash_header_key("retry-after"): return HFS_RETRY_AFTER;
        case hash_header_key("server"): return HFS_SERVER;
        case hash_header_key("supported"):
        case hash_header_key("k"): return HFS_SUPPORTED;
        case hash_header_key("allow"): return HFS_ALLOW;
        case hash_header_key("unsupported"): return HFS_UNSUPPORTED;
        case hash_header_key("warning"): return HFS_WARNING;
        case hash_header_key("www-authenticate"): return HFS_WWW_AUTHENTICATE;
        case hash_header_key("authorization"):
        case hash_header_key("uthorization"): return HFS_AUTHORIZATION;
        case hash_header_key("subscription-state"): return HFS_SUBSCRIPTION_STATE;
        } // don't replace the string constants!

        thread_local HeaderKeySet fallbackKey;
        fallbackKey = HeaderKeySet(keyFromPayload);
        return fallbackKey;
    }
} // namespace siddiqsoft

#endif
