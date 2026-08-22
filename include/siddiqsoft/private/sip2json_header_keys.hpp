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

        const std::string& canonical() const { return m_canonical; }
        const std::string& lower() const { return m_lowercase; }
        const std::string& alt() const { return m_abbreviation; }

        auto operator[](size_t index) const
        {
            switch (index)
            {
            case 0: return m_lowercase;
            case 1: return m_canonical;
            case 2: return m_abbreviation;
            default: throw std::out_of_range("HeaderKeySet index out of range");
            }
        }
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

    inline CanonicalHeaderKeyResult canonicalizeHeaderKey(const std::string& keyFromPayload)
    {
        // Convert the key to lowercase for comparison against canonical header sets
        std::string lowerKey;
        lowerKey.reserve(keyFromPayload.size());
        std::transform(keyFromPayload.begin(), keyFromPayload.end(), std::back_inserter(lowerKey), ::tolower);

        // compare the lowerKey against the known header key sets and return the canonical form if found
        // match against the lowercase version of the key or the abbreviation (if present) in the HeaderKeySet

        // WARNING! The Authorization header has a special case where some implementations send "uthorization" instead of "Authorization".
        // The abbreviation for "Authorization" is "uthorization" (without the leading "A") to support those implementations.
        // This is a known bug in those implementations, but we must support it for interoperability.
        if (HFS_AUTHORIZATION.lower() == lowerKey || HFS_AUTHORIZATION.alt() == lowerKey) return {true, false, HFS_AUTHORIZATION};

        if (HFS_FROM.lower() == lowerKey || HFS_FROM.alt() == lowerKey) return {true, false, HFS_FROM};
        if (HFS_TO.lower() == lowerKey || HFS_TO.alt() == lowerKey) return {true, false, HFS_TO};
        if (HFS_PRIORITY.lower() == lowerKey || HFS_PRIORITY.alt() == lowerKey) return {true, false, HFS_PRIORITY};
        if (HFS_CONTENT_ENCODING.lower() == lowerKey || HFS_CONTENT_ENCODING.alt() == lowerKey)
            return {true, false, HFS_CONTENT_ENCODING};
        if (HFS_CONTENT_LENGTH.lower() == lowerKey || HFS_CONTENT_LENGTH.alt() == lowerKey)
            return {true, false, HFS_CONTENT_LENGTH};
        if (HFS_CONTENT_TYPE.lower() == lowerKey || HFS_CONTENT_TYPE.alt() == lowerKey) return {true, false, HFS_CONTENT_TYPE};
        if (HFS_CALLID.lower() == lowerKey || HFS_CALLID.alt() == lowerKey) return {true, false, HFS_CALLID};
        if (HFS_CSEQ.lower() == lowerKey || HFS_CSEQ.alt() == lowerKey) return {true, false, HFS_CSEQ};
        if (HFS_VIA.lower() == lowerKey || HFS_VIA.alt() == lowerKey) return {true, true, HFS_VIA};
        if (HFS_ENCRYPTION.lower() == lowerKey || HFS_ENCRYPTION.alt() == lowerKey) return {true, false, HFS_ENCRYPTION};
        if (HFS_SUBJECT.lower() == lowerKey || HFS_SUBJECT.alt() == lowerKey) return {true, false, HFS_SUBJECT};
        if (HFS_LOCATION.lower() == lowerKey || HFS_LOCATION.alt() == lowerKey) return {true, false, HFS_LOCATION};
        if (HFS_EXPIRES.lower() == lowerKey || HFS_EXPIRES.alt() == lowerKey) return {true, false, HFS_EXPIRES};
        if (HFS_CONTACT.lower() == lowerKey || HFS_CONTACT.alt() == lowerKey) return {true, false, HFS_CONTACT};
        if (HFS_ACCEPT.lower() == lowerKey || HFS_ACCEPT.alt() == lowerKey) return {true, true, HFS_ACCEPT};
        if (HFS_ACCEPT_ENCODING.lower() == lowerKey || HFS_ACCEPT_ENCODING.alt() == lowerKey)
            return {true, false, HFS_ACCEPT_ENCODING};
        if (HFS_ACCEPT_LANGUAGE.lower() == lowerKey || HFS_ACCEPT_LANGUAGE.alt() == lowerKey)
            return {true, false, HFS_ACCEPT_LANGUAGE};
        if (HFS_DATE.lower() == lowerKey || HFS_DATE.alt() == lowerKey) return {true, false, HFS_DATE};
        if (HFS_RECORD_ROUTE.lower() == lowerKey || HFS_RECORD_ROUTE.alt() == lowerKey) return {true, true, HFS_RECORD_ROUTE};
        if (HFS_TIMESTAMP.lower() == lowerKey || HFS_TIMESTAMP.alt() == lowerKey) return {true, false, HFS_TIMESTAMP};
        if (HFS_HIDE.lower() == lowerKey || HFS_HIDE.alt() == lowerKey) return {true, false, HFS_HIDE};
        if (HFS_MAX_FORWARDS.lower() == lowerKey || HFS_MAX_FORWARDS.alt() == lowerKey) return {true, false, HFS_MAX_FORWARDS};
        if (HFS_ORGANIZATION.lower() == lowerKey || HFS_ORGANIZATION.alt() == lowerKey) return {true, false, HFS_ORGANIZATION};
        if (HFS_PROXY_AUTHORIZATION.lower() == lowerKey || HFS_PROXY_AUTHORIZATION.alt() == lowerKey)
            return {true, false, HFS_PROXY_AUTHORIZATION};
        if (HFS_PROXY_REQUIRE.lower() == lowerKey || HFS_PROXY_REQUIRE.alt() == lowerKey) return {true, false, HFS_PROXY_REQUIRE};
        if (HFS_ROUTE.lower() == lowerKey || HFS_ROUTE.alt() == lowerKey) return {true, true, HFS_ROUTE};
        if (HFS_REQUIRE.lower() == lowerKey || HFS_REQUIRE.alt() == lowerKey) return {true, false, HFS_REQUIRE};
        if (HFS_RESPONSE_KEY.lower() == lowerKey || HFS_RESPONSE_KEY.alt() == lowerKey) return {true, false, HFS_RESPONSE_KEY};
        if (HFS_USER_AGENT.lower() == lowerKey || HFS_USER_AGENT.alt() == lowerKey) return {true, false, HFS_USER_AGENT};
        if (HFS_PROXY_AUTHENTICATE.lower() == lowerKey || HFS_PROXY_AUTHENTICATE.alt() == lowerKey)
            return {true, false, HFS_PROXY_AUTHENTICATE};
        if (HFS_RETRY_AFTER.lower() == lowerKey || HFS_RETRY_AFTER.alt() == lowerKey) return {true, false, HFS_RETRY_AFTER};
        if (HFS_SERVER.lower() == lowerKey || HFS_SERVER.alt() == lowerKey) return {true, false, HFS_SERVER};
        if (HFS_SUPPORTED.lower() == lowerKey || HFS_SUPPORTED.alt() == lowerKey) return {true, true, HFS_SUPPORTED};
        if (HFS_ALLOW.lower() == lowerKey || HFS_ALLOW.alt() == lowerKey) return {true, false, HFS_ALLOW};
        if (HFS_UNSUPPORTED.lower() == lowerKey || HFS_UNSUPPORTED.alt() == lowerKey) return {true, false, HFS_UNSUPPORTED};
        if (HFS_WARNING.lower() == lowerKey || HFS_WARNING.alt() == lowerKey) return {true, true, HFS_WARNING};
        if (HFS_WWW_AUTHENTICATE.lower() == lowerKey || HFS_WWW_AUTHENTICATE.alt() == lowerKey)
            return {true, false, HFS_WWW_AUTHENTICATE};
        if (HFS_SUBSCRIPTION_STATE.lower() == lowerKey || HFS_SUBSCRIPTION_STATE.alt() == lowerKey)
            return {true, false, HFS_SUBSCRIPTION_STATE};

        // Return original keyFromPayload if no match found for custom headers
        return CanonicalHeaderKeyResult {keyFromPayload};
    }
} // namespace siddiqsoft

#endif
