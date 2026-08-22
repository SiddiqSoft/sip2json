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

    /// @brief The HeaderKeySet class expresses a set of defaults for the SIP headers.
    /// It also allows for custom headers to be defined and used in the parser.
    class HeaderKeySet
    {
    private:
        std::string m_lowercase {};
        std::string m_canonical {};
        std::string m_abbreviation {};

    public:
        bool isCanonical {false};
        bool isMultiLine {false};
        bool isCustom {false};

        constexpr HeaderKeySet() = default;

        HeaderKeySet(const std::string& lower, const std::string& canon, const std::string& abbrev = {}, bool canonFlag = true, bool multiFlag = false)
            : m_lowercase(lower)
            , m_canonical(canon)
            , m_abbreviation(abbrev)
            , isCanonical(canonFlag)
            , isMultiLine(multiFlag)
            , isCustom(false)
        {
        }

        HeaderKeySet(const std::string& customKey)
            : m_lowercase(customKey)
            , m_canonical(customKey)
            , m_abbreviation({})
            , isCanonical(false)
            , isMultiLine(false)
            , isCustom(true)
        {
        }

        [[nodiscard]] inline const std::string& canonical() const { return m_canonical; }
        [[nodiscard]] inline const std::string& lower() const { return m_lowercase; }
        [[nodiscard]] inline const std::string& alt() const { return m_abbreviation; }
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
    static const HeaderKeySet HFS_FROM {"from", HF_FROM, "f", true, false};
    static const HeaderKeySet HFS_TO {"to", HF_TO, "t", true, false};
    static const HeaderKeySet HFS_PRIORITY {"priority", HF_PRIORITY, {}, true, false};
    static const HeaderKeySet HFS_CONTENT_ENCODING {"content-encoding", HF_CONTENT_ENCODING, "e", true, false};
    static const HeaderKeySet HFS_CONTENT_LENGTH {"content-length", HF_CONTENT_LENGTH, "l", true, false};
    static const HeaderKeySet HFS_CONTENT_TYPE {"content-type", HF_CONTENT_TYPE, "c", true, false};
    static const HeaderKeySet HFS_CALLID {"call-id", HF_CALLID, "i", true, false};
    static const HeaderKeySet HFS_CSEQ {"cseq", HF_CSEQ, {}, true, false};
    static const HeaderKeySet HFS_VIA {"via", HF_VIA, "v", true, true};
    static const HeaderKeySet HFS_ENCRYPTION {"encryption", HF_ENCRYPTION, {}, true, false};
    static const HeaderKeySet HFS_SUBJECT {"subject", HF_SUBJECT, "s", true, false};
    static const HeaderKeySet HFS_LOCATION {"location", HF_LOCATION, {}, true, false};
    static const HeaderKeySet HFS_EXPIRES {"expires", HF_EXPIRES, {}, true, false};
    static const HeaderKeySet HFS_CONTACT {"contact", HF_CONTACT, "m", true, false};
    static const HeaderKeySet HFS_ACCEPT {"accept", HF_ACCEPT, {}, true, true};
    static const HeaderKeySet HFS_ACCEPT_ENCODING {"accept-encoding", HF_ACCEPT_ENCODING, {}, true, false};
    static const HeaderKeySet HFS_ACCEPT_LANGUAGE {"accept-language", HF_ACCEPT_LANGUAGE, {}, true, false};
    static const HeaderKeySet HFS_DATE {"date", HF_DATE, {}, true, false};
    static const HeaderKeySet HFS_RECORD_ROUTE {"record-route", HF_RECORD_ROUTE, {}, true, true};
    static const HeaderKeySet HFS_TIMESTAMP {"timestamp", HF_TIMESTAMP, {}, true, false};
    static const HeaderKeySet HFS_HIDE {"hide", HF_HIDE, {}, true, false};
    static const HeaderKeySet HFS_MAX_FORWARDS {"max-forwards", HF_MAX_FORWARDS, {}, true, false};
    static const HeaderKeySet HFS_ORGANIZATION {"organization", HF_ORGANIZATION, {}, true, false};
    static const HeaderKeySet HFS_PROXY_AUTHORIZATION {"proxy-authorization", HF_PROXY_AUTHORIZATION, {}, true, false};
    static const HeaderKeySet HFS_PROXY_REQUIRE {"proxy-require", HF_PROXY_REQUIRE, {}, true, false};
    static const HeaderKeySet HFS_ROUTE {"route", HF_ROUTE, {}, true, true};
    static const HeaderKeySet HFS_REQUIRE {"require", HF_REQUIRE, {}, true, false};
    static const HeaderKeySet HFS_RESPONSE_KEY {"response-key", HF_RESPONSE_KEY, {}, true, false};
    static const HeaderKeySet HFS_USER_AGENT {"user-agent", HF_USER_AGENT, {}, true, false};
    static const HeaderKeySet HFS_PROXY_AUTHENTICATE {"proxy-authenticate", HF_PROXY_AUTHENTICATE, {}, true, false};
    static const HeaderKeySet HFS_RETRY_AFTER {"retry-after", HF_RETRY_AFTER, {}, true, false};
    static const HeaderKeySet HFS_SERVER {"server", HF_SERVER, {}, true, false};
    static const HeaderKeySet HFS_SUPPORTED {"supported", HF_SUPPORTED, "k", true, true};
    static const HeaderKeySet HFS_ALLOW {"allow", HF_ALLOW, {}, true, false};
    static const HeaderKeySet HFS_UNSUPPORTED {"unsupported", HF_UNSUPPORTED, {}, true, false};
    static const HeaderKeySet HFS_WARNING {"warning", HF_WARNING, {}, true, true};
    static const HeaderKeySet HFS_WWW_AUTHENTICATE {"www-authenticate", HF_WWW_AUTHENTICATE, {}, true, false};
    static const HeaderKeySet HFS_AUTHORIZATION {"authorization", HF_AUTHORIZATION, "uthorization", true, false};
    static const HeaderKeySet HFS_SUBSCRIPTION_STATE {"subscription-state", HF_SUBSCRIPTION_STATE, {}, true, false};
    static const HeaderKeySet HFS_EMPTY {"", "", {}, false, false};

    /// @brief Packs up to 4 characters into a 64-bit integer tag with inline case-folding.
    /// @details For architectural design details on why 64-bit packed switch matching outperforms SSO string comparisons,
    /// see docs/features/optimization_choices.md (https://siddiqsoft.github.io/sip2json/features/optimization_choices/).
    constexpr uint64_t pack_key_4(const char* s, size_t len) noexcept
    {
        uint64_t val = 0;
        size_t count = len < 4 ? len : 4;
        for (size_t i = 0; i < count; ++i)
        {
            char c = s[i];
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
            val |= (static_cast<uint64_t>(static_cast<unsigned char>(c)) << (i * 8));
        }
        return val;
    }

    constexpr uint64_t make_tag4(const char* s) noexcept
    {
        uint64_t val = 0;
        size_t i = 0;
        while (s[i] != '\0' && i < 4)
        {
            char c = s[i];
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
            val |= (static_cast<uint64_t>(static_cast<unsigned char>(c)) << (i * 8));
            i++;
        }
        return val;
    }

    inline const HeaderKeySet& canonicalizeHeaderKey(const std::string& keyFromPayload)
    {
        if (keyFromPayload.empty()) return HFS_EMPTY;

        uint64_t tag = pack_key_4(keyFromPayload.data(), keyFromPayload.size());

        // Fast-path: Custom headers starting with X- / x- / X_ / x_
        if ((tag & 0xFFFF) == make_tag4("x-") || (tag & 0xFFFF) == make_tag4("x_"))
        {
            thread_local HeaderKeySet customKey;
            customKey = HeaderKeySet(keyFromPayload);
            return customKey;
        }

        std::string lowerKey;
        lowerKey.reserve(keyFromPayload.size());
        std::transform(keyFromPayload.begin(), keyFromPayload.end(), std::back_inserter(lowerKey), ::tolower);

        switch (tag)
        {
        case make_tag4("auth"):
        case make_tag4("utho"):
            if (lowerKey == HFS_AUTHORIZATION.lower() || lowerKey == HFS_AUTHORIZATION.alt())
                return HFS_AUTHORIZATION;
            break;
        case make_tag4("from"):
            if (lowerKey == HFS_FROM.lower()) return HFS_FROM;
            break;
        case make_tag4("f"):
            return HFS_FROM;
        case make_tag4("to"):
            if (lowerKey == HFS_TO.lower()) return HFS_TO;
            break;
        case make_tag4("t"):
            return HFS_TO;
        case make_tag4("prio"):
            if (lowerKey == HFS_PRIORITY.lower()) return HFS_PRIORITY;
            break;
        case make_tag4("cont"):
            if (lowerKey == HFS_CONTENT_ENCODING.lower()) return HFS_CONTENT_ENCODING;
            if (lowerKey == HFS_CONTENT_LENGTH.lower()) return HFS_CONTENT_LENGTH;
            if (lowerKey == HFS_CONTENT_TYPE.lower()) return HFS_CONTENT_TYPE;
            if (lowerKey == HFS_CONTACT.lower()) return HFS_CONTACT;
            break;
        case make_tag4("e"):
            return HFS_CONTENT_ENCODING;
        case make_tag4("l"):
            return HFS_CONTENT_LENGTH;
        case make_tag4("c"):
            return HFS_CONTENT_TYPE;
        case make_tag4("call"):
            if (lowerKey == HFS_CALLID.lower()) return HFS_CALLID;
            break;
        case make_tag4("i"):
            return HFS_CALLID;
        case make_tag4("cseq"):
            if (lowerKey == HFS_CSEQ.lower()) return HFS_CSEQ;
            break;
        case make_tag4("via"):
            if (lowerKey == HFS_VIA.lower()) return HFS_VIA;
            break;
        case make_tag4("v"):
            return HFS_VIA;
        case make_tag4("encr"):
            if (lowerKey == HFS_ENCRYPTION.lower()) return HFS_ENCRYPTION;
            break;
        case make_tag4("subj"):
            if (lowerKey == HFS_SUBJECT.lower()) return HFS_SUBJECT;
            break;
        case make_tag4("s"):
            return HFS_SUBJECT;
        case make_tag4("loca"):
            if (lowerKey == HFS_LOCATION.lower()) return HFS_LOCATION;
            break;
        case make_tag4("expi"):
            if (lowerKey == HFS_EXPIRES.lower()) return HFS_EXPIRES;
            break;
        case make_tag4("m"):
            return HFS_CONTACT;
        case make_tag4("acce"):
            if (lowerKey == HFS_ACCEPT.lower()) return HFS_ACCEPT;
            if (lowerKey == HFS_ACCEPT_ENCODING.lower()) return HFS_ACCEPT_ENCODING;
            if (lowerKey == HFS_ACCEPT_LANGUAGE.lower()) return HFS_ACCEPT_LANGUAGE;
            break;
        case make_tag4("date"):
            if (lowerKey == HFS_DATE.lower()) return HFS_DATE;
            break;
        case make_tag4("reco"):
            if (lowerKey == HFS_RECORD_ROUTE.lower()) return HFS_RECORD_ROUTE;
            break;
        case make_tag4("time"):
            if (lowerKey == HFS_TIMESTAMP.lower()) return HFS_TIMESTAMP;
            break;
        case make_tag4("hide"):
            if (lowerKey == HFS_HIDE.lower()) return HFS_HIDE;
            break;
        case make_tag4("max-"):
            if (lowerKey == HFS_MAX_FORWARDS.lower()) return HFS_MAX_FORWARDS;
            break;
        case make_tag4("orga"):
            if (lowerKey == HFS_ORGANIZATION.lower()) return HFS_ORGANIZATION;
            break;
        case make_tag4("prox"):
            if (lowerKey == HFS_PROXY_AUTHORIZATION.lower()) return HFS_PROXY_AUTHORIZATION;
            if (lowerKey == HFS_PROXY_REQUIRE.lower()) return HFS_PROXY_REQUIRE;
            if (lowerKey == HFS_PROXY_AUTHENTICATE.lower()) return HFS_PROXY_AUTHENTICATE;
            break;
        case make_tag4("rout"):
            if (lowerKey == HFS_ROUTE.lower()) return HFS_ROUTE;
            break;
        case make_tag4("requ"):
            if (lowerKey == HFS_REQUIRE.lower()) return HFS_REQUIRE;
            break;
        case make_tag4("resp"):
            if (lowerKey == HFS_RESPONSE_KEY.lower()) return HFS_RESPONSE_KEY;
            break;
        case make_tag4("user"):
            if (lowerKey == HFS_USER_AGENT.lower()) return HFS_USER_AGENT;
            break;
        case make_tag4("retr"):
            if (lowerKey == HFS_RETRY_AFTER.lower()) return HFS_RETRY_AFTER;
            break;
        case make_tag4("serv"):
            if (lowerKey == HFS_SERVER.lower()) return HFS_SERVER;
            break;
        case make_tag4("supp"):
            if (lowerKey == HFS_SUPPORTED.lower()) return HFS_SUPPORTED;
            break;
        case make_tag4("k"):
            return HFS_SUPPORTED;
        case make_tag4("allo"):
            if (lowerKey == HFS_ALLOW.lower()) return HFS_ALLOW;
            break;
        case make_tag4("unsu"):
            if (lowerKey == HFS_UNSUPPORTED.lower()) return HFS_UNSUPPORTED;
            break;
        case make_tag4("warn"):
            if (lowerKey == HFS_WARNING.lower()) return HFS_WARNING;
            break;
        case make_tag4("www-"):
            if (lowerKey == HFS_WWW_AUTHENTICATE.lower()) return HFS_WWW_AUTHENTICATE;
            break;
        case make_tag4("subs"):
            if (lowerKey == HFS_SUBSCRIPTION_STATE.lower()) return HFS_SUBSCRIPTION_STATE;
            break;
        }

        thread_local HeaderKeySet fallbackKey;
        fallbackKey = HeaderKeySet(keyFromPayload);
        return fallbackKey;
    }
} // namespace siddiqsoft

#endif
