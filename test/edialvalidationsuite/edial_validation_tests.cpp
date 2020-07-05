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

namespace siddiqsoftware
{
	// NOLINTNEXTLINE
	TEST_CLASS(edial_validation_tests)
	{
	public:
		std::string loadSampleFile(const std::string& fileName)
		{
			std::stringstream testFile;
			std::ifstream	  sampleInputFile(fmt::format("../test/samples/{}.sip", fileName));

			if (sampleInputFile.is_open())
			{
				while (sampleInputFile.peek() != EOF)
				{
					testFile << (char)sampleInputFile.get();
				}
				sampleInputFile.close();
			}


			return testFile.str();
		}

	public:
		bool dummy;

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_startline)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm.value("/rl/uri"_json_pointer, std::string {}));
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_headers)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm.getContentLength());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_headers_serialize)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<size_t>(848, sipm.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm.value("/mh/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

			// Now, we will serialize the decoded sipm..
			auto serializedFromDecoded = sip2json::serialize(sipm);

			std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

			// So we can decode it again and ensure that we can round-trip!
			auto serializedFromDecodedStart = serializedFromDecoded.begin();
			auto sipm2						= sip2json::parseFromBuffer(serializedFromDecodedStart, serializedFromDecoded.end());
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm2.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm2.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm2.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm2.getContentType());
			// Content-Length
			Assert::AreEqual<size_t>(848, sipm2.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm2.value("/mh/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm2.value("/mh/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm2.value("/mh/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm2.value("/mh/X-Call-Instance-ID"_json_pointer, ""));
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegAdd)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegAdd");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.flatten().dump(2);

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "15105076141563il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(783, sipm.getContentLength());

			Assert::AreEqual<std::string>("akirmayer@sidley.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
			Assert::IsTrue(sipm.contains("/mh/X-rss-id"_json_pointer));
			Assert::AreEqual<std::string>("2 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm.value("/mh/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("MTUxMDUwNzYxNDE1NjNpbC1lZC1tYXJhLTAxOjE1OTM1NjQxNjc6Mjg0NDcw",
										  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			Assert::IsTrue(!sipm.value("/mb"_json_pointer, nlohmann::json {}).empty());
			Assert::IsTrue(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/mb/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/access_code"_json_pointer, ""), "2742801");
			// Check leg_no is parsed
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/leg_no"_json_pointer, ""), "12");
			// Check status is parsed
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/status"_json_pointer, ""), "(203) answered  ");
			// Check timing is parsed into array
			Assert::AreEqual<unsigned long>(sipm.value("/mb/sdp/0/t/0"_json_pointer, 1L), 3802556545L);
			Assert::AreEqual<unsigned long>(sipm.value("/mb/sdp/0/t/1"_json_pointer, 1L), 0L);

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/dn"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/type"_json_pointer, ""), "TN");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/dn"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/name"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/t1"_json_pointer, ""), "55706299459030");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/t2"_json_pointer, ""), "847687142");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/type"_json_pointer, ""), "IN");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/user"_json_pointer, ""), "akirmayer@sidley.com");

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-15.5.2.S4");
			Assert::AreEqual<bool>(sipm.value("/mb/sdp/0/a/new_change"_json_pointer, false), true);
			Assert::IsTrue(sipm.contains("/mb/sdp/0/a/far_end"_json_pointer));
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/clir"_json_pointer, ""), "false");
			//a=dialin:2742801:6660014385@205.252.237.66-$$-1
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/dialin"_json_pointer, ""),
										  "2742801:6660014385@205.252.237.66-$$-1");

			// Now, we will serialize the decoded sipm..
			auto serializedFromDecoded = sip2json::serialize(sipm);

			std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

			// So we can decode it again and ensure that we can round-trip!
			auto serializedFromDecodedStart = serializedFromDecoded.begin();
			auto sipm2						= sip2json::parseFromBuffer(serializedFromDecodedStart, serializedFromDecoded.end());
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm2.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm2.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegDrop)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.flatten().dump(2);

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
			Assert::IsTrue(sipm.contains("/mh/X-rss-id"_json_pointer));
			Assert::AreEqual<bool>(false, sipm.value("/mh/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			Assert::IsTrue(!sipm.value("/mb"_json_pointer, nlohmann::json {}).empty());
			Assert::IsTrue(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/mb/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/access_code"_json_pointer, ""), "2873116");
			// Check leg_no is parsed
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/leg_no"_json_pointer, ""), "24");
			// Check status is parsed
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
			// Check timing is parsed into array
			Assert::AreEqual<unsigned long>(sipm.value("/mb/sdp/0/t/0"_json_pointer, 0L), 3802534341L);
			Assert::AreEqual<unsigned long>(sipm.value("/mb/sdp/0/t/1"_json_pointer, 0L), 3802534887L);

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/dn"_json_pointer, ""), "+4044166441");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/type"_json_pointer, ""), "TN");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/dn"_json_pointer, ""), "+4044166441");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/name"_json_pointer, ""), "\"Cell Phone   GA\"");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/t1"_json_pointer, ""), "148492049389635");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/t2"_json_pointer, ""), "847595153");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/type"_json_pointer, ""), "IN");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/user"_json_pointer, ""), "jrbirge@nscorp.com");

			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
			Assert::AreEqual<bool>(sipm.value("/mb/sdp/0/a/new_change"_json_pointer, false), true);
			Assert::IsTrue(sipm.contains("/mb/sdp/0/a/far_end"_json_pointer));
			Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/clir"_json_pointer, ""), "false");

			// Now, we will serialize the decoded sipm..
			auto serializedFromDecoded = sip2json::serialize(sipm);

			std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

			// So we can decode it again and ensure that we can round-trip!
			auto serializedFromDecodedStart = serializedFromDecoded.begin();
			auto sipm2						= sip2json::parseFromBuffer(serializedFromDecodedStart, serializedFromDecoded.end());
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm2.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm2.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm2.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm2.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm2.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm2.value("/mh/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm2.value("/mh/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm2.value("/mh/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm2.value("/mh/X-Call-Instance-ID"_json_pointer, ""));
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_CallEnd)
		{
			auto buffer		 = loadSampleFile("NOTIFY_CallEnd");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.flatten().dump(2);

			auto verifyItems = [](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, ""));
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
											  sipm.value("/rl/uri"_json_pointer, ""));
				// Via is an array
				Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 3);
				// Call-ID
				Assert::AreEqual<std::string>(sipm.getCallID(), "119035121230567il-ed-mara-01");
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(1326, sipm.getContentLength());

				Assert::AreEqual<std::string>("matthew.gabbard@stblaw.com", sipm.value("/mh/X-control-master"_json_pointer, ""));
				Assert::IsTrue(sipm.contains("/mh/X-rss-id"_json_pointer));
				Assert::AreEqual<std::string>("49 NOTIFY", sipm.value("/mh/CSeq"_json_pointer, ""));
				Assert::AreEqual<bool>(true, sipm.value("/mh/X-Billing-code-required"_json_pointer, false));
				Assert::AreEqual<std::string>("MTE5MDM1MTIxMjMwNTY3aWwtZWQtbWFyYS0wMToxNTkzNjM3MTcwOjE1Njc0Ng==",
											  sipm.value("/mh/X-Call-Instance-ID"_json_pointer, ""));

				// Check the body
				Assert::IsTrue(!sipm.value("/mb"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/mb/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/access_code"_json_pointer, ""), "2997255");
				// Check leg_no is parsed
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/leg_no"_json_pointer, ""), "2");
				// Check status is parsed
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/mb/sdp/0/t/0"_json_pointer, 0L), 3802625984L);
				Assert::AreEqual<unsigned long>(sipm.value("/mb/sdp/0/t/1"_json_pointer, 0L), 3802626770L);

				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/name"_json_pointer, ""), "\"Matt%20Gabbard%20-%20\"");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/t1"_json_pointer, ""), "3598380125");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/t2"_json_pointer, ""), "241");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/type"_json_pointer, ""), "IN");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/o/user"_json_pointer, ""), "matthew.gabbard@stblaw.com");

				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/cli-screening"_json_pointer, ""), "00");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/x-ring2-smartproxy"_json_pointer, ""),
											  "usaze-asalt01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				Assert::AreEqual<bool>(sipm.value("/mb/sdp/0/a/new_change"_json_pointer, false), true);
				Assert::IsTrue(sipm.contains("/mb/sdp/0/a/far_end"_json_pointer));
				Assert::AreEqual<std::string>(sipm.value("/mb/sdp/0/a/clir"_json_pointer, ""), "false:18777464263");
			};

			verifyItems(sipm);

			// Now, we will serialize the decoded sipm..
			auto serializedFromDecoded = sip2json::serialize(sipm);

			std::cerr << "Serialized from decoded SIPMessage\n" << serializedFromDecoded;

			// So we can decode it again and ensure that we can round-trip!
			auto serializedFromDecodedStart = serializedFromDecoded.begin();
			auto sipm2						= sip2json::parseFromBuffer(serializedFromDecodedStart, serializedFromDecoded.end());

			//verifyItems(sipm2);

			//Forces output; disable when implementation is completed.
			//Assert::AreEqual<std::string>(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).size(), 0)
			//		<< "Debugging only; disable line when completed.";
		}


		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_200_OK)
		{
			auto buffer		 = loadSampleFile("REGISTER_200_OK");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<uint32_t>(200, sipm.value("/sl/statusCode"_json_pointer, 0));
			// Via is an array
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 1);
			Assert::AreEqual<std::string>(sipm.value("/mh/Via/0"_json_pointer, ""), "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
			// Content-Type
			//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
			Assert::AreEqual<uint32_t>(300, sipm.getExpires());
			Assert::AreEqual<bool>(true, sipm.value("/mh/X-subscribe-to-leg-events"_json_pointer, false));
		}


		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_1)
		{
			auto buffer		 = loadSampleFile("REGISTER_1");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<std::string>(METHOD_REGISTER, sipm.value("/rl/method"_json_pointer, ""));
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/mh/Via"_json_pointer, nlohmann::json {}).size() == 1);
			Assert::AreEqual<std::string>(sipm.value("/mh/Via/0"_json_pointer, nlohmann::json {}),
										  "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
			// Content-Type
			//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
			Assert::AreEqual<uint32_t>(300, sipm.getExpires());
			Assert::AreEqual<bool>(true, sipm.value("/mh/X-subscribe-to-leg-events"_json_pointer, false));
		}


		// NOLINTNEXTLINE
		TEST_METHOD(OK_REGISTER_Multiline_ContactHeader_1)
		{
			auto buffer		 = loadSampleFile(__func__);
			auto item		 = 0;
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			// Source file has three frames
			Assert::AreEqual<size_t>(3, msgs.size());
			// Affirm that the first frame has a Contact that has been unfolded properly!
			Assert::AreEqual<std::string>(
					msgs[0].value("/mh/Contact"_json_pointer, ""),
					"sip:jcollier@federationbankia.com;expires=1593725109;tag=sp2(263988)_IL-PS-CONGO-02.ring2.com, "
					"sip:jcollier@federationbankia.com;expires=1593725109;tag=sp2(26392)_IL-PS-CONGO-01.ring2.com, "
					"sip:jcollier@federationbankia.com;expires=1593725269;tag=65750151432167il-ed-mara-01__sp3[USCHEQ-ASRTA01."
					"ring2.com]");
			// Affirm that the second item's contact is a single line
			Assert::AreEqual<std::string>(msgs[1].value("/mh/Contact"_json_pointer, ""), "<sip:216.111.92.37:8443;transport=ssl>");

			// Affirm that the third element's contact ends with a space.
			Assert::AreEqual<std::string>(msgs[2].value("/mh/Contact"_json_pointer, ""),
										  "<sip:216.111.92.37:8443;transport=ssl>;expires=3600;tag=65750151432167il-ed-mara-01__"
										  "sp3[USCHEQ-ASRTA01.ring2.com], ");

			for (auto& i : msgs)
			{
				auto str = fmt::format("{} - document {} -> {}\n", __func__, ++item, i.flatten().dump(2));
				Logger::WriteMessage(str.c_str());
			}
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_SDP_multi_1)
		{
			auto buffer		 = loadSampleFile(__func__);
			auto item		 = 0;
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			// We're going to have a single frame
			Assert::AreEqual<size_t>(1, msgs.size());

			for (auto& i : msgs)
			{
				auto str = fmt::format("{} - document {} -> {}\n", __func__, ++item, i.flatten().dump(2));
				Logger::WriteMessage(str.c_str());
			}

			// Affirm that the first frame has a Contact that has been unfolded properly!
			Assert::AreEqual<std::string>(msgs[0].value("/mh/Contact"_json_pointer, ""), "<sip:localhost:8443;transport=ssl>");
			// We should have 4 SDP elements
			Assert::AreEqual<size_t>(4, msgs[0].value("/mb/sdp"_json_pointer, nlohmann::json {}).size());
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Trying_INVITE_1)
		{
			auto buffer		 = loadSampleFile(__func__);
			auto item		 = 0;
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());

			Logger::WriteMessage(fmt::format("{} - Found: {} messages\n", __func__, msgs.size()).c_str());

			// We're going to have a single frame
			Assert::AreEqual<size_t>(1, msgs.size(), L"Expect only one message parsed.");

			for (auto& i : msgs)
			{
				auto str = fmt::format("{} - document {} -> {}\n", __func__, ++item, i.flatten().dump(2));
				Logger::WriteMessage(str.c_str());
			}

			Assert::AreEqual<std::string>("1593721670540996", msgs[0].value("/mh/X-Message-Time"_json_pointer, ""));
			Assert::AreEqual<std::string>("3 INVITE", msgs[0].value("/mh/CSeq"_json_pointer, ""));

			Assert::AreEqual<uint32_t>(100, msgs[0].value("/sl/statusCode"_json_pointer, 0));
			Assert::AreEqual<std::string>("Trying", msgs[0].value("/sl/reason"_json_pointer, ""));
			Assert::AreEqual<std::string>(
					"X-Signed start=\"1593721669\",expire=\"1593725269\",user=\"jcollier@federationbankia.com\",confwiz=\"my "
					"string\",nsadrs=\"il-ed-mara-01.ring2.com\",signed=\"a73789748d9c7dd2d1092794597d2a57\"",
					msgs[0].value("/mh/Authorization"_json_pointer, ""));
		}
	};
} // namespace siddiqsoftware
