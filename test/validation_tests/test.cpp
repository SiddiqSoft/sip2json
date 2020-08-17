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


#define FMT_HEADER_ONLY 1
#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

#include "../../src/sip2json.hpp"
#include "../../src/sip2json_exception.hpp"

#include "gtest/gtest.h"


using namespace siddiqsoftware;
using namespace std;

static std::string loadSampleFile(const std::string_view& fileName)
{
	std::stringstream testFile;
	std::ifstream	  sampleInputFile(fmt::format("../test/samples/{}.sip", fileName), std::ios::binary);

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

#ifdef WORKING

// NOLINTNEXTLINE
TEST(siphelpers, Test_createRequest_then_response)
{
	sipmessage registerMessage("REGISTER", "sip:hello@world.com", createCallId(), 1);

	EXPECT_TRUE(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());

	registerMessage["/h/To"_json_pointer]	   = "sip:hello@world.com";
	registerMessage["/h/Contact"_json_pointer] = "sip:hello@world.com";

	EXPECT_TRUE(registerMessage.size() != 0);
	EXPECT_TRUE(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	Assert::AreEqual<SIPMessageType>(SIPMessageType::request,
									 registerMessage.value("/s/type"_json_pointer, SIPMessageType::notspecified));

	// WARNING
	// As we're passing the registerMessage as parameter to create an inplace response message
	// the original registerMessage object will be clobbered with the items from the
	// response message create function.
	sipmessage responseMessage(200, registerMessage);

	EXPECT_TRUE(responseMessage.size() != 0);
	EXPECT_TRUE(responseMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
	Assert::AreEqual<SIPMessageType>(SIPMessageType::response,
									 responseMessage.value("/s/type"_json_pointer, SIPMessageType::notspecified));
	EXPECT_TRUE(!responseMessage.value("/h/Date"_json_pointer, std::string {}).empty());

	Assert::AreEqual<std::string>(registerMessage.value("/h/Call-ID"_json_pointer, "req"),
								  responseMessage.value("/h/Call-ID"_json_pointer, "resp"));
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
		Assert::AreEqual(registerMessage.getContentLength(), sipm2.getContentLength());
		Assert::AreEqual(registerMessage.getCallID(), sipm2.getCallID());

			std::clog <<"\n============vvv=\n");
			std::clog <<strsipm.c_str());
			std::clog <<"\n============   =\n");
			std::clog <<sip2json::serialize(sipm2).c_str());
			std::clog <<"\n============^^^=\n");

			Assert::AreEqual(strsipm.length(), sip2json::serialize(sipm2).length());
	}
	catch (const std::exception& e)
	{
			std::clog <<fmt::format("{}:Exception lastline:{} --> {}\n", __func__, ll, e.what()).c_str());
			Assert::Fail(L"Unexpected exception.");
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
	Assert::ExpectException<std::exception>([&]() { sip2json::serialize(registerMessage); });
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
	sip2json::serialize(registerMessage);
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_loadTestFile)
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

	EXPECT_TRUE(testFile.str().length() > 0);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_parse)
{
	auto buffer = siddiqsoftware::SIP_SAMPLE_MINIMAL_MESSAGE;
	try
	{
		auto bs = buffer.begin();
		sip2json::parseFromBuffer(bs, buffer.end());
		Assert::Fail(L"Expect exception: incomplete_buffer_for_parse\n");
	}
	catch (incomplete_buffer_for_parse_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_parse);
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_content)
{
	auto buffer = loadSampleFile(__func__); // NOLINT
	try
	{
		auto bs = buffer.begin();
		sip2json::parseFromBuffer(bs, buffer.end());
		Assert::Fail(L"Expect exception: incomplete_buffer_for_content\n");
	}
	catch (incomplete_buffer_for_content_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_content);
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_header)
{
	auto buffer = loadSampleFile(__func__); // NOLINT
	try
	{
		auto bs = buffer.begin();
		sip2json::parseFromBuffer(bs, buffer.end());
		Assert::Fail(L"Expect exception: incomplete_buffer_for_header\n");
	}
	catch (incomplete_buffer_for_header_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_header);
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_unsupported_contenttype)
{
	auto buffer = loadSampleFile(__func__); // NOLINT
	try
	{
		auto bs = buffer.begin();
		sip2json::parseFromBuffer(bs, buffer.end());
		Assert::Fail(L"Expect exception: unsupported_contenttype\n");
	}
	catch (unsupported_contenttype_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::unsupported_contenttype);
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_document)
{
	sipmessage sipm;

	try
	{
		sipm["dummy"] = "world";
		sip2json::serialize(sipm);
		Assert::Fail(L"Expect exception: invalid_document\n");
	}
	catch (invalid_document_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::invalid_document);
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_document_startline)
{
	sipmessage sipm("ROR", "sip:dummy@world.com");

	try
	{
		sip2json::serialize(sipm);
		Assert::Fail(L"Expect exception: invalid_document\n");
	}
	catch (invalid_document_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::invalid_document);
	}
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
	Assert::ExpectException<siddiqsoftware::invalid_document_error>([&]() {
		try
		{
				std::clog <<sip2json::serialize(sipm).c_str());
		}
		catch (siddiqsoftware::invalid_document_error& e)
		{
				std::clog <<e.what());
				throw;
		}
	});

	// Reset the invalid body so we can set it to SDP and recheck
	sipm.body() = nullptr; // dont' erase()
	// Set some dummy value..
	sipm.body("/sdp/0/v"_json_pointer, 0)
			.body("/sdp/0/s"_json_pointer, "subject")
			.body("/sdp/0/a/access_code"_json_pointer, "0277777")
			.body("/sdp/0/t"_json_pointer, nlohmann::json {100001, 200002});

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object());

		std::clog <<fmt::format("{} - Contents\n{}\n", __func__, sipm.dump(2)).c_str());

		Assert::AreEqual<uint32_t>(0, sipm.body()["sdp"][0]["v"].get<uint32_t>());
		Assert::AreEqual<std::string>("subject", sipm.body()["sdp"][0]["s"].get<std::string>());
		Assert::AreEqual<uint32_t>(100001, sipm.body()["sdp"][0]["t"][0].get<uint32_t>());
		Assert::AreEqual<uint32_t>(200002, sipm.body()["sdp"][0]["t"][1].get<uint32_t>());

		sipm.erase("b");
		sipm.setHeader("Content-Type", CONTENT_TYPE_TEXT_PLAIN);
		std::clog <<"\n");
		std::clog <<sip2json::serialize(sipm).c_str());
}


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

		std::clog <<fmt::format("{} - contents\n{}\n", __func__, sip2json::serialize(sipm)).c_str());

		// Check that the header exists..
		Assert::AreEqual<std::string>("sip:hello@world.com", sipm.getHeader<std::string>("To"));
		Assert::AreEqual<std::string>("sip:hello@world.com", sipm.getHeader<std::string>("Contact"));
		Assert::AreEqual<std::string>(CONTENT_TYPE_TEXT_PLAIN, sipm.getContentType());
		Assert::AreEqual<std::string>(callId, sipm.getCallID());
		Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
		Assert::AreEqual<std::string>("1 REGISTER", sipm.getHeader<std::string>("CSeq"));

		// Remove the header object
		sipm.headers().erase("To");
		sipm.headers().erase("From");
		sipm.headers().erase("Contact");

		std::clog <<fmt::format("{} - contents\n{}\n", __func__, sip2json::serialize(sipm)).c_str());

		Assert::IsFalse(sipm.headers().contains("To"));
		Assert::IsFalse(sipm.headers().contains("From"));
		Assert::IsFalse(sipm.headers().contains("Contact"));
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_message)
{
	sipmessage emptyMessage;

	try
	{
		sip2json::serialize(emptyMessage);
		Assert::Fail(L"Expect exception: empty_message\n");
	}
	catch (empty_message_error& e)
	{
			std::clog <<e.what());
			EXPECT_TRUE(e.errCode == sip2jsonErrors::empty_message);
	}
	catch (std::exception& e)
	{
			std::clog <<e.what());
			Assert::Fail(L"unknown/unhandled exception.");
	}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_startline)
{
	auto buffer = loadSampleFile(__func__); // NOLINT

	try
	{
		auto bs = buffer.begin();
		sip2json::parseFromBuffer(bs, buffer.end());
		Assert::Fail(L"Expect exception: invalid_startline\n");
	}
	catch (invalid_startline_error& e)
	{
			std::clog <<e.what());
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
	Assert::AreEqual<SIPMessageType>(SIPMessageType::response, sipm.value("/s/type"_json_pointer, SIPMessageType::notspecified));
	Assert::AreEqual<uint32_t>(608, sipm.getStatusCode());
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
	Assert::AreEqual<std::string>("test/test", sipm.getContentType());

	sipm["h"]["Content-Type"] = "test/test2";
	Assert::AreEqual<std::string>("test/test2", sipm.getContentType());

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

	Assert::AreEqual<std::string>("sip:hello@world.com", sipm.getHeader<std::string>("Contact"));
	Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_body_method)
{
	sipmessage sipm("REGISTER", "sip:hello@world.com", createCallId(), 1);

	sipm.setHeader("Content-Type", CONTENT_TYPE_APP_SDP);

	EXPECT_TRUE(sipm.body().empty());

	// Set dummy but all required values! v, 0, s, t, m
	sipm.body("/sdp/0/v"_json_pointer, 0)
			.body("/sdp/0/o"_json_pointer,
				  nlohmann::json {{"user", "sip:hello@world.com"},
								  {"type", "IN"},
								  {"subtype", "IP4"},
								  {"host", "host.name.com"},
								  {"t1", "900001"},	 // these must be string
								  {"t2", "900009"}}) // must be string
			.body("/sdp/0/s"_json_pointer, "subject")
			.body("/sdp/0/a/access_code"_json_pointer, "0277777")
			.body("/sdp/0/t"_json_pointer, nlohmann::json {100001, 200002})
			.body("/sdp/0/m"_json_pointer, "audio voice");

	// Check again for the body. it should be non-null
	EXPECT_TRUE(sipm.body().is_object());

		std::clog <<fmt::format("{} - Contents\n{}\n", __func__, sipm.dump(2)).c_str());

		Assert::AreEqual<uint32_t>(0, sipm.body()["sdp"][0]["v"].get<uint32_t>());
		Assert::AreEqual<std::string>("subject", sipm.body()["sdp"][0]["s"].get<std::string>());
		Assert::AreEqual<uint32_t>(100001, sipm.body()["sdp"][0]["t"][0].get<uint32_t>());
		Assert::AreEqual<uint32_t>(200002, sipm.body()["sdp"][0]["t"][1].get<uint32_t>());

		try
		{
			std::clog <<sip2json::serialize(sipm).c_str());
		}
		catch (const std::exception& e)
		{
			std::clog <<fmt::format("{}:Exception: {}\n", __func__, e.what()).c_str());
			Assert::Fail(L"Unexpected exception.");
		}
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_startline)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile(__func__); // NOLINT
	auto bs		  = buffer.begin();
	sip2json::parse(bs, buffer.end(), {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::invalid_startline);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_parse)
{
	auto buffer	  = siddiqsoftware::SIP_SAMPLE_MINIMAL_MESSAGE;
	bool passTest = false;
	auto bs		  = buffer.begin();
	sip2json::parse(bs, buffer.end(), {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_parse);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_content)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile(__func__); // NOLINT
	auto bs		  = buffer.begin();
	sip2json::parse(bs, buffer.end(), {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		// We would get multiple exceptions/callbacks so we should watch out for our specific code.
					std::clog <<fmt::format("Test_incomplete_buffer_for_content: got error:{}\n", e.errCode).c_str());
					if (e.errCode == sip2jsonErrors::incomplete_buffer_for_content) passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_header)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile(__func__); // NOLINT
	auto bs		  = buffer.begin();
	sip2json::parse(bs, buffer.end(), {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
		EXPECT_TRUE(e.errCode == sip2jsonErrors::incomplete_buffer_for_header);
		passTest = true;
	});
	EXPECT_TRUE(passTest);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_unsupported_contenttype)
{
	bool passTest = false;
	auto buffer	  = loadSampleFile(__func__); // NOLINT
	auto bs		  = buffer.begin();
	sip2json::parse(bs, buffer.end(), {}, [&](const sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
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
	sip2json::parse(
			bs,
			buffer.end(),
			[&](sipmessage& sipm) {
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