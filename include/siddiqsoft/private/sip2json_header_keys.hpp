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
        HeaderKeySet(const std::string& lower, const std::string& canon, const std::string& abbrev = {})
            : m_lowercase(lower)
            , m_canonical(canon)
            , m_abbreviation(abbrev)
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
    static const HeaderKeySet HFS_FROM {"from", HF_FROM, "f"};
    static const HeaderKeySet HFS_TO {"to", HF_TO, "t"};
    static const HeaderKeySet HFS_PRIORITY {"priority", HF_PRIORITY, {}};
    static const HeaderKeySet HFS_CONTENT_ENCODING {"content-encoding", HF_CONTENT_ENCODING, "e"};
    static const HeaderKeySet HFS_CONTENT_LENGTH {"content-length", HF_CONTENT_LENGTH, "l"};
    static const HeaderKeySet HFS_CONTENT_TYPE {"content-type", HF_CONTENT_TYPE, "c"};
    static const HeaderKeySet HFS_CALLID {"call-id", HF_CALLID, "i"};
    static const HeaderKeySet HFS_CSEQ {"cseq", HF_CSEQ, {}};
    static const HeaderKeySet HFS_VIA {"via", HF_VIA, "v"};
    static const HeaderKeySet HFS_ENCRYPTION {"encryption", HF_ENCRYPTION, {}};
    static const HeaderKeySet HFS_SUBJECT {"subject", HF_SUBJECT, "s"};
    static const HeaderKeySet HFS_LOCATION {"location", HF_LOCATION, {}};
    static const HeaderKeySet HFS_EXPIRES {"expires", HF_EXPIRES, {}};
    static const HeaderKeySet HFS_CONTACT {"contact", HF_CONTACT, "m"};
    static const HeaderKeySet HFS_ACCEPT {"accept", HF_ACCEPT, {}};
    static const HeaderKeySet HFS_ACCEPT_ENCODING {"accept-encoding", HF_ACCEPT_ENCODING, {}};
    static const HeaderKeySet HFS_ACCEPT_LANGUAGE {"accept-language", HF_ACCEPT_LANGUAGE, {}};
    static const HeaderKeySet HFS_DATE {"date", HF_DATE, {}};
    static const HeaderKeySet HFS_RECORD_ROUTE {"record-route", HF_RECORD_ROUTE, {}};
    static const HeaderKeySet HFS_TIMESTAMP {"timestamp", HF_TIMESTAMP, {}};
    static const HeaderKeySet HFS_HIDE {"hide", HF_HIDE, {}};
    static const HeaderKeySet HFS_MAX_FORWARDS {"max-forwards", HF_MAX_FORWARDS, {}};
    static const HeaderKeySet HFS_ORGANIZATION {"organization", HF_ORGANIZATION, {}};
    static const HeaderKeySet HFS_PROXY_AUTHORIZATION {"proxy-authorization", HF_PROXY_AUTHORIZATION, {}};
    static const HeaderKeySet HFS_PROXY_REQUIRE {"proxy-require", HF_PROXY_REQUIRE, {}};
    static const HeaderKeySet HFS_ROUTE {"route", HF_ROUTE, {}};
    static const HeaderKeySet HFS_REQUIRE {"require", HF_REQUIRE, {}};
    static const HeaderKeySet HFS_RESPONSE_KEY {"response-key", HF_RESPONSE_KEY, {}};
    static const HeaderKeySet HFS_USER_AGENT {"user-agent", HF_USER_AGENT, {}};
    static const HeaderKeySet HFS_PROXY_AUTHENTICATE {"proxy-authenticate", HF_PROXY_AUTHENTICATE, {}};
    static const HeaderKeySet HFS_RETRY_AFTER {"retry-after", HF_RETRY_AFTER, {}};
    static const HeaderKeySet HFS_SERVER {"server", HF_SERVER, {}};
    static const HeaderKeySet HFS_SUPPORTED {"supported", HF_SUPPORTED, "k"};
    static const HeaderKeySet HFS_ALLOW {"allow", HF_ALLOW, {}};
    static const HeaderKeySet HFS_UNSUPPORTED {"unsupported", HF_UNSUPPORTED, {}};
    static const HeaderKeySet HFS_WARNING {"warning", HF_WARNING, {}};
    static const HeaderKeySet HFS_WWW_AUTHENTICATE {"www-authenticate", HF_WWW_AUTHENTICATE, {}};

    // DO NOT CHANGE THIS! There are some some implementations that send "uthorization" instead of "Authorization"; yes--without the leading "A".
    // This is a bug in those implementations, but we must support it for interoperability.
    static const HeaderKeySet HFS_AUTHORIZATION {"authorization", HF_AUTHORIZATION, "uthorization"};

    // Subscribe/Notify header fields
    static const HeaderKeySet HFS_SUBSCRIPTION_STATE {"subscription-state", HF_SUBSCRIPTION_STATE, {}};

    // This HFS_CUSTOM is used to hint at non-canonical header.
    static const HeaderKeySet HFS_EMPTY {"", "", {}};

    class CanonicalHeaderKeyResult
    {
    public:
        bool                isCanonical {false};
        bool                isMultiLine {false};
        bool                isCustom {false};
        const HeaderKeySet& canonicalKeySet = HFS_EMPTY;
        HeaderKeySet        customKeySet;

        CanonicalHeaderKeyResult(bool isCanon, bool isMulti, const HeaderKeySet& keySet)
            : isCanonical(isCanon)
            , isMultiLine(isMulti)
            , canonicalKeySet(keySet)
            , isCustom(false)
            , customKeySet(HFS_EMPTY)
        {
        }

        CanonicalHeaderKeyResult(const std::string& customKey)
            : isCanonical(false)
            , isMultiLine(false)
            , isCustom(true)
            , canonicalKeySet(HFS_EMPTY)
            , customKeySet(customKey, customKey)
        {
        }
        operator const HeaderKeySet&() const { return isCanonical ? canonicalKeySet : customKeySet; }
    };

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

    inline CanonicalHeaderKeyResult canonicalizeHeaderKey(const std::string& keyFromPayload)
    {
        if (keyFromPayload.empty()) return CanonicalHeaderKeyResult {keyFromPayload};

        uint64_t tag = pack_key_4(keyFromPayload.data(), keyFromPayload.size());

        // Fast-path: Custom headers starting with X- / x- / X_ / x_
        if ((tag & 0xFFFF) == make_tag4("x-") || (tag & 0xFFFF) == make_tag4("x_"))
            return CanonicalHeaderKeyResult {keyFromPayload};

        // Convert the key to lowercase for matching full strings inside the switch branch
        std::string lowerKey;
        lowerKey.reserve(keyFromPayload.size());
        std::transform(keyFromPayload.begin(), keyFromPayload.end(), std::back_inserter(lowerKey), ::tolower);

        switch (tag)
        {
        case make_tag4("auth"):
        case make_tag4("utho"):
            if (lowerKey == HFS_AUTHORIZATION.lower() || lowerKey == HFS_AUTHORIZATION.alt())
                return {true, false, HFS_AUTHORIZATION};
            break;
        case make_tag4("from"):
            if (lowerKey == HFS_FROM.lower()) return {true, false, HFS_FROM};
            break;
        case make_tag4("f"):
            return {true, false, HFS_FROM};
        case make_tag4("to"):
            if (lowerKey == HFS_TO.lower()) return {true, false, HFS_TO};
            break;
        case make_tag4("t"):
            return {true, false, HFS_TO};
        case make_tag4("prio"):
            if (lowerKey == HFS_PRIORITY.lower()) return {true, false, HFS_PRIORITY};
            break;
        case make_tag4("cont"):
            if (lowerKey == HFS_CONTENT_ENCODING.lower()) return {true, false, HFS_CONTENT_ENCODING};
            if (lowerKey == HFS_CONTENT_LENGTH.lower()) return {true, false, HFS_CONTENT_LENGTH};
            if (lowerKey == HFS_CONTENT_TYPE.lower()) return {true, false, HFS_CONTENT_TYPE};
            if (lowerKey == HFS_CONTACT.lower()) return {true, false, HFS_CONTACT};
            break;
        case make_tag4("e"):
            return {true, false, HFS_CONTENT_ENCODING};
        case make_tag4("l"):
            return {true, false, HFS_CONTENT_LENGTH};
        case make_tag4("c"):
            return {true, false, HFS_CONTENT_TYPE};
        case make_tag4("call"):
            if (lowerKey == HFS_CALLID.lower()) return {true, false, HFS_CALLID};
            break;
        case make_tag4("i"):
            return {true, false, HFS_CALLID};
        case make_tag4("cseq"):
            if (lowerKey == HFS_CSEQ.lower()) return {true, false, HFS_CSEQ};
            break;
        case make_tag4("via"):
            if (lowerKey == HFS_VIA.lower()) return {true, true, HFS_VIA};
            break;
        case make_tag4("v"):
            return {true, true, HFS_VIA};
        case make_tag4("encr"):
            if (lowerKey == HFS_ENCRYPTION.lower()) return {true, false, HFS_ENCRYPTION};
            break;
        case make_tag4("subj"):
            if (lowerKey == HFS_SUBJECT.lower()) return {true, false, HFS_SUBJECT};
            break;
        case make_tag4("s"):
            return {true, false, HFS_SUBJECT};
        case make_tag4("loca"):
            if (lowerKey == HFS_LOCATION.lower()) return {true, false, HFS_LOCATION};
            break;
        case make_tag4("expi"):
            if (lowerKey == HFS_EXPIRES.lower()) return {true, false, HFS_EXPIRES};
            break;
        case make_tag4("m"):
            return {true, false, HFS_CONTACT};
        case make_tag4("acce"):
            if (lowerKey == HFS_ACCEPT.lower()) return {true, true, HFS_ACCEPT};
            if (lowerKey == HFS_ACCEPT_ENCODING.lower()) return {true, false, HFS_ACCEPT_ENCODING};
            if (lowerKey == HFS_ACCEPT_LANGUAGE.lower()) return {true, false, HFS_ACCEPT_LANGUAGE};
            break;
        case make_tag4("date"):
            if (lowerKey == HFS_DATE.lower()) return {true, false, HFS_DATE};
            break;
        case make_tag4("reco"):
            if (lowerKey == HFS_RECORD_ROUTE.lower()) return {true, true, HFS_RECORD_ROUTE};
            break;
        case make_tag4("time"):
            if (lowerKey == HFS_TIMESTAMP.lower()) return {true, false, HFS_TIMESTAMP};
            break;
        case make_tag4("hide"):
            if (lowerKey == HFS_HIDE.lower()) return {true, false, HFS_HIDE};
            break;
        case make_tag4("max-"):
            if (lowerKey == HFS_MAX_FORWARDS.lower()) return {true, false, HFS_MAX_FORWARDS};
            break;
        case make_tag4("orga"):
            if (lowerKey == HFS_ORGANIZATION.lower()) return {true, false, HFS_ORGANIZATION};
            break;
        case make_tag4("prox"):
            if (lowerKey == HFS_PROXY_AUTHORIZATION.lower()) return {true, false, HFS_PROXY_AUTHORIZATION};
            if (lowerKey == HFS_PROXY_REQUIRE.lower()) return {true, false, HFS_PROXY_REQUIRE};
            if (lowerKey == HFS_PROXY_AUTHENTICATE.lower()) return {true, false, HFS_PROXY_AUTHENTICATE};
            break;
        case make_tag4("rout"):
            if (lowerKey == HFS_ROUTE.lower()) return {true, true, HFS_ROUTE};
            break;
        case make_tag4("requ"):
            if (lowerKey == HFS_REQUIRE.lower()) return {true, false, HFS_REQUIRE};
            break;
        case make_tag4("resp"):
            if (lowerKey == HFS_RESPONSE_KEY.lower()) return {true, false, HFS_RESPONSE_KEY};
            break;
        case make_tag4("user"):
            if (lowerKey == HFS_USER_AGENT.lower()) return {true, false, HFS_USER_AGENT};
            break;
        case make_tag4("retr"):
            if (lowerKey == HFS_RETRY_AFTER.lower()) return {true, false, HFS_RETRY_AFTER};
            break;
        case make_tag4("serv"):
            if (lowerKey == HFS_SERVER.lower()) return {true, false, HFS_SERVER};
            break;
        case make_tag4("supp"):
            if (lowerKey == HFS_SUPPORTED.lower()) return {true, true, HFS_SUPPORTED};
            break;
        case make_tag4("k"):
            return {true, true, HFS_SUPPORTED};
        case make_tag4("allo"):
            if (lowerKey == HFS_ALLOW.lower()) return {true, false, HFS_ALLOW};
            break;
        case make_tag4("unsu"):
            if (lowerKey == HFS_UNSUPPORTED.lower()) return {true, false, HFS_UNSUPPORTED};
            break;
        case make_tag4("warn"):
            if (lowerKey == HFS_WARNING.lower()) return {true, true, HFS_WARNING};
            break;
        case make_tag4("www-"):
            if (lowerKey == HFS_WWW_AUTHENTICATE.lower()) return {true, false, HFS_WWW_AUTHENTICATE};
            break;
        case make_tag4("subs"):
            if (lowerKey == HFS_SUBSCRIPTION_STATE.lower()) return {true, false, HFS_SUBSCRIPTION_STATE};
            break;
        }

        // Return original keyFromPayload if no match found for custom headers
        return CanonicalHeaderKeyResult {keyFromPayload};
    }
} // namespace siddiqsoft

#endif
