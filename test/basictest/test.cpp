/*
  Unit Tests for GoogleTest framework for sip2json
  A SIP Parser for Modern C++ / Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

#include <string>
#include <chrono>
#include <fstream>

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

	TEST(SIPParser, Test_loadTestFile)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("NOTIFY_LegDrop.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);
	}


	TEST(SIPParser, Test_EmptyBodyParseFail)
	{
		std::string emptyBuffer;
		EXPECT_ANY_THROW(sip2json::parseFromBuffer(emptyBuffer.begin(), emptyBuffer.end()));
	}

	TEST(SIPParser, Test_parse_1_fail)
	{
		auto buffer = siddiqsoftware::SIP_SAMPLE_MINIMAL_MESSAGE;
		EXPECT_ANY_THROW(sip2json::parseFromBuffer(buffer.begin(), buffer.end()));
	}

	TEST(SIPParser, Test_parse_NOTIFY_1_startline)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("NOTIFY_LegDrop.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

		// Start checking if we decoded properly..
		// METHOD: NOTIFY
		EXPECT_EQ(siddiqsoftware::METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
		EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm.value("/rl/uri"_json_pointer, std::string {}));
	}


	TEST(SIPParser, Test_parse_NOTIFY_1_headers)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("NOTIFY_LegDrop.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

		// Start checking if we decoded properly..
		// METHOD: NOTIFY
		EXPECT_EQ(siddiqsoftware::METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
		EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm.value("/rl/uri"_json_pointer, std::string {}));
		// Via is an array
		ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_EQ(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		// Call-ID
		EXPECT_EQ(sipm.getCallID(), "6732196043737il-ed-mara-01");
		// Content-Type
		EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
		// Content-Length
		EXPECT_EQ(848, sipm.getContentLength());
	}


	TEST(SIPParser, Test_parse_NOTIFY_1_headers_serialize)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("NOTIFY_LegDrop.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

		// Start checking if we decoded properly..
		// METHOD: NOTIFY
		EXPECT_EQ(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
		EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm.value("/rl/uri"_json_pointer, std::string {}));
		// Via is an array
		ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_EQ(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		// Call-ID
		EXPECT_EQ(sipm.getCallID(), "6732196043737il-ed-mara-01");
		// Content-Type
		EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
		// Content-Length
		EXPECT_EQ(848, sipm.getContentLength());

		EXPECT_EQ("jrbirge@nscorp.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
		EXPECT_EQ("267 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
		EXPECT_EQ(false, sipm.value("/mh/X-Billing-code-required"_json_pointer, true));
		EXPECT_EQ("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
				  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

		// Now, we will serialize the decoded sipm..
		auto serializedFromDecoded = sip2json::serialize(sipm);

		std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

		// So we can decode it again and ensure that we can round-trip!
		auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());
		EXPECT_EQ(METHOD_NOTIFY, sipm2.value("/rl/method"_json_pointer, std::string {}));
		EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm2.value("/rl/uri"_json_pointer, std::string {}));
		// Via is an array
		ASSERT_TRUE(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_EQ(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		// Call-ID
		EXPECT_EQ(sipm2.getCallID(), "6732196043737il-ed-mara-01");
		// Content-Type
		EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm2.getContentType());
		// Content-Length
		EXPECT_EQ(848, sipm2.getContentLength());

		EXPECT_EQ("jrbirge@nscorp.com", sipm2.value("/mh/X-control-master"_json_pointer, ""));
		EXPECT_EQ("267 NOTIFY", sipm2.value("/mh/CSeq"_json_pointer, ""));
		EXPECT_EQ(false, sipm2.value("/mh/X-Billing-code-required"_json_pointer, true));
		EXPECT_EQ("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
				  sipm2.value("/mh/X-Call-Instance-ID"_json_pointer, ""));
	}


	TEST(SIPParser, Test_parse_NOTIFY_LegDrop_body)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("NOTIFY_LegDrop.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.flatten().dump(2);

		// Start checking if we decoded properly..
		// METHOD: NOTIFY
		EXPECT_EQ(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
		EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm.value("/rl/uri"_json_pointer, std::string {}));
		// Via is an array
		ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_EQ(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		// Call-ID
		EXPECT_EQ(sipm.getCallID(), "6732196043737il-ed-mara-01");
		// Content-Type
		EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
		// Content-Length
		EXPECT_EQ(848, sipm.getContentLength());

		EXPECT_EQ("jrbirge@nscorp.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
		EXPECT_EQ("267 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
		EXPECT_EQ(false, sipm.value("/mh/X-Billing-code-required"_json_pointer, true));
		EXPECT_EQ("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
				  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

		// Check the body
		EXPECT_TRUE(!sipm.value("/mb"_json_pointer, nlohmann::json {}).empty());
		EXPECT_TRUE(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_TRUE(sipm.value("/mb/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
		// Check access_code is parsed
		EXPECT_EQ(sipm.value("/mb/sdp/0/a/access_code"_json_pointer, ""), "2873116");
		// Check leg_no is parsed
		EXPECT_EQ(sipm.value("/mb/sdp/0/a/leg_no"_json_pointer, ""), "24");
		// Check status is parsed
		EXPECT_EQ(sipm.value("/mb/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
		// Check timing is parsed into array
		EXPECT_EQ(sipm.value("/mb/sdp/0/t/0"_json_pointer, 0L), 3802534341L);
		EXPECT_EQ(sipm.value("/mb/sdp/0/t/1"_json_pointer, 0L), 3802534887L);

		EXPECT_EQ(sipm.value("/mb/sdp/0/c/dn"_json_pointer, ""), "+4044166441");
		EXPECT_EQ(sipm.value("/mb/sdp/0/c/type"_json_pointer, ""), "TN");
		EXPECT_EQ(sipm.value("/mb/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

		EXPECT_EQ(sipm.value("/mb/sdp/0/i/dn"_json_pointer, ""), "+4044166441");
		EXPECT_EQ(sipm.value("/mb/sdp/0/i/name"_json_pointer, ""), "Cell Phone   GA");
		EXPECT_EQ(sipm.value("/mb/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

		EXPECT_EQ(sipm.value("/mb/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
		EXPECT_EQ(sipm.value("/mb/sdp/0/o/subtype"_json_pointer, ""), "IP4");
		EXPECT_EQ(sipm.value("/mb/sdp/0/o/t1"_json_pointer, ""), "148492049389635");
		EXPECT_EQ(sipm.value("/mb/sdp/0/o/t2"_json_pointer, ""), "847595153");
		EXPECT_EQ(sipm.value("/mb/sdp/0/o/type"_json_pointer, ""), "IN");
		EXPECT_EQ(sipm.value("/mb/sdp/0/o/user"_json_pointer, ""), "jrbirge@nscorp.com");

		EXPECT_EQ(sipm.value("/mb/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
		EXPECT_EQ(sipm.value("/mb/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
		EXPECT_TRUE(sipm.contains("/mb/sdp/0/a/new_change"_json_pointer));
		EXPECT_TRUE(sipm.contains("/mb/sdp/0/a/far_end"_json_pointer));
		EXPECT_EQ(sipm.value("/mb/sdp/0/a/clir"_json_pointer, ""), "false");

		// Now, we will serialize the decoded sipm..
		auto serializedFromDecoded = sip2json::serialize(sipm);

		std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

		// So we can decode it again and ensure that we can round-trip!
		auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());
		EXPECT_EQ(METHOD_NOTIFY, sipm2.value("/rl/method"_json_pointer, std::string {}));
		EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm2.value("/rl/uri"_json_pointer, std::string {}));
		// Via is an array
		ASSERT_TRUE(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_EQ(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		// Call-ID
		EXPECT_EQ(sipm2.getCallID(), "6732196043737il-ed-mara-01");
		// Content-Type
		EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm2.getContentType());
		// Content-Length
		EXPECT_EQ(848, sipm2.getContentLength());

		EXPECT_EQ("jrbirge@nscorp.com", sipm2.value("/mh/X-control-master"_json_pointer, ""));
		EXPECT_EQ("267 NOTIFY", sipm2.value("/mh/CSeq"_json_pointer, ""));
		EXPECT_EQ(false, sipm2.value("/mh/X-Billing-code-required"_json_pointer, true));
		EXPECT_EQ("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
				  sipm2.value("/mh/X-Call-Instance-ID"_json_pointer, ""));
	}


	TEST(SIPParser, Test_parse_NOTIFY_CallEnd_body)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("NOTIFY_CallEnd.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.flatten().dump(2);

		auto verifyItems = [](sipmessage& sipm) {
			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			EXPECT_EQ(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, ""));
			EXPECT_EQ("sip:subscribe_to_call_events@loopup.com;machine", sipm.value("/rl/uri"_json_pointer, ""));
			// Via is an array
			ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			EXPECT_EQ(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 3);
			// Call-ID
			EXPECT_EQ(sipm.getCallID(), "119035121230567il-ed-mara-01");
			// Content-Type
			EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			EXPECT_EQ(1326, sipm.getContentLength());

			EXPECT_EQ("matthew.gabbard@stblaw.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
			EXPECT_EQ("49 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
			EXPECT_EQ(true, sipm.value("/mh/X-Billing-code-required"_json_pointer, false));
			EXPECT_EQ("MTE5MDM1MTIxMjMwNTY3aWwtZWQtbWFyYS0wMToxNTkzNjM3MTcwOjE1Njc0Ng==",
					  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			EXPECT_TRUE(!sipm.value("/mb"_json_pointer, nlohmann::json {}).empty());
			EXPECT_TRUE(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).is_array());
			EXPECT_TRUE(sipm.value("/mb/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/access_code"_json_pointer, ""), "2997255");
			// Check leg_no is parsed
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/leg_no"_json_pointer, ""), "2");
			// Check status is parsed
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
			// Check timing is parsed into array
			EXPECT_EQ(sipm.value("/mb/sdp/0/t/0"_json_pointer, 0L), 3802625984L);
			EXPECT_EQ(sipm.value("/mb/sdp/0/t/1"_json_pointer, 0L), 3802626770L);

			EXPECT_EQ(sipm.value("/mb/sdp/0/c/dn"_json_pointer, ""), "+12124553521");
			EXPECT_EQ(sipm.value("/mb/sdp/0/c/type"_json_pointer, ""), "TN");
			EXPECT_EQ(sipm.value("/mb/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			EXPECT_EQ(sipm.value("/mb/sdp/0/i/dn"_json_pointer, ""), "+12124553521");
			EXPECT_EQ(sipm.value("/mb/sdp/0/i/name"_json_pointer, ""), "Matt%20Gabbard%20-%20");
			EXPECT_EQ(sipm.value("/mb/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			EXPECT_EQ(sipm.value("/mb/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			EXPECT_EQ(sipm.value("/mb/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			EXPECT_EQ(sipm.value("/mb/sdp/0/o/t1"_json_pointer, ""), "3598380125");
			EXPECT_EQ(sipm.value("/mb/sdp/0/o/t2"_json_pointer, ""), "241");
			EXPECT_EQ(sipm.value("/mb/sdp/0/o/type"_json_pointer, ""), "IN");
			EXPECT_EQ(sipm.value("/mb/sdp/0/o/user"_json_pointer, ""), "matthew.gabbard@stblaw.com");

			EXPECT_EQ(sipm.value("/mb/sdp/0/a/cli-screening"_json_pointer, ""), "00");
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/x-ring2-smartproxy"_json_pointer, ""), "usaze-asalt01.ring2.com");
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			EXPECT_TRUE(sipm.contains("/mb/sdp/0/a/new_change"_json_pointer));
			EXPECT_TRUE(sipm.contains("/mb/sdp/0/a/far_end"_json_pointer));
			EXPECT_EQ(sipm.value("/mb/sdp/0/a/clir"_json_pointer, ""), "false:18777464263");
		};

		verifyItems(sipm);

		// Now, we will serialize the decoded sipm..
		auto serializedFromDecoded = sip2json::serialize(sipm);

		std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

		// So we can decode it again and ensure that we can round-trip!
		auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());

		//verifyItems(sipm2);

		//Forces output; disable when implementation is completed.
		//EXPECT_EQ(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).size(), 0)
		//		<< "Debugging only; disable line when completed.";
	}


	TEST(SIPParser, Test_parse_REGISTER_200_OK)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("REGISTER_200_OK.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

		// Start checking if we decoded properly..
		// Start-Line (response): SIP/2.0 200 OK
		ASSERT_EQ(200, sipm.value("/sl/statusCode"_json_pointer, 0));
		// Via is an array
		ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		EXPECT_EQ(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 1);
		ASSERT_EQ(sipm.value("/mh/Via/0"_json_pointer, ""), "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
		// Call-ID
		ASSERT_EQ(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
		// Content-Type
		//ASSERT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
		// Content-Length
		ASSERT_EQ(0, sipm.getContentLength());
		ASSERT_EQ(300, sipm.getExpires());
		ASSERT_EQ(true, sipm.value("/mh/X-subscribe-to-leg-events"_json_pointer, false));
	}

	TEST(SIPParser, Test_parse_REGISTER_1)
	{
		std::stringstream testFile;
		std::ifstream	  sampleInputFile("REGISTER_1.sip");

		if (sampleInputFile.is_open())
		{
			while (sampleInputFile.peek() != EOF)
			{
				testFile << (char)sampleInputFile.get();
			}
			sampleInputFile.close();
		}

		EXPECT_TRUE(testFile.str().length() > 0);

		auto buffer = testFile.str();
		auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

		std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

		// Start checking if we decoded properly..
		// Start-Line (response): SIP/2.0 200 OK
		ASSERT_EQ(METHOD_REGISTER, sipm.value("/rl/method"_json_pointer, ""));
		ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
		ASSERT_TRUE(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size() == 1);
		ASSERT_EQ(sipm.value("/mh/Via/0"_json_pointer, nlohmann::json {}), "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
		// Call-ID
		ASSERT_EQ(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
		// Content-Type
		//ASSERT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
		// Content-Length
		ASSERT_EQ(0, sipm.getContentLength());
		ASSERT_EQ(300, sipm.getExpires());
		ASSERT_EQ(true, sipm.value("/mh/X-subscribe-to-leg-events"_json_pointer, false));
	}

} // namespace siddiqsoftware