/*
  A SIP Parser for Modern C++ / Version 2.5.x
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
#include <set>
#include <vector>

#include <iomanip>
#include <sstream>

#include <format>
#include "nlohmann/json.hpp"

#include "siddiqsoft/sip2json.hpp"


#include "gtest/gtest.h"
#include <thread>

#if defined(_WIN32)
#include <Windows.h>
#include <processenv.h>
#else
#include <unistd.h>
#endif


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_callid_uniqueness)
{
    constexpr int         numIds = 100;
    std::set<std::string> ids;

    for (int i = 0; i < numIds; i++)
    {
        auto id = siddiqsoft::createCallId();
        EXPECT_EQ(44, id.length()) << "CallId length mismatch at iteration " << i;
        ids.insert(id);
    }
    EXPECT_EQ(numIds, ids.size()) << "Duplicate Call-IDs detected";
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_create_various_request_methods)
{
    std::vector<std::string> methods = {
            "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER", "SUBSCRIBE", "NOTIFY", "MESSAGE", "INFO"};

    for (const auto& method : methods)
    {
        siddiqsoft::sipmessage sipm(method, "sip:test@example.com", siddiqsoft::createCallId(), 1);
        EXPECT_TRUE(sipm.isMessageRequest()) << "Failed for method: " << method;
        EXPECT_EQ(method, sipm.getMethod()) << "Method mismatch for: " << method;
        EXPECT_FALSE(sipm.getCallID().empty()) << "Missing Call-ID for: " << method;
        EXPECT_EQ(std::format("1 {}", method), sipm.getHeader<std::string>("CSeq")) << "CSeq mismatch for: " << method;
    }
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_create_various_response_codes)
{
    std::vector<uint32_t> codes = {100, 180, 200, 302, 400, 401, 403, 404, 480, 486, 500, 503, 600, 603, 606};

    for (auto code : codes)
    {
        siddiqsoft::sipmessage sipm(code);
        EXPECT_TRUE(sipm.isMessageResponse()) << "Failed for code: " << code;
        EXPECT_EQ(code, sipm.getStatusCode()) << "Status code mismatch for: " << code;
        EXPECT_FALSE(sipm.getReason().empty()) << "Missing reason for code: " << code;
    }

    // Anything else should return an empty string.. and the status code will be
    siddiqsoft::sipmessage nonExistentSIPCode(9999);
    // The status code is maintained..
    EXPECT_EQ(9999, nonExistentSIPCode.getStatusCode());
    // But there is no reason string.. it will be an empty string!
    EXPECT_EQ(0, nonExistentSIPCode.getReason().length());
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_serialize_roundtrip_response)
{
    siddiqsoft::sipmessage request("REGISTER", "sip:test@example.com", siddiqsoft::createCallId(), 1);
    request.setHeader("To", "sip:test@example.com").setHeader("Contact", "sip:test@example.com");

    siddiqsoft::sipmessage response(200, request);

    try
    {
        auto serialized = siddiqsoft::sip2json::serialize(response);
        EXPECT_FALSE(serialized.empty());

        auto                   bs     = serialized.begin();
        siddiqsoft::sipmessage parsed = siddiqsoft::sip2json::parseFromBuffer(bs, serialized.end());

        EXPECT_TRUE(parsed.isMessageResponse());
        EXPECT_EQ(200, parsed.getStatusCode());
        EXPECT_EQ(response.getCallID(), parsed.getCallID());
        EXPECT_EQ(response.getContentLength(), parsed.getContentLength());
    }
    catch (const std::exception& e)
    {
        FAIL() << "Unexpected exception: " << e.what();
    }
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_empty_string_buffer_parse)
{
    std::string emptyBuffer;
    auto        bs = emptyBuffer.begin();

    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, emptyBuffer.end()));
    EXPECT_TRUE(sipm.empty() || sipm.size() <= 1) << "Expected empty or meta-only sipmessage from empty buffer";
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_sipmessage_default_has_meta)
{
    siddiqsoft::sipmessage sipm;
    EXPECT_TRUE(sipm.contains("meta"));
    EXPECT_FALSE(sipm.value("/meta/version"_json_pointer, std::string {}).empty());
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_setHeader_overwrite)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:test@example.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader("X-Custom", "value1");
    EXPECT_EQ("value1", sipm.getHeader<std::string>("X-Custom"));

    sipm.setHeader("X-Custom", "value2");
    EXPECT_EQ("value2", sipm.getHeader<std::string>("X-Custom"));
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_setHeader_batch_merge)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:test@example.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader({{"X-First", "one"}, {"X-Second", "two"}, {"X-Third", "three"}});

    EXPECT_EQ("one", sipm.getHeader<std::string>("X-First"));
    EXPECT_EQ("two", sipm.getHeader<std::string>("X-Second"));
    EXPECT_EQ("three", sipm.getHeader<std::string>("X-Third"));

    sipm.setHeader({{"X-First", "ONE"}, {"X-Fourth", "four"}});
    EXPECT_EQ("ONE", sipm.getHeader<std::string>("X-First"));
    EXPECT_EQ("two", sipm.getHeader<std::string>("X-Second"));
    EXPECT_EQ("four", sipm.getHeader<std::string>("X-Fourth"));
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_body_text_plain)
{
    siddiqsoft::sipmessage sipm("MESSAGE", "sip:test@example.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader("To", "sip:test@example.com")
            .setHeader("Contact", "sip:test@example.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_TEXT_PLAIN);

    sipm.body() = "Hello, World!";
    EXPECT_TRUE(sipm.body().is_string());
    EXPECT_EQ("Hello, World!", sipm.body().get<std::string>());

    try
    {
        auto serialized = siddiqsoft::sip2json::serialize(sipm);
        EXPECT_FALSE(serialized.empty());
        EXPECT_TRUE(serialized.find("Hello, World!") != std::string::npos) << serialized;
    }
    catch (const std::exception& e)
    {
        FAIL() << "Unexpected exception: " << e.what();
    }
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_TimeAsRFC3339_now)
{
    auto now = siddiqsoft::TimeAsRFC3339();
    EXPECT_FALSE(now.empty());
    EXPECT_EQ('Z', now.back());
    EXPECT_TRUE(now.find('T') != std::string::npos);
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_sipmessage_copy_semantics)
{
    siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", siddiqsoft::createCallId(), 1);
    original.setHeader("X-Custom", "test-value");

    siddiqsoft::sipmessage copy(original);
    EXPECT_EQ(original.getCallID(), copy.getCallID());
    EXPECT_EQ("test-value", copy.getHeader<std::string>("X-Custom"));

    copy.setHeader("X-Custom", "modified");
    EXPECT_EQ("test-value", original.getHeader<std::string>("X-Custom"));
    EXPECT_EQ("modified", copy.getHeader<std::string>("X-Custom"));
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_sipmessage_move_semantics)
{
    auto                   callId = siddiqsoft::createCallId();
    siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", callId, 1);

    siddiqsoft::sipmessage moved(std::move(original));
    EXPECT_EQ(callId, moved.getCallID());
    EXPECT_TRUE(moved.isMessageRequest());
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_serialize_unsupported_method_throws)
{
    siddiqsoft::sipmessage sipm("ROR", "sip:test@example.com");
    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);
}


// NOLINTNEXTLINE
TEST(edge_cases_2, Test_serialize_missing_headers_throws)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:test@example.com", siddiqsoft::createCallId(), 1);
    sipm.erase("h");

    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);
}


// NOLINTNEXTLINE
TEST(Issue33_SDPOptionalFields, OmitUnpopulatedOptionalSDPLines)
{
    // Minimal SDP without optional i=, u=, e=, p=, c= fields
    std::string rawMsg =
        "INVITE sip:bob@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK-issue33\r\n"
        "From: <sip:alice@example.com>;tag=1\r\n"
        "To: <sip:bob@example.com>\r\n"
        "Call-ID: issue33-omit-optional-sdp\r\n"
        "CSeq: 1 INVITE\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 104\r\n"
        "\r\n"
        "v=0\r\n"
        "o=alice 2890844526 2890844526 IN IP4 192.0.2.1\r\n"
        "s=Issue 33 Test\r\n"
        "t=0 0\r\n"
        "m=audio 49170 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto bs = rawMsg.begin();
    siddiqsoft::sipmessage sipm = siddiqsoft::sip2json::parseFromBuffer(bs, rawMsg.end());

    std::string serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_FALSE(serialized.contains("i=\r\n"));
    EXPECT_FALSE(serialized.contains("u=\r\n"));
    EXPECT_FALSE(serialized.contains("e=\r\n"));
    EXPECT_FALSE(serialized.contains("p=\r\n"));
    EXPECT_FALSE(serialized.contains("c=\r\n"));
}


// NOLINTNEXTLINE
TEST(Issue33_SDPOptionalFields, PreservePopulatedOptionalSDPLines)
{
    std::string rawMsg =
        "INVITE sip:bob@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK-issue33b\r\n"
        "From: <sip:alice@example.com>;tag=1\r\n"
        "To: <sip:bob@example.com>\r\n"
        "Call-ID: issue33-preserve-optional-sdp\r\n"
        "CSeq: 1 INVITE\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 172\r\n"
        "\r\n"
        "v=0\r\n"
        "o=alice 2890844526 2890844526 IN IP4 192.0.2.1\r\n"
        "s=Issue 33 Test\r\n"
        "i=Session Information Text\r\n"
        "c=IN IP4 192.0.2.1\r\n"
        "t=0 0\r\n"
        "m=audio 49170 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto bs = rawMsg.begin();
    siddiqsoft::sipmessage sipm = siddiqsoft::sip2json::parseFromBuffer(bs, rawMsg.end());

    std::string serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.contains("i=Session Information Text\r\n"));
    EXPECT_TRUE(serialized.contains("c=IN IP4 192.0.2.1\r\n"));
    EXPECT_FALSE(serialized.contains("u=\r\n"));
    EXPECT_FALSE(serialized.contains("e=\r\n"));
    EXPECT_FALSE(serialized.contains("p=\r\n"));
}
