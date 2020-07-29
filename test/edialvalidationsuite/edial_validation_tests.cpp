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

#include "../../src/sip2json_utils.hpp"
#include "../../src/sip2json.hpp"

#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace siddiqsoftware;

namespace test_suite
{
	static std::string loadSampleFile(const std::string& fileName)
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


	static void writeSampleFile(const std::string& fileName, const std::string& buffer)
	{
		std::ofstream outputFile(fmt::format("../test/samples/{}.sip.out", fileName), std::ios::binary);

		if (outputFile.is_open())
		{
			outputFile << buffer;
			outputFile.close();
		}
	}


	// NOLINTNEXTLINE
	TEST_CLASS(serialize)
	{
		void roundTripVerify(
				const std::string& funcName, const std::string& buffer, sipmessage& sipm, std::function<void(sipmessage&)> verify)
		{
			try
			{
				// Verify the parse
				verify(sipm);
				//Logger::WriteMessage(fmt::format("\n{}\n====================\n", sipm.flatten().dump(2)).c_str());
				// Serialize the parsed item
				auto sipmSerialized = sip2json::serialize(sipm);
				//Logger::WriteMessage(fmt::format("\n{}\n====================\n", sipmSerialized).c_str());
				// Parse the serialized result
				auto bufferStart = sipmSerialized.begin();
				auto sipm2		 = sip2json::parseFromBuffer(bufferStart, sipmSerialized.end());
				// Verify the round-trip
				verify(sipm2);
				// Ensure that the round-trip is equal; if buffer is empty, skip this check.
				if (!buffer.empty())
				{
					Logger::WriteMessage(
							fmt::format("{}: buffer:{}  serialized:{}\n", funcName, buffer.length(), sipmSerialized.length())
									.c_str());
					Assert::AreEqual(buffer.length(), sipmSerialized.length());
				}
			}
			catch (std::runtime_error& e)
			{
				Logger::WriteMessage(fmt::format("{}: Exception:{}\n", funcName, e.what()).c_str());
				Assert::Fail(L"Failed due to exception");
			}
		}

	public:
		bool dummy;

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_connectorleg_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto item		 = 0;
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			// We're going to have a single frame
			Assert::AreEqual<size_t>(1, msgs.size());

			for (auto& i : msgs)
			{
				auto str = fmt::format("{} - document {} -> {}\n", __func__, ++item, i.flatten().dump(2));
				//Logger::WriteMessage(str.c_str());
			}

			auto verify = [](sipmessage& sipm) {
				// Affirm that the first frame has a Contact that has been unfolded properly!
				Assert::AreEqual<std::string>(sipm.value("/h/Contact"_json_pointer, ""), "<sip:localhost:8443;transport=ssl>");
				Assert::AreEqual<size_t>(1, sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).size());
				Assert::AreEqual<size_t>(886, sipm.getContentLength());

				// c=IN IP4 10.254.254.33
				Assert::AreEqual<std::string>("IN", sipm.value("/b/sdp/0/c/type"_json_pointer, ""));
				Assert::AreEqual<std::string>("IP4", sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""));
				Assert::AreEqual<std::string>("10.254.254.33", sipm.value("/b/sdp/0/c/dn"_json_pointer, ""));

