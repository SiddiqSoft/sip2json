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
#include "../../src/sip2json_exception.hpp"

#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace siddiqsoftware;

namespace test_suite
{
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
			auto registerMessage = sipmessage::create("REGISTER", "sip:hello@world.com", createCallId(), 1);
			auto diagContents	 = registerMessage.flatten().dump(2);
			std::cerr << diagContents << std::endl;
			Assert::IsTrue(registerMessage.size() != 0);
			Assert::IsTrue(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());
			Assert::IsTrue(!registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).empty());
			Assert::IsTrue(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::IsTrue(registerMessage.value("/s/type"_json_pointer, std::string {}).find("request") != std::string::npos);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_createResponse)
		{
			auto dummyMessage = sipmessage::create(500);
			auto diagContents = dummyMessage.flatten().dump(2);
			std::cerr << diagContents << std::endl;
			Assert::IsTrue(dummyMessage.size() != 0);
			Assert::IsTrue(!dummyMessage.value("/s/reason"_json_pointer, std::string {}).empty());
			Assert::IsTrue(dummyMessage.value("/s/type"_json_pointer, std::string {}).compare(sipmessage::MessageTypeResponse) ==
						   0);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(createRequest_toCloudEvent)
		{
			auto registerMessage = sipmessage::create("REGISTER", "sip:hello@world.com", createCallId(), 1);
			auto diagContents	 = registerMessage.flatten().dump(2);
			std::cerr << diagContents << std::endl;
			Assert::IsTrue(registerMessage.size() != 0);
			Assert::IsTrue(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());
			Assert::IsTrue(!registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).empty());
			Assert::IsTrue(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::IsTrue(registerMessage.value("/s/type"_json_pointer, std::string {}).find("request") != std::string::npos);

			try
			{
				auto ce = registerMessage.to_cloudEvent();
				Logger::WriteMessage(ce.flatten().dump(2).c_str());
				Assert::IsTrue(ce.contains("id"));
				Assert::IsTrue(ce.contains("type"));
				Assert::IsTrue(ce.contains("specversion"));
				Assert::IsTrue(ce.contains("datacontenttype"));
				Assert::IsTrue(ce.contains("source"));
				Assert::IsTrue(ce.contains("time"));
				Assert::IsTrue(ce.contains("subject"));
				Assert::IsTrue(ce.contains("data"));
			}
			catch (std::runtime_error& e)
			{
				Logger::WriteMessage(e.what());
				Assert::Fail(L"Failed");
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(createResponse_toCloudEvent)
		{
			auto dummyMessage = sipmessage::create(500);
			auto diagContents = dummyMessage.flatten().dump(2);
			std::cerr << diagContents << std::endl;
			Assert::IsTrue(dummyMessage.size() != 0);
			Assert::IsTrue(!dummyMessage.value("/s/reason"_json_pointer, std::string {}).empty());
			Assert::IsTrue(dummyMessage.value("/s/type"_json_pointer, std::string {}).compare(sipmessage::MessageTypeResponse) ==
						   0);

			try
			{
				auto ce = dummyMessage.to_cloudEvent();
				Logger::WriteMessage(ce.flatten().dump(2).c_str());
				Assert::IsTrue(ce.contains("id"));
				Assert::IsTrue(ce.contains("type"));
				Assert::IsTrue(ce.contains("specversion"));
				Assert::IsTrue(ce.contains("datacontenttype"));
				Assert::IsTrue(ce.contains("source"));
				Assert::IsTrue(ce.contains("time"));
				Assert::IsTrue(ce.contains("subject"));
				Assert::IsTrue(ce.contains("data"));
			}
			catch (std::runtime_error& e)
			{
				Logger::WriteMessage(e.what());
				Assert::Fail(L"Failed");
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_createRequest_then_response)
		{
			auto registerMessage = sipmessage::create("REGISTER", "sip:hello@world.com", createCallId(), 1);
			std::cerr << "POST create(): registerMessage:" << registerMessage.flatten().dump(2) << std::endl;
			Assert::IsTrue(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());

			registerMessage["/h/To"_json_pointer]	   = "sip:hello@world.com";
			registerMessage["/h/Contact"_json_pointer] = "sip:hello@world.com";

			Assert::IsTrue(registerMessage.size() != 0);
			Assert::IsTrue(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::AreEqual<std::string>(registerMessage.value("/s/type"_json_pointer, std::string {}),
										  sipmessage::MessageTypeRequest);

			// WARNING
			// As we're passing the registerMessage as parameter to create an inplace response message
			// the original registerMessage object will be clobbered with the items from the
			// response message create function.
			auto responseMessage = sipmessage::create(200, registerMessage);

			Assert::IsTrue(responseMessage.size() != 0);
			Assert::IsTrue(responseMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::AreEqual<std::string>(responseMessage.value("/s/type"_json_pointer, std::string {}),
										  sipmessage::MessageTypeResponse);
			Assert::IsTrue(!responseMessage.value("/h/Date"_json_pointer, std::string {}).empty());

			std::cerr << "After response; registerMessage:" << registerMessage.flatten().dump(2) << std::endl;
			std::cerr << "After response; registerMessage serialized:" << sip2json::serialize(registerMessage) << std::endl;

			std::cerr << "After response; responseMessage:" << responseMessage.flatten().dump(2) << std::endl;
			std::cerr << "After response; responseMessage serialized:" << sip2json::serialize(responseMessage) << std::endl;


			Assert::AreEqual<std::string>(registerMessage.value("/h/Call-ID"_json_pointer, "req"),
										  responseMessage.value("/h/Call-ID"_json_pointer, "resp"));
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_serialize)
		{
			auto myCallId		 = createCallId();
			auto registerMessage = sipmessage::create("REGISTER", "sip:hello@world.com", myCallId, 1);

			registerMessage["/h/To"_json_pointer]	   = "sip:hello@world.com";
			registerMessage["/h/Contact"_json_pointer] = "sip:hello@world.com";

			auto strsipm = sip2json::serialize(registerMessage);
			std::cerr << strsipm << std::endl;
			Assert::IsTrue(strsipm.length() != 0);

			auto bufferStart = strsipm.begin();
			auto sipm2		 = sip2json::parseFromBuffer(bufferStart, strsipm.end());
			Assert::IsTrue(!sipm2.empty());
			Assert::AreEqual(registerMessage.getContentLength(), sipm2.getContentLength());
			Assert::AreEqual(registerMessage.getCallID(), sipm2.getCallID());

			Logger::WriteMessage("\n============vvv=\n");
			Logger::WriteMessage(strsipm.c_str());
			Logger::WriteMessage("\n============   =\n");
			Logger::WriteMessage(sip2json::serialize(sipm2).c_str());
			Logger::WriteMessage("\n============^^^=\n");

			Assert::AreEqual(strsipm.length(), sip2json::serialize(sipm2).length());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_serialize_empty_mb)
		{
			auto registerMessage = sipmessage::create("REGISTER", "sip:hello@world.com", createCallId(), 1);

			registerMessage["/h/To"_json_pointer]	   = "sip:hello@world.com";
			registerMessage["/h/Contact"_json_pointer] = "sip:hello@world.com";
			// Set the content-type but fail to actually set the mb
			registerMessage["/h/Content-Type"_json_pointer] = "application/sdp";
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
		TEST_METHOD(Test_incomplete_buffer_for_parse)
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
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::incomplete_buffer_for_parse);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_incomplete_buffer_for_content)
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
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::incomplete_buffer_for_content);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_incomplete_buffer_for_header)
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
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::incomplete_buffer_for_header);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_unsupported_contenttype)
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
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::unsupported_contenttype);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_invalid_document)
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
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::invalid_document);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_invalid_document_startline)
		{
			sipmessage sipm = sipmessage::create("ROR", "sip:dummy@world.com");

			try
			{
				sip2json::serialize(sipm);
				Assert::Fail(L"Expect exception: invalid_document\n");
			}
			catch (invalid_document_error& e)
			{
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::invalid_document);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_empty_mb)
		{
			auto registerMessage = sipmessage::create("REGISTER", "sip:hello@world.com", createCallId(), 1);

			registerMessage.header("To", "sip:hello@world.com");
			registerMessage.header("Contact", "sip:hello@world.com");
			// Set the content-type but fail to actually set the mb
			registerMessage.header("Content-Type", "application/sdp");
			Assert::IsTrue(registerMessage.body().empty());
			Assert::ExpectException<std::exception>([&]() { sip2json::serialize(registerMessage); });
			// Set some dummy value..
			registerMessage["b"]["sdp"][0]["v"] = 0;
			// Check again for the body. it should be non-null
			Assert::IsTrue(registerMessage.body().is_object());
			Assert::ExpectException<std::exception>([&]() { Logger::WriteMessage(sip2json::serialize(registerMessage).c_str()); });
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_empty_message)
		{
			sipmessage emptyMessage;
			try
			{
				sip2json::serialize(emptyMessage);
				Assert::Fail(L"Expect exception: empty_message\n");
			}
			catch (empty_message_error& e)
			{
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::empty_message);
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_invalid_startline)
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
				Logger::WriteMessage(e.what());
				Assert::IsTrue(e.errCode == sip2jsonErrors::invalid_startline);
			}
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_check_isMessageTypeRequest)
		{
			auto sipm = sipmessage::create("INVITE", "sip:hello@world.com", createCallId(), 1);

			Assert::IsTrue(sipm.size() != 0);
			Assert::IsTrue(!sipm.value("/h/Date"_json_pointer, std::string {}).empty());
			Assert::IsTrue(!sipm.value("/h/Call-ID"_json_pointer, std::string {}).empty());
			Assert::IsTrue(sipm.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::IsTrue(sipm.value("/s/type"_json_pointer, std::string {}).find("request") != std::string::npos);

			Assert::IsTrue(sipm.isMessageTypeRequest());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_check_isMessageTypeResponse)
		{
			auto sipm = sipmessage::create(608);

			Assert::IsTrue(sipm.size() != 0);
			Assert::IsTrue(!sipm.value("/s/reason"_json_pointer, std::string {}).empty());
			Assert::AreEqual(sipmessage::MessageTypeResponse, sipm.value("/s/type"_json_pointer, std::string {}));
			Assert::AreEqual<uint32_t>(608, sipm.getStatusCode());
			Assert::IsTrue(sipm.isMessageTypeResponse());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_check_getContentType)
		{
			auto sipm = sipmessage::create("INVITE", "sip:hello@world.com", createCallId(), 1);

			Assert::IsTrue(sipm.size() != 0);
			Assert::IsTrue(!sipm.value("/h/Date"_json_pointer, std::string {}).empty());
			Assert::IsTrue(!sipm.value("/h/Call-ID"_json_pointer, std::string {}).empty());
			Assert::IsTrue(sipm.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
			Assert::IsTrue(sipm.value("/s/type"_json_pointer, std::string {}).find("request") != std::string::npos);

			// Note the "Content-type" and "Content-Type"; either way the method should return the value.
			sipm["h"]["Content-type"] = "test/test";
			Assert::AreEqual<std::string>("test/test", sipm.getContentType());

			sipm["h"]["Content-Type"] = "test/test2";
			Assert::AreEqual<std::string>("test/test2", sipm.getContentType());

			sipm["h"].erase("Content-Type");
			sipm["h"].erase("Content-type");
			Assert::IsTrue(sipm.getContentType().empty());
		}

	}; // SIPHelpers

	// NOLINTNEXTLINE
	TEST_CLASS(parseAllFromBuffer)
	{
	public:
		bool dummy;

		// NOLINTNEXTLINE
		TEST_METHOD(Test_invalid_startline)
		{
			bool passTest = false;
			auto buffer	  = loadSampleFile(__func__); // NOLINT
			auto bs		  = buffer.begin();
			sip2json::parseAllFromBuffer(bs, buffer.end(), {}, [&](const sip2jsonErrors& errCode) {
				Assert::IsTrue(errCode == sip2jsonErrors::invalid_startline);
				passTest = true;
			});
			Assert::IsTrue(passTest);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_incomplete_buffer_for_parse)
		{
			auto buffer	  = siddiqsoftware::SIP_SAMPLE_MINIMAL_MESSAGE;
			bool passTest = false;
			auto bs		  = buffer.begin();
			sip2json::parseAllFromBuffer(bs, buffer.end(), {}, [&](const sip2jsonErrors& errCode) {
				Assert::IsTrue(errCode == sip2jsonErrors::incomplete_buffer_for_parse);
				passTest = true;
			});
			Assert::IsTrue(passTest);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_incomplete_buffer_for_content)
		{
			bool passTest = false;
			auto buffer	  = loadSampleFile(__func__); // NOLINT
			auto bs		  = buffer.begin();
			sip2json::parseAllFromBuffer(bs, buffer.end(), {}, [&](const sip2jsonErrors& errCode) {
				// We would get multiple exceptions/callbacks so we should watch out for our specific code.
				Logger::WriteMessage(fmt::format("Test_incomplete_buffer_for_content: got error:{}\n", errCode).c_str());
				if (errCode == sip2jsonErrors::incomplete_buffer_for_content) passTest = true;
			});
			Assert::IsTrue(passTest);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_incomplete_buffer_for_header)
		{
			bool passTest = false;
			auto buffer	  = loadSampleFile(__func__); // NOLINT
			auto bs		  = buffer.begin();
			sip2json::parseAllFromBuffer(bs, buffer.end(), {}, [&](const sip2jsonErrors& errCode) {
				Assert::IsTrue(errCode == sip2jsonErrors::incomplete_buffer_for_header);
				passTest = true;
			});
			Assert::IsTrue(passTest);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_unsupported_contenttype)
		{
			bool passTest = false;
			auto buffer	  = loadSampleFile(__func__); // NOLINT
			auto bs		  = buffer.begin();
			sip2json::parseAllFromBuffer(bs, buffer.end(), {}, [&](const sip2jsonErrors& errCode) {
				Assert::IsTrue(errCode == sip2jsonErrors::unsupported_contenttype);
				passTest = true;
			});
			Assert::IsTrue(passTest);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Test_unknown_exception)
		{
			bool pass1Test = false;
			bool pass2Test = false;
			auto buffer	   = loadSampleFile("REGISTER_1"); // NOLINT
			auto bs		   = buffer.begin();

			// Deliberately throw an exception in the parse-callback so we can ensure that the error-callback is invoked.
			sip2json::parseAllFromBuffer(
					bs,
					buffer.end(),
					[&](sipmessage& sipm) {
						// We should parse valid message and get our callback.
						pass1Test = true;
						// Throw so we can get the error-callback triggered.
						throw 666;
					},
					[&](const sip2jsonErrors& errCode) {
						if (pass1Test) pass2Test = (errCode == sip2jsonErrors::unknown);
					});

			Assert::IsTrue(pass1Test, L"First stage callback not invoked.");
			Assert::IsTrue(pass2Test, L"Error callback not invoked.");
		}
	};
} // namespace test_suite
