/*
  Unit Tests for GoogleTest framework for sip2json
  A SIP Parser for Modern C++ / Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

#include <string>
#include <chrono>

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

#include "../../src/sip2json.hpp"

#include "gtest/gtest.h"


namespace siddiqsoftware
{
	TEST(Helpers, Test_createCallId)
	{
		auto ci = createCallId();
		EXPECT_TRUE(ci.length() == 44);
	}


	TEST(DateTime, Test_getRFC1123)
	{
		auto todays_date = getRFC1123();
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


		auto todays_date = getRFC1123(std::chrono::system_clock::from_time_t(_mkgmtime(&knowntm)));
		EXPECT_TRUE(todays_date.compare("Sat, 13 Nov 2010 23:29:00 GMT") == 0) << todays_date;
	}

	TEST(DateTime, Test_getISO8601)
	{
		auto todays_date = getISO8601();
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

		auto knownDate = getISO8601(std::chrono::system_clock::from_time_t(_mkgmtime(&knowntm)));
		// Note the use of "find" instead of compare since the milliseconds are an unkown and
		// unless we create from scratch they will contain an arbitrary noise.
		EXPECT_TRUE(knownDate.find("2010-11-13T23:29:00.") == 0) << knownDate;
	}


	TEST(SIPHelpers, Test_createRequest)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);
		auto diagContents	 = registerMessage.flatten().dump(2);
		std::cerr << diagContents << std::endl;
		EXPECT_TRUE(registerMessage.size() != 0);
		EXPECT_TRUE(!registerMessage.value("/mh/Date"_json_pointer, std::string {}).empty());
		EXPECT_TRUE(!registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).empty());
		EXPECT_TRUE(registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
		EXPECT_TRUE(registerMessage.value("/type"_json_pointer, std::string {}).find("request") != std::string::npos);
	}


	TEST(SIPHelpers, Test_createResponse)
	{
		auto dummyMessage = sip2json::createResponse(500);
		auto diagContents = dummyMessage.flatten().dump(2);
		std::cerr << diagContents << std::endl;
		EXPECT_TRUE(dummyMessage.size() != 0);
		EXPECT_TRUE(!dummyMessage.value("/sl/reason"_json_pointer, std::string {}).empty());
		EXPECT_TRUE(dummyMessage.value("/type"_json_pointer, std::string {}).compare(sip2json::MessageTypeResponse) == 0);
		EXPECT_TRUE(!dummyMessage.value("/mh/Date"_json_pointer, std::string {}).empty());
	}


	TEST(SIPHelpers, Test_createRequest_then_response)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);
		std::cerr << "POST createRequest(): registerMessage:" << registerMessage.flatten().dump(2) << std::endl;
		EXPECT_TRUE(!registerMessage.value("/mh/Date"_json_pointer, std::string {}).empty());

		registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
		registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";

		EXPECT_TRUE(registerMessage.size() != 0);
		EXPECT_TRUE(registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
		EXPECT_EQ(registerMessage.value("/type"_json_pointer, std::string {}), sip2json::MessageTypeRequest)
				<< "registerMessage type is:" << registerMessage.value("/type"_json_pointer, std::string {}) << "---";

		// WARNING
		// As we're passing the registerMessage as parameter to create an inplace response message
		// the original registerMessage object will be clobbered with the items from the
		// response message create function.
		auto responseMessage = sip2json::createResponse(200, registerMessage);

		EXPECT_TRUE(responseMessage.size() != 0);
		EXPECT_TRUE(responseMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
		EXPECT_EQ(responseMessage.value("/type"_json_pointer, std::string {}), sip2json::MessageTypeResponse)
				<< "responseMessage type:" << responseMessage.value("/type"_json_pointer, std::string {}) << "----";
		EXPECT_TRUE(!responseMessage.value("/mh/Date"_json_pointer, std::string {}).empty());

		std::cerr << "After response; registerMessage:" << registerMessage.flatten().dump(2) << std::endl;
		std::cerr << "After response; registerMessage serialized:" << sip2json::serialize(registerMessage) << std::endl;

		std::cerr << "After response; responseMessage:" << responseMessage.flatten().dump(2) << std::endl;
		std::cerr << "After response; responseMessage serialized:" << sip2json::serialize(responseMessage) << std::endl;


		EXPECT_EQ(registerMessage.value("/mh/Call-ID"_json_pointer, "req"),
				  responseMessage.value("/mh/Call-ID"_json_pointer, "resp"))
				<< "Response must have the same Call-ID as request";
	}

	TEST(SIPSerializers, Test_serialize)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);

		registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
		registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";

		auto strsipm = sip2json::serialize(registerMessage);
		std::cerr << strsipm << std::endl;
		EXPECT_TRUE(strsipm.length() != 0) << "Serialized message must be non-empty.";
	}

	TEST(SIPSerializers, Test_serialize_empty_mb)
	{
		auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);

		registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
		registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";
		// Set the content-type but fail to actually set the mb
		registerMessage["/mh/Content-Type"_json_pointer] = "application/sdp";
		EXPECT_ANY_THROW(sip2json::serialize(registerMessage));
	}
} // namespace siddiqsoftware