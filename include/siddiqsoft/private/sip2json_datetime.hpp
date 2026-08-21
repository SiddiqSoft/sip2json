/*
    A SIP Parser for Modern C++: Date and Time Utilities
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

#ifndef SIP2JSON_DATETIME_HPP
#define SIP2JSON_DATETIME_HPP

#include <chrono>
#include <format>
#include <optional>
#include <string>

namespace siddiqsoft
{
#pragma region Datetime helpers
    /// @brief Helper struct which runs your lambda when this object goes out of scope. Used to time expression scope.
    /// @tparam Fn Lambda of type void(long long delta) called upon destructor; must not throw.
    template <typename Fn> struct InvokeOnDestruct
    {
        Fn                                          callbackOnEnd;
        const std::chrono::system_clock::time_point ttxStart {std::chrono::system_clock::now()};

        /// @brief Gets the time delta between start/instantiation of this object and now
        /// @return long long type delta
        auto ttx() noexcept
        {
            const auto ttxNow = std::chrono::system_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(ttxNow - ttxStart).count();
        };

        // Must provide a callback!
        InvokeOnDestruct() = delete;

        /// @brief Constructor with callback that is to be invoked at destructor
        /// @param callback Callback must accept long long indicating the delta
        /// @return Creates the object
        InvokeOnDestruct(Fn&& callback) noexcept
            : callbackOnEnd {callback} { };

        // Invoke the callback and silently ignore the exceptions
        ~InvokeOnDestruct() noexcept
        {
            try
            {
                callbackOnEnd(ttx());
            }
            catch (...)
            {
            }
        };
    };


    /// @brief Create a string representation of the timepoint as RFC1123 spec
    /// @param tp Optional system_clock::timepoint; uses "now" if not provided
    /// @return String with your date/time as "Sun, 28 Jun 2020 23:29:00 GMT"
    template <class T = std::string>
    static T TimeAsRFC1123(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
    {
        // This cast is critical since the RFC1123 does not have a fractional portion!
        const auto rawtp = std::chrono::time_point_cast<std::chrono::seconds>(src.value_or(std::chrono::system_clock::now()));

        if constexpr (std::is_same_v<T, std::string>)
            return std::format("{0:%a, %d %h %Y %T GMT}", rawtp);
        else if constexpr (std::is_same_v<T, std::wstring>)
            return std::format(L"{0:%a, %d %h %Y %T GMT}", rawtp);
        else
            return T {};
    }

    /// @brief Creates a string representaiton of the date time in RFC3339 format with millisecond precision.
    /// @param tp Optional system_clock::timepoint; uses "now" if not provided
    /// @return String RFC3339 "2020-06-28T23:29:00.000Z"
    template <class T = std::string>
    static T TimeAsRFC3339(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
    {
        // This cast is critical since the RFC3339 only asks for milliseconds!
        const auto rawtp = std::chrono::time_point_cast<std::chrono::milliseconds>(src.value_or(std::chrono::system_clock::now()));

        if constexpr (std::is_same_v<T, std::string>)
            return std::format("{0:%Y-%m-%dT%H:%M:%S}Z", rawtp);
        else if constexpr (std::is_same_v<T, std::wstring>)
            return std::format(L"{0:%Y-%m-%dT%H:%M:%S}Z", rawtp);
        else
            return T {};
    }

    /// @brief Creates a string representaiton of the date time in ISO8601 format with millisecond precision. Alias for TimeAsRFC3339 method.
    /// @param tp Optional system_clock::timepoint; uses "now" if not provided
    /// @return String ISO8601 "2020-06-28T23:29:00.000Z"
    template <class T = std::string>
    static T TimeAsISO8601(std::optional<std::chrono::system_clock::time_point> src = {}) noexcept(false)
    {
        // This cast is critical since the ISO8601 only asks for milliseconds!
        const auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(src.value_or(std::chrono::system_clock::now()));

        // NOTE: The resolution for %T includes siz-digits of microsecond detail!
        if constexpr (std::is_same_v<T, std::wstring>) { return std::format(L"{:%Y-%m-%dT%T}Z", tp); }

        return std::format("{:%Y-%m-%dT%T}Z", tp);
    }

#pragma endregion
} // namespace siddiqsoft

#endif
