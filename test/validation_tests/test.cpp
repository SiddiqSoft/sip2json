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
#include <string_view>
#include <filesystem>

#define FMT_HEADER_ONLY 1
#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

#include "../../src/sip2json.hpp"
#include "../../src/sip2json_exception.hpp"

#include "gtest/gtest.h"


using namespace siddiqsoft;
using namespace std;

static std::string loadSampleFile(const std::string_view& fileName)
{
	std::stringstream testFile;
	std::ifstream	  sampleInputFile(fmt::format("../../samples/{}.sip", fileName), std::ios::binary);

	if (sampleInputFile.is_open())
	{
		testFile << sampleInputFile.rdbuf();
		sampleInputFile.close();
	}

	return testFile.str();
}


// NOLINTNEXTLINE
TEST(core_parser_tests, Test_UserAgent)
{
	auto	   ua = __func__; //NOLINT
	sipmessage sipm(METHOD_REGISTER, "sip:hello@world.com");

	try
	{
		sipm.setUserAgent(ua);
		std::clog << sip2json::serialize(sipm);
		EXPECT_TRUE(sipm.getUserAgent().find(ua) != std::string::npos);
		EXPECT_TRUE(sipm.getUserAgent().find("sip2json"s) != std::string::npos);
	}
	catch (const std::exception& e)
	{
		EXPECT_TRUE(true) << L"Got exception. " << e.what();
	}
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_meta_element)
{
	sipmessage sipm(METHOD_REGISTER, "sip:hello@world.com");

	try
	{
		std::clog << sip2json::serialize(sipm);
		EXPECT_TRUE(sipm.contains("meta"));
	}
	catch (const std::exception& e)
	{
		EXPECT_TRUE(true) << L"Got exception. " << e.what();
	}
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_sip2jsonErrors)
{
	auto ee = sip2jsonErrors::ok;

	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::empty_message;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::incomplete_buffer_for_content;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::incomplete_buffer_for_header;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::incomplete_buffer_for_parse;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::invalid_document;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::invalid_document_unsupported_content;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::invalid_document_unsupported_method;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());

	ee = sip2jsonErrors::invalid_startline;
	std::clog << fmt::format("{} - {} --> {}\n", __func__, ee, nlohmann::json(ee).dump());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_createCallId)
{
	auto ci = createCallId();
	EXPECT_TRUE(ci.length() == 44);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC1123)
{
	auto todays_date = TimeAsRFC1123();
	EXPECT_TRUE(!todays_date.empty());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC1123_args)
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


	auto todays_date = TimeAsRFC1123(std::chrono::system_clock::from_time_t(_mkgmtime(&knowntm)));
	EXPECT_TRUE(todays_date.compare("Sat, 13 Nov 2010 23:29:00 GMT") == 0);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601)
{
	auto todays_date = TimeAsISO8601();
	EXPECT_TRUE(!todays_date.empty());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601_args)
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

	auto knownDate = TimeAsISO8601(std::chrono::system_clock::from_time_t(_mkgmtime(&knowntm)));
	// Note the use of "find" instead of compare since the milliseconds are an unkown and
	// unless we create from scratch they will contain an arbitrary noise.
	EXPECT_TRUE(knownDate.find("2010-11-13T23:29:00.") == 0);
}


