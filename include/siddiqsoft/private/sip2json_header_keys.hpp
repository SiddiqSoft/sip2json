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
    // We're defining header key set as an array that contains the canonical header key
    // and any other variations of the header key that we want to support.
    // The second element is the canonical header key, and the first element is the lowercase version of the canonical header key.
    // The last item (if present) is the single character abbreviation for the header key.
    using HeaderKeySet = std::array<std::string_view, 3>; // {lowercase, canonical, abbreviation}

    // This allows us to compare incoming header keys against a set of known variations, making the parser more robust and flexible.
    static constexpr HeaderKeySet HFS_FROM {"from", "From", "f"};
    static constexpr HeaderKeySet HFS_TO {"to", "To", "t"};
    static constexpr HeaderKeySet HFS_PRIORITY {"priority", "Priority", {}};
    static constexpr HeaderKeySet HFS_CONTENT_ENCODING {"content-encoding", "Content-Encoding", "e"};
    static constexpr HeaderKeySet HFS_CONTENT_LENGTH {"content-length", "Content-Length", "l"};
    static constexpr HeaderKeySet HFS_CONTENT_TYPE {"content-type", "Content-Type", "c"};
    static constexpr HeaderKeySet HFS_CALLID {"call-id", "Call-ID", "i"};
    static constexpr HeaderKeySet HFS_CSEQ {"cseq", "CSeq", {}};
    static constexpr HeaderKeySet HFS_VIA {"via", "Via", "v"};
    static constexpr HeaderKeySet HFS_ENCRYPTION {"encryption", "Encryption", {}};
    static constexpr HeaderKeySet HFS_SUBJECT {"subject", "Subject", "s"};
    static constexpr HeaderKeySet HFS_LOCATION {"location", "Location", {}};
    static constexpr HeaderKeySet HFS_EXPIRES {"expires", "Expires", {}};
    static constexpr HeaderKeySet HFS_CONTACT {"contact", "Contact", "m"};
    static constexpr HeaderKeySet HFS_ACCEPT {"accept", "Accept", {}};
    static constexpr HeaderKeySet HFS_ACCEPT_ENCODING {"accept-encoding", "Accept-Encoding", {}};
    static constexpr HeaderKeySet HFS_ACCEPT_LANGUAGE {"accept-language", "Accept-Language", {}};
    static constexpr HeaderKeySet HFS_DATE {"date", "Date", {}};
    static constexpr HeaderKeySet HFS_RECORD_ROUTE {"record-route", "Record-Route", {}};
    static constexpr HeaderKeySet HFS_TIMESTAMP {"timestamp", "Timestamp", {}};
    static constexpr HeaderKeySet HFS_HIDE {"hide", "Hide", {}};
    static constexpr HeaderKeySet HFS_MAX_FORWARDS {"max-forwards", "Max-Forwards", {}};
    static constexpr HeaderKeySet HFS_ORGANIZATION {"organization", "Organization", {}};
    static constexpr HeaderKeySet HFS_PROXY_AUTHORIZATION {"proxy-authorization", "Proxy-Authorization", {}};
    static constexpr HeaderKeySet HFS_PROXY_REQUIRE {"proxy-require", "Proxy-Require", {}};
    static constexpr HeaderKeySet HFS_ROUTE {"route", "Route", {}};
    static constexpr HeaderKeySet HFS_REQUIRE {"require", "Require", {}};
    static constexpr HeaderKeySet HFS_RESPONSE_KEY {"response-key", "Response-Key", {}};
    static constexpr HeaderKeySet HFS_USER_AGENT {"user-agent", "User-Agent", {}};
    static constexpr HeaderKeySet HFS_PROXY_AUTHENTICATE {"proxy-authenticate", "Proxy-Authenticate", {}};
    static constexpr HeaderKeySet HFS_RETRY_AFTER {"retry-after", "Retry-After", {}};
    static constexpr HeaderKeySet HFS_SERVER {"server", "Server", {}};
    static constexpr HeaderKeySet HFS_SUPPORTED {"supported", "Supported", "k"};
    static constexpr HeaderKeySet HFS_ALLOW {"allow", "Allow", {}};
    static constexpr HeaderKeySet HFS_UNSUPPORTED {"unsupported", "Unsupported", {}};
    static constexpr HeaderKeySet HFS_WARNING {"warning", "Warning", {}};
    static constexpr HeaderKeySet HFS_WWW_AUTHENTICATE {"www-authenticate", "WWW-Authenticate", {}};

    // DO NOT CHANGE THIS! There are some some implementations that send "uthorization" instead of "Authorization"; yes--without the leading "A".
    // This is a bug in those implementations, but we must support it for interoperability.
    static constexpr HeaderKeySet HFS_AUTHORIZATION {"authorization", "Authorization", "uthorization"};

    // Subscribe/Notify header fields
    static constexpr HeaderKeySet HFS_SUBSCRIPTION_STATE {"subscription-state", "Subscription-State", {}};

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

    struct CanonicalHeaderKeyResult
    {
        bool               isCanonical {false};
        bool               isMultiLine {false};
        std::string_view   canonicalKey {};
        const std::string* canonicalStringPtr {nullptr};

        operator std::string() const
        {
            return canonicalStringPtr ? *canonicalStringPtr : std::string {canonicalKey};
        }
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
        if (HFS_AUTHORIZATION[0] == lowerKey || HFS_AUTHORIZATION[2] == lowerKey)
            return {true, false, HFS_AUTHORIZATION[1], &HF_AUTHORIZATION};

        if (HFS_FROM[0] == lowerKey || HFS_FROM[2] == lowerKey) return {true, false, HFS_FROM[1], &HF_FROM};
        if (HFS_TO[0] == lowerKey || HFS_TO[2] == lowerKey) return {true, false, HFS_TO[1], &HF_TO};
        if (HFS_PRIORITY[0] == lowerKey || HFS_PRIORITY[2] == lowerKey) return {true, false, HFS_PRIORITY[1], &HF_PRIORITY};
        if (HFS_CONTENT_ENCODING[0] == lowerKey || HFS_CONTENT_ENCODING[2] == lowerKey)
            return {true, false, HFS_CONTENT_ENCODING[1], &HF_CONTENT_ENCODING};
        if (HFS_CONTENT_LENGTH[0] == lowerKey || HFS_CONTENT_LENGTH[2] == lowerKey) return {true, false, HFS_CONTENT_LENGTH[1], &HF_CONTENT_LENGTH};
        if (HFS_CONTENT_TYPE[0] == lowerKey || HFS_CONTENT_TYPE[2] == lowerKey) return {true, false, HFS_CONTENT_TYPE[1], &HF_CONTENT_TYPE};
        if (HFS_CALLID[0] == lowerKey || HFS_CALLID[2] == lowerKey) return {true, false, HFS_CALLID[1], &HF_CALLID};
        if (HFS_CSEQ[0] == lowerKey || HFS_CSEQ[2] == lowerKey) return {true, false, HFS_CSEQ[1], &HF_CSEQ};
        if (HFS_VIA[0] == lowerKey || HFS_VIA[2] == lowerKey) return {true, true, HFS_VIA[1], &HF_VIA};
        if (HFS_ENCRYPTION[0] == lowerKey || HFS_ENCRYPTION[2] == lowerKey) return {true, false, HFS_ENCRYPTION[1], &HF_ENCRYPTION};
        if (HFS_SUBJECT[0] == lowerKey || HFS_SUBJECT[2] == lowerKey) return {true, false, HFS_SUBJECT[1], &HF_SUBJECT};
        if (HFS_LOCATION[0] == lowerKey || HFS_LOCATION[2] == lowerKey) return {true, false, HFS_LOCATION[1], &HF_LOCATION};
        if (HFS_EXPIRES[0] == lowerKey || HFS_EXPIRES[2] == lowerKey) return {true, false, HFS_EXPIRES[1], &HF_EXPIRES};
        if (HFS_CONTACT[0] == lowerKey || HFS_CONTACT[2] == lowerKey) return {true, false, HFS_CONTACT[1], &HF_CONTACT};
        if (HFS_ACCEPT[0] == lowerKey || HFS_ACCEPT[2] == lowerKey) return {true, true, HFS_ACCEPT[1], &HF_ACCEPT};
        if (HFS_ACCEPT_ENCODING[0] == lowerKey || HFS_ACCEPT_ENCODING[2] == lowerKey) return {true, false, HFS_ACCEPT_ENCODING[1], &HF_ACCEPT_ENCODING};
        if (HFS_ACCEPT_LANGUAGE[0] == lowerKey || HFS_ACCEPT_LANGUAGE[2] == lowerKey) return {true, false, HFS_ACCEPT_LANGUAGE[1], &HF_ACCEPT_LANGUAGE};
        if (HFS_DATE[0] == lowerKey || HFS_DATE[2] == lowerKey) return {true, false, HFS_DATE[1], &HF_DATE};
        if (HFS_RECORD_ROUTE[0] == lowerKey || HFS_RECORD_ROUTE[2] == lowerKey) return {true, true, HFS_RECORD_ROUTE[1], &HF_RECORD_ROUTE};
        if (HFS_TIMESTAMP[0] == lowerKey || HFS_TIMESTAMP[2] == lowerKey) return {true, false, HFS_TIMESTAMP[1], &HF_TIMESTAMP};
        if (HFS_HIDE[0] == lowerKey || HFS_HIDE[2] == lowerKey) return {true, false, HFS_HIDE[1], &HF_HIDE};
        if (HFS_MAX_FORWARDS[0] == lowerKey || HFS_MAX_FORWARDS[2] == lowerKey) return {true, false, HFS_MAX_FORWARDS[1], &HF_MAX_FORWARDS};
        if (HFS_ORGANIZATION[0] == lowerKey || HFS_ORGANIZATION[2] == lowerKey) return {true, false, HFS_ORGANIZATION[1], &HF_ORGANIZATION};
        if (HFS_PROXY_AUTHORIZATION[0] == lowerKey || HFS_PROXY_AUTHORIZATION[2] == lowerKey)
            return {true, false, HFS_PROXY_AUTHORIZATION[1], &HF_PROXY_AUTHORIZATION};
        if (HFS_PROXY_REQUIRE[0] == lowerKey || HFS_PROXY_REQUIRE[2] == lowerKey) return {true, false, HFS_PROXY_REQUIRE[1], &HF_PROXY_REQUIRE};
        if (HFS_ROUTE[0] == lowerKey || HFS_ROUTE[2] == lowerKey) return {true, true, HFS_ROUTE[1], &HF_ROUTE};
        if (HFS_REQUIRE[0] == lowerKey || HFS_REQUIRE[2] == lowerKey) return {true, false, HFS_REQUIRE[1], &HF_REQUIRE};
        if (HFS_RESPONSE_KEY[0] == lowerKey || HFS_RESPONSE_KEY[2] == lowerKey) return {true, false, HFS_RESPONSE_KEY[1], &HF_RESPONSE_KEY};
        if (HFS_USER_AGENT[0] == lowerKey || HFS_USER_AGENT[2] == lowerKey) return {true, false, HFS_USER_AGENT[1], &HF_USER_AGENT};
        if (HFS_PROXY_AUTHENTICATE[0] == lowerKey || HFS_PROXY_AUTHENTICATE[2] == lowerKey)
            return {true, false, HFS_PROXY_AUTHENTICATE[1], &HF_PROXY_AUTHENTICATE};
        if (HFS_RETRY_AFTER[0] == lowerKey || HFS_RETRY_AFTER[2] == lowerKey) return {true, false, HFS_RETRY_AFTER[1], &HF_RETRY_AFTER};
        if (HFS_SERVER[0] == lowerKey || HFS_SERVER[2] == lowerKey) return {true, false, HFS_SERVER[1], &HF_SERVER};
        if (HFS_SUPPORTED[0] == lowerKey || HFS_SUPPORTED[2] == lowerKey) return {true, true, HFS_SUPPORTED[1], &HF_SUPPORTED};
        if (HFS_ALLOW[0] == lowerKey || HFS_ALLOW[2] == lowerKey) return {true, false, HFS_ALLOW[1], &HF_ALLOW};
        if (HFS_UNSUPPORTED[0] == lowerKey || HFS_UNSUPPORTED[2] == lowerKey) return {true, false, HFS_UNSUPPORTED[1], &HF_UNSUPPORTED};
        if (HFS_WARNING[0] == lowerKey || HFS_WARNING[2] == lowerKey) return {true, true, HFS_WARNING[1], &HF_WARNING};
        if (HFS_WWW_AUTHENTICATE[0] == lowerKey || HFS_WWW_AUTHENTICATE[2] == lowerKey)
            return {true, false, HFS_WWW_AUTHENTICATE[1], &HF_WWW_AUTHENTICATE};
        if (HFS_SUBSCRIPTION_STATE[0] == lowerKey || HFS_SUBSCRIPTION_STATE[2] == lowerKey)
            return {true, false, HFS_SUBSCRIPTION_STATE[1], &HF_SUBSCRIPTION_STATE};

        // Return original keyFromPayload if no match found for custom headers
        return {false, false, keyFromPayload, nullptr};
    }
} // namespace siddiqsoft

#endif
