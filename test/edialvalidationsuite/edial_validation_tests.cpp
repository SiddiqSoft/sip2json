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
	TEST_CLASS(edial_validation_tests)
	{
	public:
		std::string loadSampleFile(const std::string& fileName)
		{
			std::stringstream testFile;
			std::ifstream	  sampleInputFile(fmt::format("../test/samples/{}", fileName));

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


		TEST_METHOD(Test_parse_NOTIFY_1_startline)
		{
			auto buffer = loadSampleFile("NOTIFY_LegDrop.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm.value("/rl/uri"_json_pointer, std::string {}));
		}


		TEST_METHOD(Test_parse_NOTIFY_1_headers)
		{
			auto buffer = loadSampleFile("NOTIFY_LegDrop.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

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


		TEST_METHOD(Test_parse_NOTIFY_1_headers_serialize)
		{
			auto buffer = loadSampleFile("NOTIFY_LegDrop.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

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
			auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());
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


		TEST_METHOD(Test_parse_NOTIFY_LegAdd_body)
		{
			auto buffer = loadSampleFile("NOTIFY_LegAdd.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

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
			auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm2.value("/rl/method"_json_pointer, std::string {}));
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
										  sipm2.value("/rl/uri"_json_pointer, std::string {}));
			// Via is an array
			Assert::IsTrue(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm2.value("/mh/Via"_json_pointer, nlohmann::json {}).size(), 4);
		}

		TEST_METHOD(Test_parse_NOTIFY_LegDrop_body)
		{
			auto buffer = loadSampleFile("NOTIFY_LegDrop.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

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
			auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());
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


		TEST_METHOD(Test_parse_NOTIFY_CallEnd_body)
		{
			auto buffer = loadSampleFile("NOTIFY_CallEnd.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

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
			auto sipm2 = sip2json::parseFromBuffer(serializedFromDecoded.begin(), serializedFromDecoded.end());

			//verifyItems(sipm2);

			//Forces output; disable when implementation is completed.
			//Assert::AreEqual<std::string>(sipm.value("/mb/sdp"_json_pointer, nlohmann::json {}).size(), 0)
			//		<< "Debugging only; disable line when completed.";
		}


		TEST_METHOD(Test_parse_REGISTER_200_OK)
		{
			auto buffer = loadSampleFile("REGISTER_200_OK.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

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

		TEST_METHOD(Test_parse_REGISTER_1)
		{
			auto buffer = loadSampleFile("REGISTER_1.sip");
			auto sipm	= sip2json::parseFromBuffer(buffer.begin(), buffer.end());

			std::cerr << "Decoded SIPMessage document" << sipm.dump(2);

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<>(METHOD_REGISTER, sipm.value("/rl/method"_json_pointer, ""));
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
	};
} // namespace siddiqsoftware
