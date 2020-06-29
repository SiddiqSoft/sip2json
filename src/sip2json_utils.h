/*
  A SIP Parser for Modern C++: Utilities and Helpers
  Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

#pragma once

#include <string>
#include <chrono>
#include <random>
#include <sstream>

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

namespace siddiqsoftware
{
#pragma region Datetime helpers
	/// @brief Create a string representation of the timepoint as RFC1123 spec
	/// @param tp Optional system_clock::timepoint; uses "now" if not provided
	/// @return String with your date/time as "Sun, 28 Jun 2020 23:29:00 GMT"
	static std::string getRFC1123(std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now()) noexcept(false)
	{
		return fmt::format("{:%a, %d %b %Y %T} GMT", fmt::gmtime(std::chrono::system_clock::to_time_t(tp)));
	}


	/// @brief Creates a string representaiton of the date time in ISO8601 format with millisecond precision.
	/// @param tp Optional system_clock::timepoint; uses "now" if not provided
	/// @return String ISO8601 "2020-06-28T23:29:00.000Z"
	static std::string getISO8601(std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now()) noexcept(false)
	{
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
		return fmt::format("{:%Y-%m-%dT%T}.{:03}Z", fmt::gmtime(std::chrono::system_clock::to_time_t(tp)), ms);
	}
#pragma endregion


	/// @brief Creates a pseudo random number generated UUID v4. It is best to use platform-specific method to ensure guid
	/// @return string 44 character of the format: 7792eaf4-456f-4d47-d93-863af0e0-a8b99b9b9988
	static std::string createCallId()
	{
		static std::random_device			 rd;
		static std::mt19937_64				 generator(rd());
		static std::uniform_int_distribution ud(0, 15);
		static std::uniform_int_distribution ud2(8, 11);

		std::stringstream sBuffer;

		sBuffer << std::hex;
		for (auto i = 0; i < 8; i++)
			sBuffer << ud(generator);
		sBuffer << "-";
		for (auto i = 0; i < 4; i++)
			sBuffer << ud(generator);
		sBuffer << "-4";
		for (auto i = 0; i < 3; i++)
			sBuffer << ud(generator);
		sBuffer << "-";
		for (auto i = 0; i < 3; i++)
			sBuffer << ud(generator);
		sBuffer << "-";
		for (auto i = 0; i < 8; i++)
			sBuffer << ud(generator);
		sBuffer << "-";
		for (auto i = 0; i < 12; i++)
			sBuffer << ud2(generator);
		return sBuffer.str();
	}
} // namespace siddiqsoftware