TEST(siphelpers, Test_createRequest)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);
	auto	   diagContents = registerMessage.flatten().dump(2);

	EXPECT_TRUE(registerMessage.size() != 0);
	EXPECT_TRUE(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(!registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	EXPECT_TRUE(registerMessage.isMessageRequest());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_createResponse)
{
	sipmessage dummyMessage(500);
	auto	   diagContents = dummyMessage.flatten().dump(2);

	EXPECT_TRUE(dummyMessage.size() != 0);
	EXPECT_TRUE(!dummyMessage.value("/s/reason"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(dummyMessage.isMessageResponse());
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_createRequest_then_response)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);

	EXPECT_TRUE(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());

	registerMessage["/h/To"_json_pointer]	   = "sip:hello@world.com";
	registerMessage["/h/Contact"_json_pointer] = "sip:hello@world.com";

	EXPECT_TRUE(registerMessage.size() != 0);
	EXPECT_TRUE(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	EXPECT_EQ(SIPMessageType::request, registerMessage.value("/s/type"_json_pointer, SIPMessageType::notspecified));

	// WARNING
	// As we're passing the registerMessage as parameter to create an inplace response message
	// the original registerMessage object will be clobbered with the items from the
	// response message create function.
	sipmessage responseMessage(200, registerMessage);

	EXPECT_TRUE(responseMessage.size() != 0);
	EXPECT_TRUE(responseMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	EXPECT_EQ(SIPMessageType::response, responseMessage.value("/s/type"_json_pointer, SIPMessageType::notspecified));
	EXPECT_TRUE(!responseMessage.value("/h/Date"_json_pointer, std::string {}).empty());

	EXPECT_EQ(registerMessage.value("/h/Call-ID"_json_pointer, "req"s), responseMessage.value("/h/Call-ID"_json_pointer, "resp"s));
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_serialize)
{
	auto ll		  = __LINE__;
	auto myCallId = createCallId();
	ll			  = __LINE__;
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", myCallId, 1);

	ll = __LINE__;
	registerMessage.setHeader("To", "sip:hello@world.com").setHeader("Contact", "sip:hello@world.com");

	try
	{
		ll			 = __LINE__;
		auto strsipm = sip2json::serialize(registerMessage);

		ll = __LINE__;
		EXPECT_TRUE(strsipm.length() != 0);

		ll				 = __LINE__;
		auto bufferStart = strsipm.begin();
		ll				 = __LINE__;
		sipmessage sipm2 = sip2json::parseFromBuffer(bufferStart, strsipm.end());
		EXPECT_TRUE(!sipm2.empty());
		EXPECT_EQ(registerMessage.getContentLength(), sipm2.getContentLength());
		EXPECT_EQ(registerMessage.getCallID(), sipm2.getCallID());

		std::clog << "\n============vvv=\n";
		std::clog << strsipm;
		std::clog << "\n============   =\n";
		std::clog << sip2json::serialize(sipm2);
		std::clog << "\n============^^^=\n";

		EXPECT_EQ(strsipm.length(), sip2json::serialize(sipm2).length());
	}
	catch (const std::exception& e)
	{
		std::clog << fmt::format("{}:Exception lastline:{} --> {}\n", __func__, ll, e.what());
		EXPECT_TRUE(true) << L"Unexpected exception.";
	}
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_serialize_empty_mb_fail)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);

	registerMessage.setHeader("To", "sip:hello@world.com")
			.setHeader("Contact", "sip:hello@world.com")
			.setHeader("Content-Type", "application/dummy");
	// This will cause serialize to throw!
	registerMessage["b"] = 0;
	EXPECT_THROW(sip2json::serialize(registerMessage), std::exception);
}


TEST(siphelpers, Test_serialize_empty_mb_fail_2)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);

	registerMessage.setHeader(
			{{"To", "sip:hello@world.com"}, {"Contact", "sip:hello@world.com"}, {"Content-Type", "application/dummy"}});
	// This will cause serialize to throw!
	registerMessage["b"] = 0;
	EXPECT_THROW(sip2json::serialize(registerMessage), std::exception);
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_serialize_empty_mb_valid)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);

	registerMessage.setHeader("To", "sip:hello@world.com")
			.setHeader("Contact", "sip:hello@world.com")
			.setHeader("Content-Type", CONTENT_TYPE_APP_SDP);
	// Should not throw; body is null despite the header being SDP there is no body element set.
	// This is a supported use-case
	EXPECT_NO_THROW(sip2json::serialize(registerMessage));
}


