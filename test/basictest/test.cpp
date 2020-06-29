/*
  Unit Tests for GoogleTest framework for sip2json
  A SIP Parser for Modern C++ / Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

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
		auto diagContents	 = registerMessage.flatten().dump(2);
		std::cerr << diagContents << std::endl;
		EXPECT_TRUE(registerMessage.size() != 0);
		EXPECT_TRUE(!registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).empty());
		EXPECT_TRUE(registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
		EXPECT_TRUE(registerMessage.value("/type"_json_pointer, std::string {}).compare("request") == 0);
	}


	TEST(SIPHelpers, Test_createResponse)
	{
		auto dummyMessage = sip2json::createResponse(500, "Unknown");
		auto diagContents = dummyMessage.flatten().dump(2);
		std::cerr << diagContents << std::endl;
		EXPECT_TRUE(dummyMessage.size() != 0);
		EXPECT_TRUE(dummyMessage.value("/type"_json_pointer, std::string {}).compare("response") == 0);
	}


	TEST(SIPSerializers, Test_serialize)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", {}, 1);

		registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
		registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";

		auto strsipm = sip2json::serialize(registerMessage);
		std::cerr << strsipm << std::endl;
		EXPECT_TRUE(strsipm.length() != 0);
	}

	TEST(SIPSerializers, Test_serialize_empty_mb)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", {}, 1);

		registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
		registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";
		// Set the content-type but fail to actually set the mb
		registerMessage["/mh/Content-Type"_json_pointer] = "application/sdp";
		EXPECT_ANY_THROW(sip2json::serialize(registerMessage));
	}
} // namespace siddiqsoftware