#include <string>
#include <chrono>

#include "gtest/gtest.h"
#include "../../src/sip2json.hpp"
#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

namespace siddiqsoftware
{
	TEST(Helpers, Test_createCallId)
	{
		auto ci = sip2json::createCallId();
		EXPECT_TRUE(ci.length() == 44);
	}


	TEST(DateTime, Test_getRFC1123)
	{
		auto todays_date = sip2json::getRFC1123();
		EXPECT_TRUE(!todays_date.empty()) << todays_date;
	}

	TEST(DateTime, Test_getRFC1123_args)
	{
		tm knowntm {};
		knowntm.tm_year	 = 2010 - 1900;
		knowntm.tm_mon	 = 11 - 1; // Nov
		knowntm.tm_mday	 = 13;	   // 13th
		knowntm.tm_hour	 = 23;	   // 23h
		knowntm.tm_min	 = 29;	   // 29m
		knowntm.tm_sec	 = 0;	   // 0s
		knowntm.tm_wday	 = 6;	   // Sat
		knowntm.tm_isdst = 0;


		auto todays_date = sip2json::getRFC1123(std::chrono::system_clock::from_time_t(_mkgmtime(&knowntm)));
		EXPECT_TRUE(todays_date.compare("Sat, 13 Nov 2010 23:29:00 GMT") == 0) << todays_date;
	}

	TEST(DateTime, Test_getISO8601)
	{
		auto todays_date = sip2json::getISO8601();
		EXPECT_TRUE(!todays_date.empty()) << todays_date;
	}

	TEST(DateTime, Test_getISO8601_args)
	{
		tm knowntm {};
		knowntm.tm_year	 = 2010 - 1900;
		knowntm.tm_mon	 = 11 - 1; // Nov
		knowntm.tm_mday	 = 13;	   // 13th
		knowntm.tm_hour	 = 23;	   // 23h
		knowntm.tm_min	 = 29;	   // 29m
		knowntm.tm_sec	 = 0;	   // 0s
		knowntm.tm_wday	 = 6;	   // Sat
		knowntm.tm_isdst = 0;

		auto knownDate = sip2json::getISO8601(std::chrono::system_clock::from_time_t(_mkgmtime(&knowntm)));
		// Note the use of "find" instead of compare since the milliseconds are an unkown and
		// unless we create from scratch they will contain an arbitrary noise.
		EXPECT_TRUE(knownDate.find("2010-11-13T23:29:00.") == 0) << knownDate;
	}


	TEST(SIPHelpers, Test_createRequest)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", {}, 1);
		auto diagContents	 = registerMessage.dump(2);
		std::cout << diagContents << std::endl;
		std::cerr << diagContents << std::endl;
		EXPECT_TRUE(registerMessage.size() != 0);
	}


} // namespace siddiqsoftware