TEST(siphelpers, Test_serialize_empty_mb_valid_2)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);

	registerMessage.setHeader(
			{{"To", "sip:hello@world.com"}, {"Contact", "sip:hello@world.com"}, {"Content-Type", CONTENT_TYPE_APP_SDP}});
	// Should not throw; body is null despite the header being SDP there is no body element set.
	// This is a supported use-case
	EXPECT_NO_THROW(sip2json::serialize(registerMessage));
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_loadTestFile)
{
	std::stringstream testFile;
	std::ifstream	  sampleInputFile("../../samples/NOTIFY_LegDrop.sip");

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


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_parse)
{
	EXPECT_THROW(
			[]() {
				auto buffer = siddiqsoft::SIP_SAMPLE_MINIMAL_MESSAGE;
				auto bs		= buffer.begin();
				auto dummy	= sip2json::parseFromBuffer(bs, buffer.end());
			}(),
			incomplete_buffer_for_parse_error)
			<< L"Expect exception: incomplete_buffer_for_parse\n";
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_content)
{
	EXPECT_THROW(
			[]() {
				auto buffer = loadSampleFile("Test_incomplete_buffer_for_content"s); // NOLINT
				auto bs		= buffer.begin();
				auto dummy	= sip2json::parseFromBuffer(bs, buffer.end());
			}(),
			incomplete_buffer_for_content_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_header)
{
	EXPECT_THROW(
			[]() {
				auto buffer = loadSampleFile("Test_incomplete_buffer_for_header"s); // NOLINT
				auto bs		= buffer.begin();
				auto dummy	= sip2json::parseFromBuffer(bs, buffer.end());
			}(),
			incomplete_buffer_for_header_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_unsupported_contenttype)
{
	EXPECT_THROW(
			[]() {
				auto buffer = loadSampleFile("Test_unsupported_contenttype"); // NOLINT
				auto bs		= buffer.begin();
				auto dummy	= sip2json::parseFromBuffer(bs, buffer.end());
			}(),
			unsupported_contenttype_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_document)
{
	EXPECT_THROW(
			[]() {
				sipmessage sipm;
				sipm["dummy"] = "world";
				sip2json::serialize(sipm);
			}(),
			invalid_document_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_document_startline)
{
	EXPECT_THROW(
			[]() {
				sipmessage sipm("ROR", "sip:dummy@world.com");
				sip2json::serialize(sipm);
			}(),
			invalid_document_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_mb)
{
	sipmessage sipm("REGISTER", "sip:hello@world.com", createCallId(), 1);

	sipm.setHeader("To", "sip:hello@world.com")
			.setHeader("Contact", "sip:hello@world.com")
			.setHeader("Content-Type", CONTENT_TYPE_APP_SDP);

	// By default the body is null
	EXPECT_TRUE(sipm.body().empty());

	// Force an error by setting the body to something non-SDP
	sipm.body() = "<root></root>";
	EXPECT_THROW(sip2json::serialize(sipm), siddiqsoft::invalid_document_error);

	// Reset the invalid body so we can set it to SDP and recheck
	sipm.body() = nullptr; // don't erase()
	// Set some dummy value..
	sipm.setBody("/sdp/0/v"_json_pointer, 0)
			.setBody("/sdp/0/s"_json_pointer, "subject")
			.setBody("/sdp/0/a/access_code"_json_pointer, "0277777")
			.setBody("/sdp/0/t"_json_pointer, nlohmann::json {100001, 200002});

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object());

	EXPECT_EQ(0, sipm.body()["sdp"][0]["v"].get<uint32_t>());
	EXPECT_EQ("subject"s, sipm.body()["sdp"][0]["s"].get<std::string>());
	EXPECT_EQ(100001, sipm.body()["sdp"][0]["t"][0].get<uint32_t>());
	EXPECT_EQ(200002, sipm.body()["sdp"][0]["t"][1].get<uint32_t>());

	sipm.erase("b");
	EXPECT_FALSE(sipm.hasBody()) << sipm.dump(2);

	sipm.setHeader("Content-Type", CONTENT_TYPE_TEXT_PLAIN);
	EXPECT_EQ(CONTENT_TYPE_TEXT_PLAIN, sipm.getHeader<std::string>(HF_CONTENT_TYPE, "unknown")) << sipm.dump(2);
}


TEST(siphelpers, Test_empty_mb_2)
{
	using namespace std;

	sipmessage sipm("REGISTER", "sip:hello@world.com", createCallId(), 1);

	sipm.setHeader({{"To", "sip:hello@world.com"}, {"Contact", "sip:hello@world.com"}, {"Content-Type", CONTENT_TYPE_APP_SDP}});

	// By default the body is null
	EXPECT_TRUE(sipm.body().empty());

	// Force an error by setting the body to something non-SDP
	sipm.body() = "<root></root>";
	EXPECT_THROW(sip2json::serialize(sipm), siddiqsoft::invalid_document_error);

	// Reset the invalid body so we can set it to SDP and recheck
	sipm.body() = nullptr; // don't erase()
	// Set some dummy value..
	sipm.setBody({{"sdp",
				   {{{"v", 0},
					 {"s", "subject"},
					 {"t", {100001, 200002}},
					 {"a", {{"server", "media-server"}, {"access_code", "0277777"}}}},

					{{"v", 0},
					 {"s", "subject-2"},
					 {"t", {133331, 233332}},
					 {"a", {{"server", "media-server-2"}, {"access_code", "2777777"}}}}}}});

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object()) << sipm.dump(2);

	//EXPECT_TRUE(false) << sipm.body().dump(2);
	EXPECT_EQ(2, sipm.body().at("sdp").size());

	// Check first sdp
	EXPECT_EQ(0, sipm.getBodyElement("/sdp/0/v"_json_pointer, 99)) << sipm.body().dump(2);
	EXPECT_EQ("subject"s, sipm.getBodyElement("/sdp/0/s"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ(100001, sipm.getBodyElement("/sdp/0/t/0"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ(200002, sipm.getBodyElement("/sdp/0/t/1"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ("media-server"s, sipm.getBodyElement("/sdp/0/a/server"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ("0277777"s, sipm.getBodyElement("/sdp/0/a/access_code"_json_pointer, ""s)) << sipm.body().dump(2);

	// Check the second sdp
	EXPECT_EQ(0, sipm.getBodyElement("/sdp/1/v"_json_pointer, 99)) << sipm.body().dump(2);
	EXPECT_EQ("subject-2"s, sipm.getBodyElement("/sdp/1/s"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ(133331, sipm.getBodyElement("/sdp/1/t/0"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ(233332, sipm.getBodyElement("/sdp/1/t/1"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ("media-server-2"s, sipm.getBodyElement("/sdp/1/a/server"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ("2777777"s, sipm.getBodyElement("/sdp/1/a/access_code"_json_pointer, ""s)) << sipm.body().dump(2);


	sipm.erase("b");
	EXPECT_FALSE(sipm.hasBody()) << sipm.dump(2);

	sipm.setHeader("Content-Type", CONTENT_TYPE_TEXT_PLAIN);
	EXPECT_EQ(CONTENT_TYPE_TEXT_PLAIN, sipm.getHeader<std::string>(HF_CONTENT_TYPE, "unknown")) << sipm.dump(2);
}


TEST(siphelpers, Test_empty_mb_3)
{
	sipmessage sipm("REGISTER", "sip:hello@world.com", createCallId(), 1);

	sipm.setHeader("To", "sip:hello@world.com")
			.setHeader("Contact", "sip:hello@world.com")
			.setHeader("Content-Type", CONTENT_TYPE_APP_SDP);

	// By default the body is null
	EXPECT_TRUE(sipm.body().empty());

	// Force an error by setting the body to something non-SDP
	sipm.body() = "<root></root>";
	EXPECT_THROW(sip2json::serialize(sipm), siddiqsoft::invalid_document_error);

	// Reset the invalid body so we can set it to SDP and recheck
	sipm.body() = nullptr; // don't erase()
	// Set some dummy value..
	sipm.setBody("/sdp/0/v"_json_pointer, 0)
			.setBody("/sdp/0/s"_json_pointer, "sssssss")
			.setBody("/sdp/0/a/access_code"_json_pointer, "0000000")
			.setBody("/sdp/0/a/clir"_json_pointer, "false")
			.setBody("/sdp/0/t"_json_pointer, nlohmann::json {999999, 999999});

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object()) << sipm.dump(2);

	// Check first sdp
	EXPECT_EQ(0, sipm.getBodyElement("/sdp/0/v"_json_pointer, 99)) << sipm.body().dump(2);
	EXPECT_EQ("sssssss"s, sipm.getBodyElement("/sdp/0/s"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ(999999, sipm.getBodyElement("/sdp/0/t/0"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ(999999, sipm.getBodyElement("/sdp/0/t/1"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ("0000000"s, sipm.getBodyElement("/sdp/0/a/access_code"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ("false"s, sipm.getBodyElement("/sdp/0/a/clir"_json_pointer, ""s)) << sipm.body().dump(2);

	sipm.setBody({{"sdp",
				   {
						   {{"v", 0},
							{"s", "subject"},
							{"t", {100001, 200002}},
							{"a", {{"server", "media-server"}, {"access_code", "0277777"}}}},
				   }}});

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object()) << sipm.dump(2);

	//EXPECT_TRUE(false) << sipm.body().dump(2);
	EXPECT_EQ(1, sipm.body().at("sdp").size());

	// Check first sdp
	EXPECT_EQ(0, sipm.getBodyElement("/sdp/0/v"_json_pointer, 99)) << sipm.body().dump(2);
	EXPECT_EQ("subject"s, sipm.getBodyElement("/sdp/0/s"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ(100001, sipm.getBodyElement("/sdp/0/t/0"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ(200002, sipm.getBodyElement("/sdp/0/t/1"_json_pointer, 0)) << sipm.body().dump(2);
	EXPECT_EQ("media-server"s, sipm.getBodyElement("/sdp/0/a/server"_json_pointer, ""s)) << sipm.body().dump(2);
	EXPECT_EQ("0277777"s, sipm.getBodyElement("/sdp/0/a/access_code"_json_pointer, ""s)) << sipm.body().dump(2);
	// The field is removed!
	EXPECT_EQ(""s, sipm.getBodyElement("/sdp/0/a/clir"_json_pointer, ""s)) << sipm.body().dump(2);

	sipm.erase("b");
	EXPECT_FALSE(sipm.hasBody()) << sipm.dump(2);

	sipm.setHeader("Content-Type", CONTENT_TYPE_TEXT_PLAIN);
	EXPECT_EQ(CONTENT_TYPE_TEXT_PLAIN, sipm.getHeader<std::string>(HF_CONTENT_TYPE, "unknown")) << sipm.dump(2);
}


#ifdef _DEBUG


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_h)
{
	auto		callId = createCallId();
	std::string cSeq {};
	sipmessage	sipm("REGISTER", "sip:hello@world.com", callId, 1);

	sipm.setHeader("To", "sip:hello@world.com")
			.setHeader("Contact", "sip:hello@world.com")
			.setHeader("Content-Type", CONTENT_TYPE_TEXT_PLAIN)
			.setHeader("Content-Length", 0);

	std::clog << fmt::format("{} - contents\n{}\n", __func__, sip2json::serialize(sipm));

	// Check that the header exists..
	EXPECT_EQ("sip:hello@world.com", sipm.getHeader<std::string>("To"));
	EXPECT_EQ("sip:hello@world.com", sipm.getHeader<std::string>("Contact"));
	EXPECT_EQ(CONTENT_TYPE_TEXT_PLAIN, sipm.getContentType());
	EXPECT_EQ(callId, sipm.getCallID());
	EXPECT_EQ(0, sipm.getContentLength());
	EXPECT_EQ("1 REGISTER", sipm.getHeader<std::string>("CSeq"));

	// Remove the header object
	sipm.headers().erase("To");
	sipm.headers().erase("From");
	sipm.headers().erase("Contact");

	std::clog << fmt::format("{} - contents\n{}\n", __func__, sip2json::serialize(sipm));

	EXPECT_FALSE(sipm.headers().contains("To"));
	EXPECT_FALSE(sipm.headers().contains("From"));
	EXPECT_FALSE(sipm.headers().contains("Contact"));
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_message)
{
	sipmessage emptyMessage;

	try
	{
		sip2json::serialize(emptyMessage);

		ASSERT_FALSE(false) << L"Expect exception: empty_message\n";
	}
	catch (empty_message_error& e)
	{
		std::clog << e.what();
		EXPECT_TRUE(e.errCode == sip2jsonErrors::empty_message);
	}
	catch (std::exception& e)
	{
		std::clog << e.what();
		ASSERT_FALSE(false) << L"unknown/unhandled exception.";
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_startline)
{
	auto buffer = loadSampleFile(__func__); // NOLINT

	try
	{
		auto bs	   = buffer.begin();
		auto dummy = sip2json::parseFromBuffer(bs, buffer.end());
		EXPECT_FALSE(false) << L"Expect exception: invalid_startline\n";
	}
	catch (invalid_startline_error& e)
	{
		std::clog << e.what();
		EXPECT_TRUE(e.errCode == sip2jsonErrors::invalid_startline);
	}
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_check_isMessageTypeRequest)
{
	sipmessage sipm("INVITE", "sip:hello@world.com", createCallId(), 1);

	EXPECT_TRUE(sipm.size() != 0);
	EXPECT_TRUE(!sipm.value("/h/Date"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(!sipm.value("/h/Call-ID"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(sipm.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	EXPECT_TRUE(sipm.isMessageRequest());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_check_isMessageTypeResponse)
{
	sipmessage sipm(608);

	EXPECT_TRUE(sipm.size() != 0);
	EXPECT_TRUE(!sipm.value("/s/reason"_json_pointer, std::string {}).empty());
	EXPECT_EQ(SIPMessageType::response, sipm.value("/s/type"_json_pointer, SIPMessageType::notspecified));
	EXPECT_EQ(608, sipm.getStatusCode());
	EXPECT_TRUE(sipm.isMessageResponse());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_check_getContentType)
{
	sipmessage sipm("INVITE", "sip:hello@world.com", createCallId(), 1);

	EXPECT_TRUE(sipm.size() != 0);
	EXPECT_TRUE(!sipm.value("/h/Date"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(!sipm.value("/h/Call-ID"_json_pointer, std::string {}).empty());
	EXPECT_TRUE(sipm.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	EXPECT_TRUE(sipm.isMessageRequest());

	// Note the "Content-type" and "Content-Type"; either way the method should return the value.
	sipm["h"]["Content-type"] = "test/test";
	EXPECT_EQ("test/test", sipm.getContentType());

	sipm["h"]["Content-Type"] = "test/test2";
	EXPECT_EQ("test/test2", sipm.getContentType());

	sipm["h"].erase("Content-Type");
	sipm["h"].erase("Content-type");
	EXPECT_TRUE(sipm.getContentType().empty());
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_header_method)
{
	sipmessage sipm("REGISTER", "sip:hello@world.com", createCallId(), 1);

	sipm.setHeader("To", "sip:hello@world.com")
			.setHeader("Contact", "sip:hello@world.com")
			.setHeader("Content-Type", CONTENT_TYPE_APP_SDP);

	EXPECT_EQ("sip:hello@world.com", sipm.getHeader<std::string>("Contact"));
	EXPECT_EQ(CONTENT_TYPE_APP_SDP, sipm.getContentType());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_body_method)
{
	using namespace std;

	auto	   iName = "Test body HЯ⾀ method"s;
	sipmessage sipm("INVITE", "sip:hello@world.com", createCallId(), 1);

	sipm.setHeader("Content-Type", CONTENT_TYPE_APP_SDP);

	EXPECT_TRUE(sipm.body().empty());

	// Set dummy but all required values! v, 0, s, t, m
	sipm.setBody("/sdp/0/v"_json_pointer, 0)
			.setBody("/sdp/0/o"_json_pointer,
					 nlohmann::json {{"user", "sip:hello@world.com"},
									 {"type", "IN"},
									 {"subtype", "IP4"},
									 {"host", "host.name.com"},
									 {"t1", "900001"},	// these must be string
									 {"t2", "900009"}}) // must be string
			.setBody("/sdp/0/s"_json_pointer, "subject")
			.setBody("/sdp/0/i/name"_json_pointer, iName)
			.setBody("/sdp/0/i/type"_json_pointer, "CallByPhone-URL")
			.setBody("/sdp/0/i/dn"_json_pointer, "16668661212")
			.setBody("/sdp/0/a/access_code"_json_pointer, "0277777")
			.setBody("/sdp/0/t"_json_pointer, nlohmann::json {100001, 200002})
			.setBody("/sdp/0/m"_json_pointer, "audio voice");

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object());

	EXPECT_EQ(0, sipm.body()["sdp"][0]["v"].get<uint32_t>());
	EXPECT_EQ("subject", sipm.body()["sdp"][0]["s"].get<std::string>());
	EXPECT_EQ(100001, sipm.body()["sdp"][0]["t"][0].get<uint32_t>());
	EXPECT_EQ(200002, sipm.body()["sdp"][0]["t"][1].get<uint32_t>());

	try
	{
		auto buffer = sip2json::serialize(sipm);

		// Returns the remaining buffer!
		buffer = sip2json::parseAsync(buffer, [&](auto&& dsipm) {
			// We Should check for the iline
			EXPECT_EQ(iName, dsipm.body().value("/sdp/0/i/name"_json_pointer, "")) << dsipm.dump(2);

			EXPECT_EQ(0, dsipm.body()["sdp"][0]["v"].get<uint32_t>());
			EXPECT_EQ("subject", dsipm.body()["sdp"][0]["s"].get<std::string>());
			EXPECT_EQ(100001, dsipm.body()["sdp"][0]["t"][0].get<uint32_t>());
			EXPECT_EQ(200002, dsipm.body()["sdp"][0]["t"][1].get<uint32_t>());
		});
		EXPECT_EQ(0, buffer.length()) << buffer;
	}
	catch (const std::exception& e)
	{
		std::clog << fmt::format("{}:Exception: {}\n", __func__, e.what());
		EXPECT_FALSE(false) << L"Unexpected exception.";
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_async_invalid_startline)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile("Test_invalid_startline"); // NOLINT
	auto bs		  = buffer.begin();
	std::clog << buffer << "\n";
	sip2json::parseAsync(buffer, {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::invalid_startline);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_async_incomplete_buffer_for_parse)
{
	auto buffer	  = siddiqsoft::SIP_SAMPLE_MINIMAL_MESSAGE;
	bool passTest = false;
	auto bs		  = buffer.begin();
	sip2json::parseAsync(buffer, {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_parse);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_async_incomplete_buffer_for_content)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile("Test_incomplete_buffer_for_content"); // NOLINT
	auto bs		  = buffer.begin();

	ASSERT_TRUE(buffer.size() > 0) << "Buffer contents: [[ " << buffer << " ]]";

	sip2json::parseAsync(buffer, {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		// We would get multiple exceptions/callbacks so we should watch out for our specific code.
		std::clog << fmt::format("Test_incomplete_buffer_for_content: got error:{}\n", e.errCode);
		if (e.errCode == sip2jsonErrors::incomplete_buffer_for_content) passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_async_incomplete_buffer_for_header)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile("Test_incomplete_buffer_for_header"); // NOLINT
	auto bs		  = buffer.begin();
	sip2json::parseAsync(buffer, {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_header);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_async_unsupported_contenttype)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile("Test_unsupported_contenttype"); // NOLINT
	auto bs		  = buffer.begin();
	sip2json::parseAsync(buffer, {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::unsupported_contenttype);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_unknown_exception)
{
	bool pass1Test = false;
	bool pass2Test = false;
	auto buffer	   = loadSampleFile("REGISTER_1"); // NOLINT
	auto bs		   = buffer.begin();

	// Deliberately throw an exception in the parse-callback so we can ensure that the error-callback is invoked.
	sip2json::parseAsync(
			buffer,
			[&](sipmessage&& sipm) {
				// We should parse valid message and get our callback.
				pass1Test = true;
				// Throw so we can get the error-callback triggered.
				throw 666;
			},
			[&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
				if (pass1Test) pass2Test = (e.errCode == sip2jsonErrors::unknown);
			});

	EXPECT_TRUE(pass1Test) << L"First stage callback not invoked.";
	EXPECT_TRUE(pass2Test) << L"Error callback not invoked.";
}

#endif


// NOLINTNEXTLINE
TEST(validation, Test_extension_aras)
{
	auto buffer		= loadSampleFile("Test_extension_aras"); // NOLINT
	auto bs			= buffer.begin();
	auto parseCount = 0;


	buffer = sip2json::parseAsync(
			buffer,
			[&](auto&& sipm) {
				switch (parseCount++)
				{
				case 0:
				{
					EXPECT_EQ(2, sipm["/h/Via"_json_pointer].size()) << sipm.dump(2);
					EXPECT_EQ(1153, sipm.getContentLength());
					EXPECT_EQ("+14152264822x,0830423985", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
					EXPECT_EQ("1.11", sipm.value("/b/sdp/0/a/x-ring2-coords"_json_pointer, "")) << sipm.dump(2);
				}
				break;
				case 1:
				{
					//EXPECT_TRUE(false) << sipm.dump(2); // Diagnostics only

					EXPECT_EQ(2, sipm["/h/Via"_json_pointer].size()) << sipm.dump(2);
					EXPECT_EQ(1302, sipm.getContentLength());
					EXPECT_EQ("+14152264822x,0830423985", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
					EXPECT_TRUE(sipm.contains("/b/sdp/0/a/x-ring2-coords"_json_pointer)) << sipm.dump(2);
					EXPECT_EQ(2, sipm["/b/sdp/0/a/x-ring2-coords"_json_pointer].size()) << sipm.dump(2);
					EXPECT_EQ("2.11", sipm.value("/b/sdp/0/a/x-ring2-coords/0"_json_pointer, "")) << sipm.dump(2);
					EXPECT_EQ("2.22", sipm.value("/b/sdp/0/a/x-ring2-coords/1"_json_pointer, "")) << sipm.dump(2);
				}
				break;
				case 2:
				{
					//EXPECT_TRUE(false) << sipm.dump(2); // Diagnostics only

					EXPECT_EQ(2, sipm["/h/Via"_json_pointer].size()) << sipm.dump(2);
					EXPECT_EQ(1355, sipm.getContentLength());
					EXPECT_EQ("+14152264822x,0830423985", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
					EXPECT_TRUE(sipm.contains("/b/sdp/0/a/x-ring2-coords"_json_pointer)) << sipm.dump(2);
					EXPECT_EQ(3, sipm["/b/sdp/0/a/x-ring2-coords"_json_pointer].size()) << sipm.dump(2);
					EXPECT_EQ("3.11", sipm.value("/b/sdp/0/a/x-ring2-coords/0"_json_pointer, "")) << sipm.dump(2);
					EXPECT_EQ("3.22", sipm.value("/b/sdp/0/a/x-ring2-coords/1"_json_pointer, "")) << sipm.dump(2);
					EXPECT_EQ("3.33", sipm.value("/b/sdp/0/a/x-ring2-coords/2"_json_pointer, "")) << sipm.dump(2);
				}
				break;
				};
			},
			[](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
				EXPECT_FALSE(true) << e.what();
			}

	);

	// There must be atleast 3 NOTIFY from the buffer
	EXPECT_EQ(3, parseCount) << "Should get 3 items instead of " << parseCount << std::endl;
}


// NOLINTNEXTLINE
TEST(validation, Test_extension_nelson)
{
	auto buffer		= loadSampleFile("Test_extension_nelson"); // NOLINT
	auto bs			= buffer.begin();
	auto parseCount = 0;


	sip2json::parseAsync(buffer, [&](siddiqsoft::sipmessage&& sipm) {
		switch (parseCount++)
		{
		case 0:
			EXPECT_EQ(1089, sipm.getContentLength());
			EXPECT_EQ("+19172084495", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			break;
		case 1:
			EXPECT_EQ(1216, sipm.getContentLength());
			EXPECT_EQ("+19172084495", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			break;
		case 2:
			EXPECT_EQ(2054, sipm.getContentLength());
			EXPECT_EQ(2, sipm.body()["sdp"].size());
			EXPECT_EQ("+19172084495", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			EXPECT_EQ("+448443351801", sipm.getBodyElement("/sdp/1/c/dn"_json_pointer, ""s));
			break;
		case 3:
			EXPECT_EQ(1002, sipm.getContentLength());
			EXPECT_EQ("+448443351801", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			break;
		case 4:
			EXPECT_EQ(1104, sipm.getContentLength());
			EXPECT_EQ("+448443351801", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			break;
		case 5:
			EXPECT_EQ(1097, sipm.getContentLength());
			EXPECT_EQ("+442080163962x,7415135063", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			break;
		case 6:
			EXPECT_EQ(1349, sipm.getContentLength());
			EXPECT_EQ("+442080163962x,7415135063", sipm.getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
			break;
		};
	});

	// There must be atleast 7 NOTIFY from the buffer
	EXPECT_EQ(7, parseCount);
}


// NOLINTNEXTLINE
TEST(validation, Test_parse_invalid_string_position)
{
	auto buffer		 = loadSampleFile("Test_parse_invalid_string_position"); // NOLINT
	auto bs			 = buffer.begin();
	auto parseResult = sip2json::parse(bs, buffer.end());

	// There must be atleast 3 NOTIFY from the buffer
	ASSERT_EQ(3, parseResult.size());

	// Frame 1
	EXPECT_EQ(1062, parseResult[0].getContentLength());
	EXPECT_EQ("+19362329469", parseResult[0].getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));

	// Frame 2
	EXPECT_EQ(1189, parseResult[1].getContentLength());
	EXPECT_EQ("+19362329469", parseResult[1].getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));

	// Frame 3
	EXPECT_EQ(1192, parseResult[2].getContentLength());
	EXPECT_EQ("+19362329469", parseResult[2].getBodyElement("/sdp/0/c/dn"_json_pointer, ""s));
}
