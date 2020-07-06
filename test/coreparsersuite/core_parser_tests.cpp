/*
  A SIP Parser for Modern C++ / Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

#include <string>
#include <chrono>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

#include "../../src/sip2json.hpp"

#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace siddiqsoftware;

namespace test_suite
{
	// NOLINTNEXTLINE
	TEST_CLASS(core_parser_tests)
	{
	public:
		bool dummy;

		// NOLINTNEXTLINE
		TEST_METHOD(Test_createCallId)
		{
			auto ci = createCallId();
			Assert::IsTrue(ci.length() == 44);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_getRFC1123)
		{
			auto todays_date = getRFC1123();
			Assert::IsTrue(!todays_date.empty());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_getRFC1123_args)
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
			Assert::IsTrue(todays_date.compare("Sat, 13 Nov 2010 23:29:00 GMT") == 0);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_getISO8601)
		{
			auto todays_date = getISO8601();
			Assert::IsTrue(!todays_date.empty());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_getISO8601_args)
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
			Assert::IsTrue(knownDate.find("2010-11-13T23:29:00.") == 0);
		}
	}; // Helpers


	// NOLINTNEXTLINE
	TEST_CLASS(SIPHelpers)
	{
	public:
		bool dummy;

		// NOLINTNEXTLINE
		TEST_METHOD(Test_createRequest)
		{
			auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);
			auto diagContents	 = registerMessage.flatten().dump(2);
			std::cerr << diagContents << std::endl;
			Assert::IsTrue(registerMessage.size() != 0);
			Assert::IsTrue(!registerMessage.value("/mh/Date"_json_pointer, std::string {}).empty());
			Assert::IsTrue(!registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).empty());
			Assert::IsTrue(registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::IsTrue(registerMessage.value("/type"_json_pointer, std::string {}).find("request") != std::string::npos);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_createResponse)
		{
			auto dummyMessage = sip2json::createResponse(500);
			auto diagContents = dummyMessage.flatten().dump(2);
			std::cerr << diagContents << std::endl;
			Assert::IsTrue(dummyMessage.size() != 0);
			Assert::IsTrue(!dummyMessage.value("/sl/reason"_json_pointer, std::string {}).empty());
			Assert::IsTrue(dummyMessage.value("/type"_json_pointer, std::string {}).compare(sip2json::MessageTypeResponse) == 0);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_createRequest_then_response)
		{
			auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);
			std::cerr << "POST createRequest(): registerMessage:" << registerMessage.flatten().dump(2) << std::endl;
			Assert::IsTrue(!registerMessage.value("/mh/Date"_json_pointer, std::string {}).empty());

			registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
			registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";

			Assert::IsTrue(registerMessage.size() != 0);
			Assert::IsTrue(registerMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::AreEqual<std::string>(registerMessage.value("/type"_json_pointer, std::string {}),
										  sip2json::MessageTypeRequest);

			// WARNING
			// As we're passing the registerMessage as parameter to create an inplace response message
			// the original registerMessage object will be clobbered with the items from the
			// response message create function.
			auto responseMessage = sip2json::createResponse(200, registerMessage);

			Assert::IsTrue(responseMessage.size() != 0);
			Assert::IsTrue(responseMessage.value("/mh/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::AreEqual<std::string>(responseMessage.value("/type"_json_pointer, std::string {}),
										  sip2json::MessageTypeResponse);
			Assert::IsTrue(!responseMessage.value("/mh/Date"_json_pointer, std::string {}).empty());

			std::cerr << "After response; registerMessage:" << registerMessage.flatten().dump(2) << std::endl;
			std::cerr << "After response; registerMessage serialized:" << sip2json::serialize(registerMessage) << std::endl;

			std::cerr << "After response; responseMessage:" << responseMessage.flatten().dump(2) << std::endl;
			std::cerr << "After response; responseMessage serialized:" << sip2json::serialize(responseMessage) << std::endl;


			Assert::AreEqual<std::string>(registerMessage.value("/mh/Call-ID"_json_pointer, "req"),
										  responseMessage.value("/mh/Call-ID"_json_pointer, "resp"));
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_serialize)
		{
			auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);

			registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
			registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";

			auto strsipm = sip2json::serialize(registerMessage);
			std::cerr << strsipm << std::endl;
			Assert::IsTrue(strsipm.length() != 0);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_serialize_empty_mb)
		{
			auto registerMessage = sip2json::createRequest("REGISTER", "sip:hello@world.com", createCallId(), 1);

			registerMessage["/mh/To"_json_pointer]		= "sip:hello@world.com";
			registerMessage["/mh/Contact"_json_pointer] = "sip:hello@world.com";
			// Set the content-type but fail to actually set the mb
			registerMessage["/mh/Content-Type"_json_pointer] = "application/sdp";
			Assert::ExpectException<std::exception>([&]() { sip2json::serialize(registerMessage); });
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_loadTestFile)
		{
			std::stringstream testFile;
			std::ifstream	  sampleInputFile("../test/samples/NOTIFY_LegDrop.sip");

			if (sampleInputFile.is_open())
			{
				while (sampleInputFile.peek() != EOF)
				{
					testFile << (char)sampleInputFile.get();
				}
				sampleInputFile.close();
			}

			Assert::IsTrue(testFile.str().length() > 0);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_1_fail)
		{
			auto buffer = siddiqsoftware::SIP_SAMPLE_MINIMAL_MESSAGE;
			Assert::ExpectException<std::exception>([&]() {
				auto bs = buffer.begin();
				sip2json::parseFromBuffer(bs, buffer.end());
			});
		}
	}; // SIPHelpers
} // namespace siddiqsoftware