				// a=rtpmap should have 2 entries
				Assert::IsTrue(sipm.value("/b/sdp/0/a/rtpmap"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(2, sipm.value("/b/sdp/0/a/rtpmap"_json_pointer, nlohmann::json {}).size());
			};

			// Verify the decode
			verify(msgs[0]);

			// Let's serialize it
			auto serialized = sip2json::serialize(msgs[0]);
			writeSampleFile("NOTIFY_connectorleg_1", serialized);

			// Decode the serialized
			auto secondBufferStart = serialized.begin();
			auto sipm2			   = sip2json::parseFromBuffer(secondBufferStart, serialized.end());

			// Verify the decode of the serialized
			verify(sipm2);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_playbacklegs_1)
		{
			size_t countOfMessage = 0;
			auto   buffer		  = loadSampleFile(__func__); // NOLINT
			auto   bufferStart	  = buffer.begin();
			auto   msgs			  = sip2json::parseAllFromBuffer(bufferStart, buffer.end());

			auto verify = [&](sipmessage& sipm) {
				Logger::WriteMessage(sipm.flatten().dump(4).c_str());
				if (countOfMessage == 1)
				{
					Assert::AreEqual<std::string>("LegAdd", sipm.value("/h/X-CallEvent"_json_pointer, ""));
					Assert::AreEqual<uint32_t>(741, sipm.getContentLength());
					Assert::AreEqual<std::string>("1445714364", sipm.value("/b/sdp/0/o/t2"_json_pointer, ""));
				}
				if (countOfMessage == 2)
				{
					Assert::AreEqual<std::string>("LegDrop", sipm.value("/h/X-CallEvent"_json_pointer, ""));
					Assert::AreEqual<uint32_t>(784, sipm.getContentLength());
					//t = 3802711133 3802711134
					Assert::AreEqual<uint32_t>(3802711134, sipm.value("/b/sdp/0/t/1"_json_pointer, 0));
					Assert::AreEqual<std::string>("1445714375", sipm.value("/b/sdp/0/o/t2"_json_pointer, ""));
				}

				// Both cases should have same values..
				// Check o-line: o=sip:nm@ring2.com 1445714250 1445714364 IN IP4 il-ed-mara-01.ring2.com
				Assert::AreEqual<std::string>("sip:nm@ring2.com", sipm.value("/b/sdp/0/o/user"_json_pointer, ""));
				Assert::AreEqual<std::string>("1445714250", sipm.value("/b/sdp/0/o/t1"_json_pointer, ""));
				Assert::AreEqual<std::string>("IN", sipm.value("/b/sdp/0/o/type"_json_pointer, ""));
				Assert::AreEqual<std::string>("IP4", sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""));
				Assert::AreEqual<std::string>("il-ed-mara-01.ring2.com", sipm.value("/b/sdp/0/o/host"_json_pointer, ""));
				// Check s-line: s=Playback-46570689829320il-ed-mara-01
				Assert::AreEqual<std::string>("Playback-46570689829320il-ed-mara-01", sipm.value("/b/sdp/0/s"_json_pointer, ""));
				// Check i-line: i=PlaybackLeg (target-legid 1) CallByPhone
				Assert::AreEqual<std::string>("PlaybackLeg", sipm.value("/b/sdp/0/i/name"_json_pointer, ""));
				Assert::AreEqual<std::string>("target-legid 1", sipm.value("/b/sdp/0/i/dn"_json_pointer, ""));
				Assert::AreEqual<std::string>("CallByPhone", sipm.value("/b/sdp/0/i/type"_json_pointer, ""));
				// Check c-line: c=IN IP4 127.0.0.1
				Assert::AreEqual<std::string>("IN", sipm.value("/b/sdp/0/c/type"_json_pointer, ""));
				Assert::AreEqual<std::string>("IP4", sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""));
				Assert::AreEqual<std::string>("127.0.0.1", sipm.value("/b/sdp/0/c/dn"_json_pointer, ""));
				// a=fmtp:x-play uri:http://10.254.254.2/slides/ring2sys_USA_msg_DialinDropParticipant_403/recording.wav
				Assert::AreEqual<std::string>(
						"x-play uri:http://10.254.254.2/slides/ring2sys_USA_msg_DialinDropParticipant_403/recording.wav",
						sipm.value("/b/sdp/0/a/fmtp"_json_pointer, ""));
				Assert::AreEqual<uint32_t>(3802711133, sipm.value("/b/sdp/0/t/0"_json_pointer, 0));
				Assert::AreEqual<std::string>("2", sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""));
			};

			countOfMessage = 1;
			roundTripVerify(__func__, EMPTY_STD_STRING_VALUE, msgs[0], verify);
			countOfMessage = 2;
			roundTripVerify(__func__, EMPTY_STD_STRING_VALUE, msgs[1], verify);

			Assert::AreEqual<size_t>(2, countOfMessage);
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_headers)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			//Logger::WriteMessage(sipm.dump(2).c_str());

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<size_t>(848, sipm.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm.value("/h/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

			try
			{
				// Now, we will serialize the decoded sipm..
				auto serializedFromDecoded = sip2json::serialize(sipm);

				writeSampleFile("NOTIFY_LegDrop", serializedFromDecoded);

				Logger::WriteMessage(serializedFromDecoded.c_str());

				// So we can decode it again and ensure that we can round-trip!
				auto serializedFromDecodedStart = serializedFromDecoded.begin();
				auto sipm2 = sip2json::parseFromBuffer(serializedFromDecodedStart, serializedFromDecoded.end());
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm2.getMethod());
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm2.getUri());
				// Via is an array
				Assert::IsTrue(sipm2.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm2.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
				// Call-ID
				Assert::AreEqual<std::string>(sipm2.getCallID(), "6732196043737il-ed-mara-01");
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm2.getContentType());
				// Content-Length
				Assert::AreEqual<size_t>(848, sipm2.getContentLength());

				Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm2.value("/h/X-control-master"_json_pointer, ""));
				Assert::AreEqual<std::string>("267 NOTIFY", sipm2.value("/h/CSeq"_json_pointer, ""));
				Assert::AreEqual<bool>(false, sipm2.value("/h/X-Billing-code-required"_json_pointer, true));
				Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
											  sipm2.value("/h/X-Call-Instance-ID"_json_pointer, ""));
			}
			catch (std::runtime_error& e)
			{
				Logger::WriteMessage(e.what());
				Assert::Fail(L"Unexpected error");
			}
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegAdd)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			roundTripVerify(__func__, buffer, sipm, [&](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
				// Call-ID
				Assert::AreEqual<std::string>(sipm.getCallID(), "15105076141563il-ed-mara-01");
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(783, sipm.getContentLength());

				Assert::AreEqual<std::string>("akirmayer@sidley.com", sipm.value("/h/X-control-master"_json_pointer, ""));
				Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
				Assert::AreEqual<std::string>("2 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
				Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
				Assert::AreEqual<std::string>("MTUxMDUwNzYxNDE1NjNpbC1lZC1tYXJhLTAxOjE1OTM1NjQxNjc6Mjg0NDcw",
											  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

				// Check the body
				Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2742801");
				// Check leg_no is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "12");
				// Check status is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(203) answered  ");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 1L), 3802556545L);
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 1L), 0L);

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+6568898813");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+6568898813");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "+6568898813");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "55706299459030");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "847687142");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "akirmayer@sidley.com");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""),
											  "Cisco-SIPGateway/IOS-15.5.2.S4");
				Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
				Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false");
				//a=dialin:2742801:6660014385@205.252.237.66-$$-1
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/dialin"_json_pointer, ""),
											  "2742801:6660014385@205.252.237.66-$$-1");
			});
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegDrop)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			roundTripVerify(__func__, buffer, sipm, [&](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
				// Call-ID
				Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(848, sipm.getContentLength());

				Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm.value("/h/X-control-master"_json_pointer, ""));
				Assert::AreEqual<std::string>("267 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
				Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
				Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
				Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
											  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

				// Check the body
				Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2873116");
				// Check leg_no is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "24");
				// Check status is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3802534341L);
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 3802534887L);

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+4044166441");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+4044166441");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Cell Phone   GA\"");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "148492049389635");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "847595153");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "jrbirge@nscorp.com");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
				Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
				Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false");
			});
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_CallStart_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			auto verifyItems = [](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/s/method"_json_pointer, ""));
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
											  sipm.value("/s/uri"_json_pointer, ""));
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(4, sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size());
				// Call-ID
				Assert::AreEqual<std::string>("777447022768721366.il-ed-mara-01.ring2.com", sipm.getCallID());
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(1081, sipm.getContentLength());

				Assert::AreEqual<std::string>("denver.peterson@loopup.com", sipm.value("/h/X-control-master"_json_pointer, ""));
				Assert::AreEqual<std::string>("1 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));

				Assert::AreEqual<std::string>("Nzc3NDQ3MDIyNzY4NzIxMzY2LmlsLWVkLW1hcmEtMDEucmluZzIuY29tOjE1OTUwNTI4NTQ6Mjk4NDQy",
											  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

				// Check the body
				Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2719318");
				//// Check leg_no is parsed
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "1");
				//// Check status is parsed
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3804041654L);
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 0L);

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+19253230928");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+19253230928");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Ring2QA%20Cingular\"");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "3598380125");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "241");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "matthew.gabbard@stblaw.com");

				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/cli-screening"_json_pointer, ""), "00");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/x-ring2-smartproxy"_json_pointer, ""),
				//							  "usaze-asalt01.ring2.com");
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				//Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
				//Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
				//Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false:18777464263");
			};

			roundTripVerify(__func__, buffer, sipm, verifyItems); // NOLINT
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_CallEnd)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			auto verifyItems = [](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/s/method"_json_pointer, ""));
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
											  sipm.value("/s/uri"_json_pointer, ""));
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 3);
				// Call-ID
				Assert::AreEqual<std::string>(sipm.getCallID(), "119035121230567il-ed-mara-01");
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(1326, sipm.getContentLength());

				Assert::AreEqual<std::string>("matthew.gabbard@stblaw.com", sipm.value("/h/X-control-master"_json_pointer, ""));
				Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
				Assert::AreEqual<std::string>("49 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
				Assert::AreEqual<bool>(true, sipm.value("/h/X-Billing-code-required"_json_pointer, false));
				Assert::AreEqual<std::string>("MTE5MDM1MTIxMjMwNTY3aWwtZWQtbWFyYS0wMToxNTkzNjM3MTcwOjE1Njc0Ng==",
											  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

				// Check the body
				Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2997255");
				// Check leg_no is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "2");
				// Check status is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3802625984L);
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 3802626770L);

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Matt%20Gabbard%20-%20\"");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "3598380125");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "241");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "matthew.gabbard@stblaw.com");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/cli-screening"_json_pointer, ""), "00");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/x-ring2-smartproxy"_json_pointer, ""),
											  "usaze-asalt01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
				Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false:18777464263");
			};

			roundTripVerify(__func__, buffer, sipm, verifyItems); // NOLINT
		}

		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_200_OK)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			Logger::WriteMessage(fmt::format("{} - Decoded SIPMessage document\n{}\n", __func__, sipm.dump(2)).c_str());

			roundTripVerify(__func__, buffer, sipm, [&](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// Start-Line (response): SIP/2.0 200 OK
				Assert::AreEqual<uint32_t>(200, sipm.getStatusCode());
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 1);
				Assert::AreEqual<std::string>(sipm.value("/h/Via/0"_json_pointer, ""), "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
				// Call-ID
				Assert::AreEqual<std::string>(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
				// Content-Type
				//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
				Assert::AreEqual<uint32_t>(300, sipm.getExpires());
				Assert::AreEqual<bool>(true, sipm.value("/h/X-subscribe-to-leg-events"_json_pointer, false));
			});
		}

		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_401_Unauthorized)
		{
			using namespace std;
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();

			try
			{
				auto sipm = sip2json::parseFromBuffer(bufferStart, buffer.end());
				roundTripVerify(__func__, buffer, sipm, [&](auto sipm) {
					Logger::WriteMessage(sipm.flatten().dump().c_str());
					// Start checking if we decoded properly..
					Assert::AreEqual<uint32_t>(401, sipm.getStatusCode());
					// Via is an array
					Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
					Assert::AreEqual<size_t>(1, sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size());
					Assert::AreEqual<std::string>("SIP/2.0/TCP il-ed-aras-01.ring2-corp.com:8443",
												  sipm.value("/h/Via/0"_json_pointer, ""));
					// Call-ID
					Assert::AreEqual<std::string>("755a8c07-ee3c-43fd-bfb-7f93ade4-89aaab98a8bb", sipm.getCallID());
					// Content-Type
					Assert::AreEqual<std::string>("<sip:no_such_user_exists@loopup.com>;tag=12345678",
												  sipm.header<std::string>("To"s));
					// Content-Length
					Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
					Assert::AreEqual<uint32_t>(360, sipm.getExpires());
					Assert::AreEqual<std::string>("Basic realm=eDial", sipm.header<std::string>("WWW-Authenticate"));
				});
			}
			catch (const std::exception& e)
			{
				Logger::WriteMessage(e.what());
			}
		}

		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];


			// NOLINTNEXTLINE
			roundTripVerify(__func__, buffer, sipm, [&](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// Start-Line (response): SIP/2.0 200 OK
				Assert::AreEqual<std::string>(METHOD_REGISTER, sipm.value("/s/method"_json_pointer, ""));
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size() == 1);
				Assert::AreEqual<std::string>(sipm.value("/h/Via/0"_json_pointer, nlohmann::json {}),
											  "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
				// Call-ID
				Assert::AreEqual<std::string>(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
				// Content-Type
				//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
				Assert::AreEqual<uint32_t>(300, sipm.getExpires());
				Assert::AreEqual<bool>(true, sipm.value("/h/X-subscribe-to-leg-events"_json_pointer, false));
			});
		}


		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_1_invalidMessageType)
		{
			auto buffer		 = loadSampleFile("REGISTER_1"); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Deliberately poison the message type
			sipm["/s/type"_json_pointer] = SIPMessageType::notspecified;
			// We should get the desired exception.
			Assert::ExpectException<invalid_document_error>([&]() { sip2json::serialize(sipm); });
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_invalid_cline_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();

			try
			{
				auto sipm = sip2json::parseFromBuffer(bufferStart, buffer.end());

				// c=T-N RFC/2543 +12124553521(hi)
				Assert::AreEqual<std::string>("T-NRFC/2543+12124553521(hi)", sipm.value("/b/sdp/0/c"_json_pointer, ""));
				Logger::WriteMessage(sipm.dump(4).c_str());
			}
			catch (std::runtime_error& e)
			{
				Logger::WriteMessage(e.what());
				Assert::Fail(L"Failed on account of exception.");
			}
		}
	};


	// NOLINTNEXTLINE
	TEST_CLASS(edial_validation_tests)
	{
	public:
		bool dummy;

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_startline)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_headers)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm.getContentLength());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegAdd)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "15105076141563il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(783, sipm.getContentLength());

			Assert::AreEqual<std::string>("akirmayer@sidley.com", sipm.value("/h/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
			Assert::AreEqual<std::string>("", sipm.value("/b/sdp/0/s"_json_pointer, "-"));
			Assert::AreEqual<std::string>("2 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("MTUxMDUwNzYxNDE1NjNpbC1lZC1tYXJhLTAxOjE1OTM1NjQxNjc6Mjg0NDcw",
										  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
			Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2742801");
			// Check leg_no is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "12");
			// Check status is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(203) answered  ");
			// Check timing is parsed into array
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 1L), 3802556545L);
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 1L), 0L);

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "55706299459030");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "847687142");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "akirmayer@sidley.com");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-15.5.2.S4");
			Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
			Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false");
			//a=dialin:2742801:6660014385@205.252.237.66-$$-1
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/dialin"_json_pointer, ""),
										  "2742801:6660014385@205.252.237.66-$$-1");
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_playbacklegs_1)
		{
			std::string debugBuffer	   = "[";
			size_t		countOfMessage = 0;
			auto		buffer		   = loadSampleFile(__func__); // NOLINT
			auto		bufferStart	   = buffer.begin();
			auto		msgs		   = sip2json::parseAllFromBuffer(bufferStart, buffer.end(), [&](sipmessage& sipm) {
				 ++countOfMessage;
				 debugBuffer += sipm.dump(4);
				 debugBuffer += ",";
				 Logger::WriteMessage(sipm.flatten().dump(4).c_str());
				 if (countOfMessage == 1)
				 {
					 Assert::AreEqual<std::string>("LegAdd", sipm.value("/h/X-CallEvent"_json_pointer, ""));
					 Assert::AreEqual<uint32_t>(741, sipm.getContentLength());
					 Assert::AreEqual<std::string>("1445714364", sipm.value("/b/sdp/0/o/t2"_json_pointer, ""));
				 }
				 if (countOfMessage == 2)
				 {
					 Assert::AreEqual<std::string>("LegDrop", sipm.value("/h/X-CallEvent"_json_pointer, ""));
					 Assert::AreEqual<uint32_t>(784, sipm.getContentLength());
					 //t = 3802711133 3802711134
					 Assert::AreEqual<uint32_t>(3802711134, sipm.value("/b/sdp/0/t/1"_json_pointer, 0));
					 Assert::AreEqual<std::string>("1445714375", sipm.value("/b/sdp/0/o/t2"_json_pointer, ""));
				 }

				 // Both cases should have same values..
				 // Check o-line: o=sip:nm@ring2.com 1445714250 1445714364 IN IP4 il-ed-mara-01.ring2.com
				 Assert::AreEqual<std::string>("sip:nm@ring2.com", sipm.value("/b/sdp/0/o/user"_json_pointer, ""));
				 Assert::AreEqual<std::string>("1445714250", sipm.value("/b/sdp/0/o/t1"_json_pointer, ""));
				 Assert::AreEqual<std::string>("IN", sipm.value("/b/sdp/0/o/type"_json_pointer, ""));
				 Assert::AreEqual<std::string>("IP4", sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""));
				 Assert::AreEqual<std::string>("il-ed-mara-01.ring2.com", sipm.value("/b/sdp/0/o/host"_json_pointer, ""));
				 // Check s-line: s=Playback-46570689829320il-ed-mara-01
				 Assert::AreEqual<std::string>("Playback-46570689829320il-ed-mara-01", sipm.value("/b/sdp/0/s"_json_pointer, ""));
				 // Check i-line: i=PlaybackLeg (target-legid 1) CallByPhone
				 Assert::AreEqual<std::string>("PlaybackLeg", sipm.value("/b/sdp/0/i/name"_json_pointer, ""));
				 Assert::AreEqual<std::string>("target-legid 1", sipm.value("/b/sdp/0/i/dn"_json_pointer, ""));
				 Assert::AreEqual<std::string>("CallByPhone", sipm.value("/b/sdp/0/i/type"_json_pointer, ""));
				 // Check c-line: c=IN IP4 127.0.0.1
				 Assert::AreEqual<std::string>("IN", sipm.value("/b/sdp/0/c/type"_json_pointer, ""));
				 Assert::AreEqual<std::string>("IP4", sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""));
				 Assert::AreEqual<std::string>("127.0.0.1", sipm.value("/b/sdp/0/c/dn"_json_pointer, ""));
				 // a=fmtp:x-play uri:http://10.254.254.2/slides/ring2sys_USA_msg_DialinDropParticipant_403/recording.wav
				 Assert::AreEqual<std::string>(
						 "x-play uri:http://10.254.254.2/slides/ring2sys_USA_msg_DialinDropParticipant_403/recording.wav",
						 sipm.value("/b/sdp/0/a/fmtp"_json_pointer, ""));
				 Assert::AreEqual<uint32_t>(3802711133, sipm.value("/b/sdp/0/t/0"_json_pointer, 0));
				 Assert::AreEqual<std::string>("2", sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""));
			 });

			Assert::AreEqual<size_t>(2, countOfMessage);
			debugBuffer += "]";
			writeSampleFile(__func__, debugBuffer); // NOLINT
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegDrop)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm.value("/h/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
			Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
			Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
			Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2873116");
			// Check leg_no is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "24");
			// Check status is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
			// Check timing is parsed into array
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3802534341L);
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 3802534887L);

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+4044166441");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+4044166441");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Cell Phone   GA\"");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "148492049389635");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "847595153");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "jrbirge@nscorp.com");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
			Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
			Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false");
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_CallEnd)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			auto verifyItems = [](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/s/method"_json_pointer, ""));
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
											  sipm.value("/s/uri"_json_pointer, ""));
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(3, sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size());
				// Call-ID
				Assert::AreEqual<std::string>("119035121230567il-ed-mara-01", sipm.getCallID());
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(1326, sipm.getContentLength());

				Assert::AreEqual<std::string>("matthew.gabbard@stblaw.com", sipm.header<std::string>("X-control-master"));
				Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
				Assert::AreEqual<std::string>("49 NOTIFY", sipm.header<std::string>("CSeq"));
				Assert::AreEqual<bool>(true, sipm.header<bool>("X-Billing-code-required"));
				Assert::AreEqual<std::string>("MTE5MDM1MTIxMjMwNTY3aWwtZWQtbWFyYS0wMToxNTkzNjM3MTcwOjE1Njc0Ng==",
											  sipm.header<std::string>("X-Call-Instance-ID"));

				// Check the body
				Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2997255");
				// Check leg_no is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "2");
				// Check status is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3802625984L);
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 3802626770L);

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Matt%20Gabbard%20-%20\"");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "3598380125");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "241");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "matthew.gabbard@stblaw.com");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/cli-screening"_json_pointer, ""), "00");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/x-ring2-smartproxy"_json_pointer, ""),
											  "usaze-asalt01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
				Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false:18777464263");
			};

			verifyItems(sipm);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_200_OK)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<uint32_t>(200, sipm.getStatusCode());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 1);
			Assert::AreEqual<std::string>(sipm.value("/h/Via/0"_json_pointer, ""), "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
			// Content-Type
			//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
			Assert::AreEqual<uint32_t>(300, sipm.getExpires());
			Assert::AreEqual<bool>(true, sipm.value("/h/X-subscribe-to-leg-events"_json_pointer, false));
		}

		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_1)
		{
			auto buffer		 = loadSampleFile("REGISTER_1");
			auto bufferStart = buffer.begin();
			auto sipm		 = sip2json::parseFromBuffer(bufferStart, buffer.end());

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<std::string>(METHOD_REGISTER, sipm.value("/s/method"_json_pointer, ""));
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size() == 1);
			Assert::AreEqual<std::string>("SIP/2.0/TCP il-ed-mara-01.ring2.com:8443",
										  sipm.value("/h/Via/0"_json_pointer, nlohmann::json {}));
			// Call-ID
			Assert::AreEqual<std::string>("8DC1AF9E-8C37-4463-B8C9-1959A1428116", sipm.getCallID());
			// Content-Type
			//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
			Assert::AreEqual<uint32_t>(300, sipm.getExpires());
			Assert::AreEqual<bool>(true, sipm.header<bool>("X-subscribe-to-leg-events"));
		}


		// NOLINTNEXTLINE
		TEST_METHOD(OK_REGISTER_Multiline_ContactHeader_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto item		 = 0;
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			// Source file has three frames
			Assert::AreEqual<size_t>(3, msgs.size());
			// Affirm that the first frame has a Contact that has been unfolded properly!
			Assert::AreEqual<std::string>(
					msgs[0].value("/h/Contact"_json_pointer, ""),
					"sip:jcollier@federationbankia.com;expires=1593725109;tag=sp2(263988)_IL-PS-CONGO-02.ring2.com, "
					"sip:jcollier@federationbankia.com;expires=1593725109;tag=sp2(26392)_IL-PS-CONGO-01.ring2.com, "
					"sip:jcollier@federationbankia.com;expires=1593725269;tag=65750151432167il-ed-mara-01__sp3[USCHEQ-ASRTA01."
					"ring2.com]");
			// Affirm that the second item's contact is a single line
			Assert::AreEqual<std::string>(msgs[1].value("/h/Contact"_json_pointer, ""), "<sip:216.111.92.37:8443;transport=ssl>");

			// Affirm that the third element's contact ends with a space.
			Assert::AreEqual<std::string>(msgs[2].value("/h/Contact"_json_pointer, ""),
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
			auto buffer		 = loadSampleFile(__func__); // NOLINT
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
			Assert::AreEqual<std::string>(msgs[0].value("/h/Contact"_json_pointer, ""), "<sip:localhost:8443;transport=ssl>");
			// We should have 4 SDP elements
			Assert::AreEqual<size_t>(4, msgs[0].value("/b/sdp"_json_pointer, nlohmann::json {}).size());
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Trying_INVITE_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			bool passTest	 = false;

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end(), [&](sipmessage& sipm) {
				Assert::IsTrue(!sipm.empty(), L"Expect valid one message parsed.");
				Assert::AreEqual<std::string>("1593721670540996", sipm.headers().value("X-Message-Time", ""));
				Assert::AreEqual<std::string>("3 INVITE", sipm.headers().value("CSeq", ""));
				Assert::AreEqual<uint32_t>(100, sipm.getStatusCode());
				Assert::AreEqual<std::string>("Trying", sipm.getReason());
				Assert::AreEqual<std::string>(
						"X-Signed start=\"1593721669\",expire=\"1593725269\",user=\"jaaaaaaa@aaaaaaaaaaaaaaaa.com\",confwiz=\"my "
						"string\",nsadrs=\"aa-aa-aaaa-00.ring2.com\",signed=\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa7\"",
						sipm.headers().value("Authorization", ""));
				passTest = true;
			});

			Assert::IsTrue(passTest);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(Mixed_Stream_1)
		{
			auto		buffer		= loadSampleFile(__func__); // NOLINT
			auto		item		= 0;
			auto		bufferStart = buffer.begin();
			std::vector matchTarget {"", // 0 element is dud.
									 "2 SUBSCRIBE",
									 "1 NOTIFY",
									 "3 INVITE",
									 "32 NOTIFY",
									 "33 NOTIFY",
									 "34 NOTIFY",
									 "35 NOTIFY",
									 "36 NOTIFY",
									 "37 NOTIFY",
									 "4 INVITE",
									 "38 NOTIFY",
									 "39 NOTIFY",
									 "40 NOTIFY",
									 "41 NOTIFY",
									 "42 NOTIFY",
									 "43 NOTIFY",
									 "44 NOTIFY",
									 "45 NOTIFY"};

			std::map<std::string, uint32_t> counters;

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());

			for (auto& i : msgs)
			{
				item++;
				auto str = fmt::format("{} - document {} -> found:{}.....expected:{}\n",
									   __func__,
									   item,
									   i.value("/h/CSeq"_json_pointer, ""),
									   matchTarget[item]);
				Logger::WriteMessage(str.c_str());

				counters[i.value("/h/CSeq"_json_pointer, "")]++;

				if (i.getStatusCode() != 0)
					counters[i.value("/s/reason"_json_pointer, "")]++;
				else
					counters[i.value("/h/method"_json_pointer, "")]++;

				// Check for each item; match the CSeq
				Assert::AreEqual<std::string>(matchTarget[item], i.value("/h/CSeq"_json_pointer, ""));
			}

			Logger::WriteMessage(fmt::format("{} - Found: {} messages\n", __func__, msgs.size()).c_str());
			Assert::AreEqual<size_t>(matchTarget.size() - 1, msgs.size(), L"Expect 18 messages parsed.");
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Mixed_Stream_2)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto item		 = 0;
			auto bufferStart = buffer.begin();
			// CSEQ, Content-Length, No items in sdp
			std::vector<std::tuple<std::string, size_t, size_t>> matchTarget {{"", 0, 0}, // 0 element is dud.
																			  {"1 NOTIFY", 1999, 2},
																			  {"2 NOTIFY", 1099, 1},
																			  {"5 NOTIFY", 1219, 1},
																			  {"6 NOTIFY", 1197, 1},
																			  {"8 NOTIFY", 1326, 1},
																			  {"1 NOTIFY", 615, 1},
																			  {"9 NOTIFY", 1081, 1},
																			  {"10 NOTIFY", 1201, 1},
																			  {"12 NOTIFY", 1326, 1}};

			std::map<std::string, uint32_t> counters;

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());

			for (auto& i : msgs)
			{
				item++;
				auto str = fmt::format("{} - document {} -> found:{}.....expected:{}; CL:{}-->{}\n",
									   __func__,
									   item,
									   i.value("/h/CSeq"_json_pointer, ""),
									   std::get<0>(matchTarget[item]),
									   i.getContentLength(),
									   std::get<1>(matchTarget[item])


				);
				Logger::WriteMessage(str.c_str());

				counters[i.value("/h/CSeq"_json_pointer, "")]++;

				if (i.getStatusCode() != 0)
					counters[i.value("/s/reason"_json_pointer, "")]++;
				else
					counters[i.value("/h/method"_json_pointer, "")]++;

				// Check for each item; match the CSeq
				Assert::AreEqual<std::string>(std::get<0>(matchTarget[item]), i.value("/h/CSeq"_json_pointer, ""));
				Assert::AreEqual<size_t>(std::get<1>(matchTarget[item]), i.getContentLength());
				Assert::AreEqual<size_t>(std::get<2>(matchTarget[item]), i["b"]["sdp"].size());
			}

			Logger::WriteMessage(fmt::format("{} - Found: {} messages\n", __func__, msgs.size()).c_str());
			Assert::AreEqual<size_t>(matchTarget.size() - 1, msgs.size(), L"Expect 9 messages parsed.");
		}

		// NOLINTNEXTLINE
		TEST_METHOD(RandomStream_Recv_File_1)
		{
			auto							buffer		= loadSampleFile(__func__); // NOLINT
			auto							item		= 0;
			auto							bufferStart = buffer.begin();
			std::vector						matchTarget {"", // 0 element is dud.
									 "1 REGISTER",
									 "848352898 SUBSCRIBE",
									 "1 REGISTER",
									 "31 NOTIFY",
									 "2 SUBSCRIBE",
									 "1 NOTIFY",
									 "3 INVITE",
									 "32 NOTIFY",
									 "33 NOTIFY",
									 "34 NOTIFY",
									 "35 NOTIFY",
									 "36 NOTIFY",
									 "37 NOTIFY",
									 "4 INVITE",
									 "38 NOTIFY",
									 "39 NOTIFY",
									 "40 NOTIFY",
									 "41 NOTIFY",
									 "42 NOTIFY",
									 "43 NOTIFY",
									 "44 NOTIFY",
									 "45 NOTIFY",
									 "46 NOTIFY",
									 "47 NOTIFY",
									 "48 NOTIFY",
									 "5 INVITE",
									 "49 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "50 NOTIFY",
									 "51 NOTIFY",
									 "52 NOTIFY",
									 "53 NOTIFY",
									 "6 INVITE",
									 "54 NOTIFY",
									 "55 NOTIFY",
									 "56 NOTIFY",
									 "57 NOTIFY",
									 "58 NOTIFY",
									 "59 NOTIFY",
									 "60 NOTIFY",
									 "7 INVITE",
									 "61 NOTIFY",
									 "62 NOTIFY",
									 "63 NOTIFY",
									 "64 NOTIFY",
									 "8 INVITE",
									 "65 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "66 NOTIFY",
									 "67 NOTIFY",
									 "68 NOTIFY",
									 "69 NOTIFY",
									 "9 INVITE",
									 "70 NOTIFY",
									 "71 NOTIFY",
									 "72 NOTIFY",
									 "73 NOTIFY",
									 "74 NOTIFY",
									 "75 NOTIFY",
									 "76 NOTIFY",
									 "77 NOTIFY",
									 "78 NOTIFY",
									 "79 NOTIFY",
									 "80 NOTIFY",
									 "81 NOTIFY",
									 "82 NOTIFY",
									 "83 NOTIFY",
									 "84 NOTIFY",
									 "85 NOTIFY",
									 "86 NOTIFY",
									 "87 NOTIFY",
									 "88 NOTIFY",
									 "89 NOTIFY",
									 "90 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "91 NOTIFY",
									 "92 NOTIFY",
									 "93 NOTIFY",
									 "94 NOTIFY",
									 "95 NOTIFY",
									 "96 NOTIFY",
									 "97 NOTIFY",
									 "98 NOTIFY",
									 "10 REGISTER",
									 "10 REGISTER",
									 "11 SUBSCRIBE",
									 "1 NOTIFY",
									 "12 INVITE",
									 "99 NOTIFY",
									 "13 INVITE",
									 "100 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "14 INVITE",
									 "101 NOTIFY",
									 "102 NOTIFY",
									 "103 NOTIFY",
									 "15 INVITE",
									 "104 NOTIFY",
									 "105 NOTIFY",
									 "106 NOTIFY",
									 "107 NOTIFY",
									 "108 NOTIFY",
									 "16 INVITE",
									 "109 NOTIFY",
									 "110 NOTIFY",
									 "111 NOTIFY",
									 "112 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "113 NOTIFY",
									 "114 NOTIFY",
									 "17 INVITE",
									 "115 NOTIFY",
									 "116 NOTIFY",
									 "117 NOTIFY",
									 "18 INVITE",
									 "118 NOTIFY",
									 "119 NOTIFY",
									 "120 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "121 NOTIFY",
									 "122 NOTIFY",
									 "123 NOTIFY",
									 "124 NOTIFY",
									 "19 INVITE",
									 "125 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "126 NOTIFY",
									 "127 NOTIFY",
									 "20 REGISTER",
									 "20 REGISTER",
									 "21 SUBSCRIBE",
									 "1 NOTIFY",
									 "128 NOTIFY",
									 "129 NOTIFY",
									 "1 REGISTER",
									 "848357801 SUBSCRIBE",
									 "8 NOTIFY",
									 "2 SUBSCRIBE",
									 "1 NOTIFY",
									 "3 INVITE",
									 "9 NOTIFY",
									 "4 INVITE",
									 "10 NOTIFY",
									 "11 NOTIFY",
									 "5 INVITE",
									 "12 NOTIFY",
									 "848357808 BYE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "22 INVITE",
									 "130 NOTIFY",
									 "131 NOTIFY",
									 "132 NOTIFY",
									 "133 NOTIFY",
									 "134 NOTIFY",
									 "135 NOTIFY",
									 "136 NOTIFY",
									 "137 NOTIFY",
									 "138 NOTIFY",
									 "139 NOTIFY",
									 "140 NOTIFY",
									 "141 NOTIFY",
									 "142 NOTIFY",
									 "143 NOTIFY",
									 "144 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "145 NOTIFY",
									 "146 NOTIFY",
									 "147 NOTIFY",
									 "148 NOTIFY",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "149 NOTIFY",
									 "150 NOTIFY",
									 "151 NOTIFY",
									 "152 NOTIFY",
									 "153 NOTIFY",
									 "154 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "155 NOTIFY",
									 "156 NOTIFY",
									 "157 NOTIFY",
									 "158 NOTIFY",
									 "159 NOTIFY",
									 "160 NOTIFY",
									 "161 NOTIFY",
									 "162 NOTIFY",
									 "163 NOTIFY",
									 "164 NOTIFY",
									 "165 NOTIFY",
									 "166 NOTIFY",
									 "167 NOTIFY",
									 "168 NOTIFY",
									 "169 NOTIFY",
									 "170 NOTIFY",
									 "171 NOTIFY",
									 "23 REGISTER",
									 "23 REGISTER",
									 "24 SUBSCRIBE",
									 "1 NOTIFY",
									 "172 NOTIFY",
									 "173 NOTIFY",
									 "174 NOTIFY",
									 "175 NOTIFY",
									 "176 NOTIFY",
									 "177 NOTIFY",
									 "178 NOTIFY",
									 "179 NOTIFY",
									 "180 NOTIFY",
									 "181 NOTIFY",
									 "182 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "183 NOTIFY",
									 "184 NOTIFY",
									 "185 NOTIFY",
									 "186 NOTIFY",
									 "187 NOTIFY",
									 "188 NOTIFY",
									 "189 NOTIFY",
									 "190 NOTIFY",
									 "191 NOTIFY",
									 "192 NOTIFY",
									 "193 NOTIFY",
									 "194 NOTIFY",
									 "195 NOTIFY",
									 "196 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "197 NOTIFY",
									 "198 NOTIFY",
									 "199 NOTIFY",
									 "200 NOTIFY",
									 "201 NOTIFY",
									 "202 NOTIFY",
									 "203 NOTIFY",
									 "204 NOTIFY",
									 "205 NOTIFY",
									 "206 NOTIFY",
									 "207 NOTIFY",
									 "208 NOTIFY",
									 "209 NOTIFY",
									 "210 NOTIFY",
									 "211 NOTIFY",
									 "212 NOTIFY",
									 "213 NOTIFY",
									 "214 NOTIFY",
									 "215 NOTIFY",
									 "216 NOTIFY",
									 "217 NOTIFY",
									 "218 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "219 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "2 REGISTER",
									 "2 REGISTER",
									 "1 INVITE",
									 "1 NOTIFY",
									 "853403460 SUBSCRIBE",
									 "2 NOTIFY",
									 "1 NOTIFY",
									 "853403463 SUBSCRIBE",
									 "3 NOTIFY",
									 "1 NOTIFY",
									 "220 NOTIFY",
									 "221 NOTIFY",
									 "222 NOTIFY",
									 "4 NOTIFY",
									 "5 NOTIFY",
									 "223 NOTIFY",
									 "224 NOTIFY",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "225 NOTIFY",
									 "226 NOTIFY",
									 "227 NOTIFY",
									 "228 NOTIFY",
									 "3 INVITE",
									 "6 NOTIFY",
									 "7 NOTIFY",
									 "8 NOTIFY",
									 "229 NOTIFY",
									 "230 NOTIFY",
									 "4 INVITE",
									 "9 NOTIFY",
									 "10 NOTIFY",
									 "231 NOTIFY",
									 "232 NOTIFY",
									 "233 NOTIFY",
									 "234 NOTIFY",
									 "235 NOTIFY",
									 "236 NOTIFY",
									 "237 NOTIFY",
									 "238 NOTIFY",
									 "239 NOTIFY",
									 "240 NOTIFY",
									 "241 NOTIFY",
									 "242 NOTIFY",
									 "5 INVITE",
									 "11 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "25 REGISTER",
									 "25 REGISTER",
									 "26 SUBSCRIBE",
									 "1 NOTIFY",
									 "243 NOTIFY",
									 "244 NOTIFY",
									 "245 NOTIFY",
									 "246 NOTIFY",
									 "247 NOTIFY",
									 "248 NOTIFY",
									 "249 NOTIFY",
									 "250 NOTIFY",
									 "251 NOTIFY",
									 "252 NOTIFY",
									 "253 NOTIFY",
									 "254 NOTIFY",
									 "255 NOTIFY",
									 "256 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "257 NOTIFY",
									 "258 NOTIFY",
									 "259 NOTIFY",
									 "260 NOTIFY",
									 "261 NOTIFY",
									 "262 NOTIFY",
									 "263 NOTIFY",
									 "264 NOTIFY",
									 "265 NOTIFY",
									 "266 NOTIFY",
									 "267 NOTIFY",
									 "268 NOTIFY",
									 "269 NOTIFY",
									 "270 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "271 NOTIFY",
									 "272 NOTIFY",
									 "273 NOTIFY",
									 "274 NOTIFY",
									 "275 NOTIFY",
									 "276 NOTIFY",
									 "277 NOTIFY",
									 "278 NOTIFY",
									 "279 NOTIFY",
									 "280 NOTIFY",
									 "281 NOTIFY",
									 "282 NOTIFY",
									 "283 NOTIFY",
									 "284 NOTIFY",
									 "285 NOTIFY",
									 "286 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "6 REGISTER",
									 "6 REGISTER",
									 "7 SUBSCRIBE",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "287 NOTIFY",
									 "288 NOTIFY",
									 "289 NOTIFY",
									 "290 NOTIFY",
									 "291 NOTIFY",
									 "292 NOTIFY",
									 "293 NOTIFY",
									 "294 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "295 NOTIFY",
									 "296 NOTIFY",
									 "297 NOTIFY",
									 "298 NOTIFY",
									 "299 NOTIFY",
									 "300 NOTIFY",
									 "301 NOTIFY",
									 "302 NOTIFY",
									 "303 NOTIFY",
									 "304 NOTIFY",
									 "305 NOTIFY",
									 "306 NOTIFY",
									 "307 NOTIFY",
									 "308 NOTIFY",
									 "309 NOTIFY",
									 "310 NOTIFY",
									 "311 NOTIFY",
									 "312 NOTIFY",
									 "313 NOTIFY",
									 "27 REGISTER",
									 "28 SUBSCRIBE",
									 "1 NOTIFY",
									 "314 NOTIFY",
									 "315 NOTIFY",
									 "27 REGISTER",
									 "316 NOTIFY",
									 "317 NOTIFY",
									 "318 NOTIFY",
									 "319 NOTIFY",
									 "320 NOTIFY",
									 "321 NOTIFY",
									 "322 NOTIFY",
									 "323 NOTIFY",
									 "324 NOTIFY",
									 "848361553 BYE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "8 REGISTER",
									 "8 REGISTER",
									 "9 SUBSCRIBE",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "1 NOTIFY",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE",
									 "1 REGISTER",
									 "1 REGISTER",
									 "2 SUBSCRIBE"};
			std::map<std::string, uint32_t> counters;
			std::string						debugBuffer = "[";

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());

			for (auto& i : msgs)
			{
				item++;
				auto str = fmt::format("{} - document {} -> found:{}.....expected:{}\n",
									   __func__,
									   item,
									   i.value("/h/CSeq"_json_pointer, ""),
									   matchTarget[item]);
				Logger::WriteMessage(str.c_str());

				counters[i.value("/h/CSeq"_json_pointer, "")]++;

				if (i.getStatusCode() != 0)
					counters[i.value("/s/reason"_json_pointer, "")]++;
				else
					counters[i.value("/h/method"_json_pointer, "")]++;

				debugBuffer += i.dump(4);
				debugBuffer += ",";

				// Check for each item; match the CSeq
				Assert::AreEqual<std::string>(matchTarget[item], i.header<std::string>("CSeq"));
			}

			debugBuffer += "]";
			writeSampleFile(__func__, debugBuffer); // NOLINT

			Logger::WriteMessage(fmt::format("{} - Found: {} messages\n", __func__, msgs.size()).c_str());
			Assert::AreEqual<size_t>(matchTarget.size() - 1, msgs.size(), L"Expected 459 messages parsed.");
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_EmptyHeaderKey_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
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
			Assert::AreEqual<std::string>("<sip:localhost:8443;transport=ssl>", msgs[0].header<std::string>("Contact"));
			Assert::AreEqual<size_t>(1, msgs[0].value("/b/sdp"_json_pointer, nlohmann::json {}).size());
			Assert::AreEqual<size_t>(729, msgs[0].getContentLength());

			// `X-rss-id: ` should end up with a valid empty string header.
			Assert::AreEqual<std::string>("", msgs[0].value("/h/X-rss-id"_json_pointer, "-"));
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_connectorleg_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto item		 = 0;
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			// We're going to have a single frame
			Assert::AreEqual<size_t>(1, msgs.size());

			for (auto& i : msgs)
			{
				auto str = fmt::format("{} - document {} -> {}\n", __func__, ++item, i.dump(2));
				Logger::WriteMessage(str.c_str());
			}

			// Affirm that the first frame has a Contact that has been unfolded properly!
			Assert::AreEqual<std::string>(msgs[0].value("/h/Contact"_json_pointer, ""), "<sip:localhost:8443;transport=ssl>");
			Assert::AreEqual<size_t>(1, msgs[0].value("/b/sdp"_json_pointer, nlohmann::json {}).size());
			Assert::AreEqual<size_t>(886, msgs[0].getContentLength());

			// c=IN IP4 10.254.254.33
			Assert::AreEqual<std::string>("IN", msgs[0].value("/b/sdp/0/c/type"_json_pointer, ""));
			Assert::AreEqual<std::string>("IP4", msgs[0].value("/b/sdp/0/c/subtype"_json_pointer, ""));
			Assert::AreEqual<std::string>("10.254.254.33", msgs[0].value("/b/sdp/0/c/dn"_json_pointer, ""));

			// a=rtpmap should have 2 entries
			Assert::IsTrue(msgs[0].value("/b/sdp/0/a/rtpmap"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(2, msgs[0].value("/b/sdp/0/a/rtpmap"_json_pointer, nlohmann::json {}).size());
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_generic_1)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			// We're going to have a single frame
			Assert::AreEqual<size_t>(1, msgs.size());

			for (auto& i : msgs)
			{
				writeSampleFile(__func__, i.dump(4)); // NOLINT
			}

			// Affirm that the first frame has a Contact that has been unfolded properly!
			Assert::AreEqual<std::string>(msgs[0].value("/h/Contact"_json_pointer, ""), "<sip:localhost:8443;transport=ssl>");
			Assert::AreEqual<size_t>(1, msgs[0].value("/b/sdp"_json_pointer, nlohmann::json {}).size());
			Assert::AreEqual<size_t>(880, msgs[0].getContentLength());

			// c=IN IP4 10.254.254.33
			Assert::AreEqual<std::string>("IN", msgs[0].value("/b/sdp/0/c/type"_json_pointer, ""));
			Assert::AreEqual<std::string>("IP4", msgs[0].value("/b/sdp/0/c/subtype"_json_pointer, ""));
			Assert::AreEqual<std::string>("10.254.254.33", msgs[0].value("/b/sdp/0/c/dn"_json_pointer, ""));

			// a=rtpmap should have 2 entries
			Assert::IsTrue(msgs[0].value("/b/sdp/0/a/rtpmap"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(2, msgs[0].value("/b/sdp/0/a/rtpmap"_json_pointer, nlohmann::json {}).size());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Trying_INVITE_generic_1)
		{
			auto buffer		 = loadSampleFile("Trying_INVITE_1"); // NOLINT
			auto bufferStart = buffer.begin();
			bool passTest	 = false;

			auto msgs = sip2json::parseAllFromBuffer(bufferStart, buffer.end(), [&](sipmessage& sipm) {
				Assert::IsTrue(!sipm.empty(), L"Expect valid one message parsed.");
				writeSampleFile("Trying_INVITE_1", sipm.dump(4));
				Assert::AreEqual<std::string>("1593721670540996", sipm.headers().value("X-Message-Time", ""));
				Assert::AreEqual<std::string>("3 INVITE", sipm.headers().value("CSeq", ""));
				Assert::AreEqual<uint32_t>(100, sipm.getStatusCode());
				Assert::AreEqual<std::string>("Trying", sipm.getReason());
				passTest = true;
			});

			Assert::IsTrue(passTest);
		}
	}; // namespace test_suite


	// NOLINTNEXTLINE
	TEST_CLASS(parseAllFromBuffer)
	{
	public:
		bool dummy;
		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_startline)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(Test_parse_NOTIFY_1_headers)
		{
			auto buffer		 = loadSampleFile("NOTIFY_LegDrop");
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(siddiqsoftware::METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm.getContentLength());
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegAdd)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "15105076141563il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(783, sipm.getContentLength());

			Assert::AreEqual<std::string>("akirmayer@sidley.com", sipm.value("/h/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
			Assert::AreEqual<std::string>("2 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
			Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("MTUxMDUwNzYxNDE1NjNpbC1lZC1tYXJhLTAxOjE1OTM1NjQxNjc6Mjg0NDcw",
										  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
			Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2742801");
			// Check leg_no is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "12");
			// Check status is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(203) answered  ");
			// Check timing is parsed into array
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 1L), 3802556545L);
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 1L), 0L);

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "+6568898813");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "55706299459030");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "847687142");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "akirmayer@sidley.com");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-15.5.2.S4");
			Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
			Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false");
			//a=dialin:2742801:6660014385@205.252.237.66-$$-1
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/dialin"_json_pointer, ""),
										  "2742801:6660014385@205.252.237.66-$$-1");
		}

		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_LegDrop)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Start checking if we decoded properly..
			// METHOD: NOTIFY
			Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.getMethod());
			Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine", sipm.getUri());
			// Via is an array
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 4);
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "6732196043737il-ed-mara-01");
			// Content-Type
			Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(848, sipm.getContentLength());

			Assert::AreEqual<std::string>("jrbirge@nscorp.com", sipm.value("/h/X-control-master"_json_pointer, ""));
			Assert::AreEqual<std::string>("267 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
			Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
			Assert::AreEqual<bool>(false, sipm.value("/h/X-Billing-code-required"_json_pointer, true));
			Assert::AreEqual<std::string>("NjczMjE5NjA0MzczN2lsLWVkLW1hcmEtMDE6MTU5MzU0NTA2NTo4MDQ0NjU=",
										  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

			// Check the body
			Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
			Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
			// Check access_code is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2873116");
			// Check leg_no is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "24");
			// Check status is parsed
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
			// Check timing is parsed into array
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3802534341L);
			Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 3802534887L);

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+4044166441");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+4044166441");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Cell Phone   GA\"");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "148492049389635");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "847595153");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "jrbirge@nscorp.com");

			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
			Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
			Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
			Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false");
		}


		// NOLINTNEXTLINE
		TEST_METHOD(NOTIFY_CallEnd)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			auto verifyItems = [](sipmessage& sipm) {
				// Start checking if we decoded properly..
				// METHOD: NOTIFY
				Assert::AreEqual<std::string>(METHOD_NOTIFY, sipm.value("/s/method"_json_pointer, ""));
				Assert::AreEqual<std::string>("sip:subscribe_to_call_events@loopup.com;machine",
											  sipm.value("/s/uri"_json_pointer, ""));
				// Via is an array
				Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
				Assert::AreEqual<size_t>(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size(), 3);
				// Call-ID
				Assert::AreEqual<std::string>("119035121230567il-ed-mara-01", sipm.getCallID());
				// Content-Type
				Assert::AreEqual<std::string>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
				// Content-Length
				Assert::AreEqual<uint32_t>(1326, sipm.getContentLength());

				Assert::AreEqual<std::string>("matthew.gabbard@stblaw.com", sipm.value("/h/X-control-master"_json_pointer, ""));
				Assert::AreEqual<std::string>("", sipm.value("/h/X-rss-id"_json_pointer, "-"));
				Assert::AreEqual<std::string>("49 NOTIFY", sipm.value("/h/CSeq"_json_pointer, ""));
				Assert::AreEqual<bool>(true, sipm.value("/h/X-Billing-code-required"_json_pointer, false));
				Assert::AreEqual<std::string>("MTE5MDM1MTIxMjMwNTY3aWwtZWQtbWFyYS0wMToxNTkzNjM3MTcwOjE1Njc0Ng==",
											  sipm.value("/h/X-Call-Instance-ID"_json_pointer, ""));

				// Check the body
				Assert::IsTrue(!sipm.value("/b"_json_pointer, nlohmann::json {}).empty());
				Assert::IsTrue(sipm.value("/b/sdp"_json_pointer, nlohmann::json {}).is_array());
				Assert::IsTrue(sipm.value("/b/sdp/0/a"_json_pointer, nlohmann::json {}).is_object());
				// Check access_code is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/access_code"_json_pointer, ""), "2997255");
				// Check leg_no is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/leg_no"_json_pointer, ""), "2");
				// Check status is parsed
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/status"_json_pointer, ""), "(4) dropped");
				// Check timing is parsed into array
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/0"_json_pointer, 0L), 3802625984L);
				Assert::AreEqual<unsigned long>(sipm.value("/b/sdp/0/t/1"_json_pointer, 0L), 3802626770L);

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/type"_json_pointer, ""), "TN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/c/subtype"_json_pointer, ""), "RFC2543");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/dn"_json_pointer, ""), "+12124553521");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/name"_json_pointer, ""), "\"Matt%20Gabbard%20-%20\"");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/i/type"_json_pointer, ""), "CallByPhone-URL");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/host"_json_pointer, ""), "il-ed-mara-01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/subtype"_json_pointer, ""), "IP4");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t1"_json_pointer, ""), "3598380125");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/t2"_json_pointer, ""), "241");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/type"_json_pointer, ""), "IN");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/o/user"_json_pointer, ""), "matthew.gabbard@stblaw.com");

				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/cli-screening"_json_pointer, ""), "00");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/user-agent"_json_pointer, ""), "Cisco-SIPGateway/IOS-16.3.3");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/x-ring2-smartproxy"_json_pointer, ""),
											  "usaze-asalt01.ring2.com");
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/trunk"_json_pointer, ""), "8:chan:0");
				Assert::AreEqual<bool>(sipm.value("/b/sdp/0/a/new_change"_json_pointer, false), true);
				Assert::IsTrue(sipm.contains("/b/sdp/0/a/far_end"_json_pointer));
				Assert::AreEqual<std::string>(sipm.value("/b/sdp/0/a/clir"_json_pointer, ""), "false:18777464263");
			};

			verifyItems(sipm);
		}


		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_200_OK)
		{
			auto buffer		 = loadSampleFile(__func__); // NOLINT
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<uint32_t>(200, sipm.getStatusCode());
			// Via is an array
			Assert::IsTrue(sipm.headers()["Via"].is_array());
			Assert::AreEqual<size_t>(1, sipm.headers()["Via"].size());
			Assert::AreEqual<std::string>("SIP/2.0/TCP il-ed-mara-01.ring2.com:8443", sipm.headers()["Via"][0]);
			// Call-ID
			Assert::AreEqual<std::string>("8DC1AF9E-8C37-4463-B8C9-1959A1428116", sipm.getCallID());
			// Content-Length
			Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
			Assert::AreEqual<uint32_t>(300, sipm.getExpires());
			Assert::AreEqual<bool>(true, sipm.header<bool>("X-subscribe-to-leg-events"));
		}


		// NOLINTNEXTLINE
		TEST_METHOD(REGISTER_1)
		{
			auto buffer		 = loadSampleFile("REGISTER_1");
			auto bufferStart = buffer.begin();
			auto msgs		 = sip2json::parseAllFromBuffer(bufferStart, buffer.end());
			auto sipm		 = msgs[0];

			// Start checking if we decoded properly..
			// Start-Line (response): SIP/2.0 200 OK
			Assert::AreEqual<std::string>(METHOD_REGISTER, sipm.value("/s/method"_json_pointer, ""));
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).is_array());
			Assert::IsTrue(sipm.value("/h/Via"_json_pointer, nlohmann::json {}).size() == 1);
			Assert::AreEqual<std::string>(sipm.value("/h/Via/0"_json_pointer, nlohmann::json {}),
										  "SIP/2.0/TCP il-ed-mara-01.ring2.com:8443");
			// Call-ID
			Assert::AreEqual<std::string>(sipm.getCallID(), "8DC1AF9E-8C37-4463-B8C9-1959A1428116");
			// Content-Type
			//Assert::AreEqual<>(CONTENT_TYPE_APP_SDP, sipm.getContentType());
			// Content-Length
			Assert::AreEqual<uint32_t>(0, sipm.getContentLength());
			Assert::AreEqual<uint32_t>(300, sipm.getExpires());
			Assert::AreEqual<bool>(true, sipm.value("/h/X-subscribe-to-leg-events"_json_pointer, false));
		}
	};
} // namespace test_suite
