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
#include <set>
#include <vector>

#include <iomanip>
#include <sstream>

#include <format>
#include "nlohmann/json.hpp"

#include "../include/siddiqsoft/sip2json.hpp"


#include "gtest/gtest.h"
#include <thread>

#if defined(_WIN32)
#include <Windows.h>
#include <processenv.h>
#else
#include <unistd.h>
#endif


// NOLINTNEXTLINE
TEST(core_parser_tests, Test_UserAgent)
{
    using namespace siddiqsoft;

    auto                   ua = __func__; //NOLINT
    siddiqsoft::sipmessage sipm(METHOD_REGISTER, "sip:hello@world.com");

    try
    {
        sipm.setUserAgent(ua);
        std::cerr << sip2json::serialize(sipm);
        EXPECT_TRUE(sipm.getUserAgent().find(ua) != std::string::npos);
        EXPECT_TRUE(sipm.getUserAgent().find("sip2json") != std::string::npos);
    }
    catch (const std::exception& e)
    {
        EXPECT_TRUE(true) << L"Got exception. " << e.what();
    }
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_meta_element)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com");

    try
    {
        std::clog << siddiqsoft::sip2json::serialize(sipm);
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
    auto ee = siddiqsoft::sip2jsonErrors::ok;

    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::empty_message;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_document;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_document_unsupported_content;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_document_unsupported_method;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_startline;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_createCallId)
{
    auto ci = siddiqsoft::createCallId();
    EXPECT_TRUE(ci.length() == 44);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC1123)
{
    auto todays_date = siddiqsoft::TimeAsRFC1123();
    EXPECT_TRUE(!todays_date.empty());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC1123_args)
{
    tm knowntm {};
    knowntm.tm_year  = 2010 - 1900;
    knowntm.tm_mon   = 11 - 1; // Nov
    knowntm.tm_mday  = 13;     // 13th
    knowntm.tm_hour  = 23;     // 23h
    knowntm.tm_min   = 29;     // 29m
    knowntm.tm_sec   = 0;      // 0s
    knowntm.tm_wday  = 6;      // Sat
    knowntm.tm_isdst = 0;

// When using mktime it screws up the conversion by always applying the local time to convert to UTC.
// So if we have UTC already, it will adjust this time with the timezone difference on this computer!
// Use _mkgmtime() on windows and timegm() on linux/macos
#ifdef _WINDOWS
    time_t tv1 {};
    tv1 = ::_mkgmtime64(&knowntm);
#else
    auto tv1 = ::timegm(&knowntm);
#endif

    char knowntmRepresentation[128] {'\0'};
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%A %c", &knowntm);
    std::clog << "     %A %c: " << knowntmRepresentation << std::endl;
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%FT%TZ", &knowntm);
    std::clog << "  strftime: " << knowntmRepresentation << std::endl;
    std::clog << "     ctime: " << ctime(&tv1) << std::endl;

    auto todays_date = siddiqsoft::TimeAsRFC1123(std::chrono::system_clock::from_time_t(tv1));
    EXPECT_EQ("Sat, 13 Nov 2010 23:29:00 GMT", todays_date);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC3339_args)
{
    tm knowntm {};
    knowntm.tm_year  = 2010 - 1900;
    knowntm.tm_mon   = 11 - 1; // Nov
    knowntm.tm_mday  = 13;     // 13th
    knowntm.tm_hour  = 23;     // 23h
    knowntm.tm_min   = 29;     // 29m
    knowntm.tm_sec   = 0;      // 0s
    knowntm.tm_wday  = 6;      // Sat
    knowntm.tm_isdst = 0;

// When using mktime it screws up the conversion by always applying the local time to convert to UTC.
// So if we have UTC already, it will adjust this time with the timezone difference on this computer!
// Use _mkgmtime() on windows and timegm() on linux/macos
#ifdef _WINDOWS
    time_t tv1 {};
    tv1 = ::_mkgmtime64(&knowntm);
#else
    auto tv1 = ::timegm(&knowntm);
#endif

    char knowntmRepresentation[128] {'\0'};
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%A %c", &knowntm);
    std::clog << "     %A %c: " << knowntmRepresentation << std::endl;
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%FT%TZ", &knowntm);
    std::clog << "  strftime: " << knowntmRepresentation << std::endl;
    std::clog << "     ctime: " << ctime(&tv1) << std::endl;

    auto todays_date = siddiqsoft::TimeAsRFC3339(std::chrono::system_clock::from_time_t(tv1));
    EXPECT_EQ("2010-11-13T23:29:00.000Z", todays_date);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601)
{
    auto todays_date = siddiqsoft::TimeAsISO8601();
    EXPECT_TRUE(!todays_date.empty());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601_args)
{
    tm knowntm {};
    knowntm.tm_year  = 2010 - 1900;
    knowntm.tm_mon   = 11 - 1; // Nov
    knowntm.tm_mday  = 13;     // 13th
    knowntm.tm_hour  = 23;     // 23h
    knowntm.tm_min   = 29;     // 29m
    knowntm.tm_sec   = 59;     // 59s
    knowntm.tm_wday  = 6;      // Sat
    knowntm.tm_isdst = 0;

// When using mktime it screws up the conversion by always applying the local time to convert to UTC.
// So if we have UTC already, it will adjust this time with the timezone difference on this computer!
// Use _mkgmtime() on windows and timegm() on linux/macos
#ifdef _WINDOWS
    time_t tv1 {};
    tv1 = ::_mkgmtime64(&knowntm);
#else
    auto tv1 = ::timegm(&knowntm);
#endif

    // When we convert, the resulting epoch should match!
    EXPECT_EQ(tv1, 1289690999L) << "tv0: " << tv1 << std::endl;

    char knowntmRepresentation[128] {'\0'};
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%A %c", &knowntm);
    std::clog << "     %A %c: " << knowntmRepresentation << std::endl;
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%FT%TZ", &knowntm);
    std::clog << "  strftime: " << knowntmRepresentation << std::endl;
    std::clog << "     ctime: " << ctime(&tv1) << std::endl;

    auto knownDate = siddiqsoft::TimeAsISO8601(std::chrono::system_clock::from_time_t(tv1));
    EXPECT_EQ("2010-11-13T23:29:59.000Z", knownDate);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601_epoch)
{
    auto timeAsEpoch {1289690999L}; // Corresponds to Saturday, November 13, 2010 11:29:59 PM

    auto knownDate = siddiqsoft::TimeAsISO8601(std::chrono::system_clock::from_time_t(timeAsEpoch));
    EXPECT_EQ("2010-11-13T23:29:59.000Z", knownDate);
}


// NOLINTNEXTLINE
TEST(ImplementationChecks, Test_stream_serializer_1)
{
    std::stringstream sstr {};

    // This should compile without issue.
    sstr << __func__ << " format " << siddiqsoft::SIPMessageType::request << " and " << siddiqsoft::SIPMessageType::response
         << " checks out." << std::endl;

    auto msg = sstr.str();

    EXPECT_TRUE(msg.find("request") != std::string::npos);
    EXPECT_TRUE(msg.find("response") != std::string::npos);

    std::cerr << msg << std::endl;
}

// NOLINTNEXTLINE
TEST(ImplementationChecks, Test_formatters_1)
{
    // This should compile without issue.
    auto msg0 = std::format("{} format ", __func__);
    auto msg1 = std::format("{} and ", siddiqsoft::SIPMessageType::request);
    auto msg2 = std::format("{} checks out.", siddiqsoft::SIPMessageType::response);
    auto msg  = msg0.append(msg1).append(msg2);

    EXPECT_TRUE(msg.find("request") != std::string::npos) << " .. expected `request` in the string : " << msg;
    EXPECT_TRUE(msg.find("response") != std::string::npos) << " .. expected `response` in the string: " << msg;

    std::cerr << "msg0 : " << msg0 << std::endl;
    std::cerr << "msg1 : " << msg1 << std::endl;
    std::cerr << "msg2 : " << msg2 << std::endl;
    std::cerr << "msg  : " << msg << std::endl;
}

// ============================================================================
// GITHUB ISSUE #32 TESTS: Header Layout & Private Folder Refactoring
// ============================================================================

TEST(Issue32_HeaderRefactoring, ModularHeaderIntegrity)
{
    using namespace siddiqsoft;

    // Test 1: Verify sipmessage creation and status mapping via private response codes header
    sipmessage req(METHOD_INVITE, "sip:alice@atlanta.com", "issue32-call-id", 1);
    EXPECT_EQ("INVITE", req.getMethod());
    EXPECT_EQ("sip:alice@atlanta.com", req.getUri());
    EXPECT_EQ("issue32-call-id", req.getCallID());

    sipmessage resp(200, req);
    EXPECT_EQ(200, resp.getStatusCode());
    EXPECT_EQ("OK", resp.getReason());

    // Test 2: Verify wire serialization via refactored private/sip2json_serializer.hpp
    std::string serialized = sip2json::serialize(resp);
    EXPECT_FALSE(serialized.empty());
    EXPECT_TRUE(serialized.find("SIP/2.0 200 OK\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Call-ID: issue32-call-id\r\n") != std::string::npos);

    // Test 3: Verify parsing via refactored private/sip2json_parser.hpp
    auto start = serialized.begin();
    auto parsedResp = sip2json::parseFromBuffer(start, serialized.end());
    EXPECT_EQ(200, parsedResp.getStatusCode());
    EXPECT_EQ("OK", parsedResp.getReason());
    EXPECT_EQ("issue32-call-id", parsedResp.getCallID());
}

TEST(Issue32_HeaderRefactoring, PrivateSDPAndAsyncParserExecution)
{
    using namespace siddiqsoft;

    std::string sipWithSDP =
        "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: Bob <sip:bob@biloxi.com>\r\n"
        "From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.atlanta.com>\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    // Test 1: Asynchronous stream parsing via private/sip2json_parser.hpp and private/sip2json_sdp.hpp
    size_t parsedCount = 0;
    std::string buffer = sipWithSDP;
    sip2json::parseAsync(
        buffer,
        [&parsedCount](sipmessage&& msg) {
            parsedCount++;
            EXPECT_EQ("INVITE", msg.getMethod());
            EXPECT_EQ("a84b4c76e66710@pc33.atlanta.com", msg.getCallID());
            EXPECT_TRUE(msg.contains("b"));
            EXPECT_TRUE(msg.contains("/b/sdp"_json_pointer));
        },
        [](const sip2json_exception& ex, std::string::iterator&, const std::string::iterator&) {
            FAIL() << "Unexpected parse exception: " << ex.what();
        }
    );

    EXPECT_EQ(1u, parsedCount);
    EXPECT_TRUE(buffer.empty()); // Check that consumed bytes were erased

    // Test 2: Exception hierarchy in private/sip2json_exception.hpp
    invalid_document_error err("Issue #32 Exception Test");
    EXPECT_EQ(sip2jsonErrors::invalid_document, err.errCode);
    EXPECT_STREQ("Issue #32 Exception Test", err.what());
}

TEST(Issue32_RepeatedHeaders, MultipleViaHeaders)
{
    using namespace siddiqsoft;

    std::string sipWithMultipleVia =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP proxy1.example.com:5060;branch=z9hG4bK776a\r\n"
        "Via: SIP/2.0/UDP proxy2.example.com:5060;branch=z9hG4bK776b\r\n"
        "Via: SIP/2.0/UDP client.example.com:5060;branch=z9hG4bK776c\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: multi-via-callid-101\r\n"
        "CSeq: 1 INVITE\r\n"
        "Content-Length: 0\r\n\r\n";

    auto start = sipWithMultipleVia.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipWithMultipleVia.end());

    // Verify Via header exists and is an array containing all 3 Via lines
    EXPECT_TRUE(sipm["h"].contains("Via"));
    EXPECT_TRUE(sipm["h"]["Via"].is_array());
    EXPECT_EQ(3u, sipm["h"]["Via"].size());
    EXPECT_EQ("SIP/2.0/UDP proxy1.example.com:5060;branch=z9hG4bK776a", sipm["h"]["Via"][0].get<std::string>());
    EXPECT_EQ("SIP/2.0/UDP proxy2.example.com:5060;branch=z9hG4bK776b", sipm["h"]["Via"][1].get<std::string>());
    EXPECT_EQ("SIP/2.0/UDP client.example.com:5060;branch=z9hG4bK776c", sipm["h"]["Via"][2].get<std::string>());

    // Verify round-trip serialization preserves all Via header lines
    std::string serialized = sip2json::serialize(sipm);
    EXPECT_TRUE(serialized.find("Via: SIP/2.0/UDP proxy1.example.com:5060;branch=z9hG4bK776a\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Via: SIP/2.0/UDP proxy2.example.com:5060;branch=z9hG4bK776b\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Via: SIP/2.0/UDP client.example.com:5060;branch=z9hG4bK776c\r\n") != std::string::npos);
}

TEST(Issue32_RepeatedHeaders, MultipleRouteAndRecordRouteHeaders)
{
    using namespace siddiqsoft;

    std::string sipWithRoutes =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP client.example.com:5060;branch=z9hG4bK1\r\n"
        "Record-Route: <sip:p1.example.com;lr>\r\n"
        "Record-Route: <sip:p2.example.com;lr>\r\n"
        "Route: <sip:p3.example.com;lr>\r\n"
        "Route: <sip:p4.example.com;lr>\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: <sip:caller@example.com>;tag=abc\r\n"
        "Call-ID: route-test-callid-202\r\n"
        "CSeq: 2 INVITE\r\n"
        "Content-Length: 0\r\n\r\n";

    auto start = sipWithRoutes.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipWithRoutes.end());

    // Verify Record-Route headers
    EXPECT_TRUE(sipm["h"]["Record-Route"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Record-Route"].size());
    EXPECT_EQ("<sip:p1.example.com;lr>", sipm["h"]["Record-Route"][0].get<std::string>());
    EXPECT_EQ("<sip:p2.example.com;lr>", sipm["h"]["Record-Route"][1].get<std::string>());

    // Verify Route headers
    EXPECT_TRUE(sipm["h"]["Route"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Route"].size());
    EXPECT_EQ("<sip:p3.example.com;lr>", sipm["h"]["Route"][0].get<std::string>());
    EXPECT_EQ("<sip:p4.example.com;lr>", sipm["h"]["Route"][1].get<std::string>());
}

TEST(Issue32_RepeatedHeaders, MultipleCustomAndStandardHeaders)
{
    using namespace siddiqsoft;

    std::string sipWithCustomRepeated =
        "REGISTER sip:example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK-reg\r\n"
        "Contact: <sip:user1@10.0.0.1:5060>\r\n"
        "Contact: <sip:user2@10.0.0.2:5060>\r\n"
        "X-Trace-ID: trace-001\r\n"
        "X-Trace-ID: trace-002\r\n"
        "X-Trace-ID: trace-003\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: <sip:user@example.com>;tag=reg123\r\n"
        "Call-ID: reg-call-303\r\n"
        "CSeq: 1 REGISTER\r\n"
        "Content-Length: 0\r\n\r\n";

    auto start = sipWithCustomRepeated.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipWithCustomRepeated.end());

    // Verify repeated Contact headers converted to array
    EXPECT_TRUE(sipm["h"]["Contact"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Contact"].size());
    EXPECT_EQ("<sip:user1@10.0.0.1:5060>", sipm["h"]["Contact"][0].get<std::string>());
    EXPECT_EQ("<sip:user2@10.0.0.2:5060>", sipm["h"]["Contact"][1].get<std::string>());

    // Verify repeated custom header X-Trace-ID converted to array
    EXPECT_TRUE(sipm["h"]["X-Trace-ID"].is_array());
    EXPECT_EQ(3u, sipm["h"]["X-Trace-ID"].size());
    EXPECT_EQ("trace-001", sipm["h"]["X-Trace-ID"][0].get<std::string>());
    EXPECT_EQ("trace-002", sipm["h"]["X-Trace-ID"][1].get<std::string>());
    EXPECT_EQ("trace-003", sipm["h"]["X-Trace-ID"][2].get<std::string>());

    // Verify serialization produces separate header lines for X-Trace-ID
    std::string serialized = sip2json::serialize(sipm);
    EXPECT_TRUE(serialized.find("X-Trace-ID: trace-001\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("X-Trace-ID: trace-002\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("X-Trace-ID: trace-003\r\n") != std::string::npos);
}

// ============================================================================
// GITHUB ISSUE #29 TESTS: RFC 3261 Case-Insensitive Header Matching & Normalization
// ============================================================================

TEST(Issue29_CaseInsensitiveHeaders, LowercaseContentLengthAndSDP)
{
    using namespace siddiqsoft;

    std::string sipLowercase =
        "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
        "via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
        "max-forwards: 70\r\n"
        "to: Bob <sip:bob@biloxi.com>\r\n"
        "from: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
        "call-id: lower-callid-101\r\n"
        "cseq: 314159 INVITE\r\n"
        "contact: <sip:alice@pc33.atlanta.com>\r\n"
        "content-type: application/sdp\r\n"
        "content-length: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipLowercase.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipLowercase.end());

    // Verify getter functions resolve correctly with lowercase headers
    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("lower-callid-101", sipm.getCallID());
    EXPECT_TRUE(sipm.contains("b"));
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
}

#if defined(sip2json_HEADERKEY_MODE_INSENSITIVE)
TEST(Issue29_CaseInsensitiveHeaders, MixedCaseHeaders)
{
    using namespace siddiqsoft;

    std::string sipMixedCase =
        "REGISTER sip:example.com SIP/2.0\r\n"
        "vIa: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK-reg\r\n"
        "tO: <sip:user@example.com>\r\n"
        "fRoM: <sip:user@example.com>;tag=reg123\r\n"
        "cAlL-iD: mixed-callid-202\r\n"
        "cSeq: 1 REGISTER\r\n"
        "eXpIrEs: 3600\r\n"
        "cOnTeNt-LeNgTh: 0\r\n\r\n";

    auto start = sipMixedCase.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipMixedCase.end());

    EXPECT_EQ(0u, sipm.getContentLength());
    EXPECT_EQ("mixed-callid-202", sipm.getCallID());
    EXPECT_EQ(3600u, sipm.getExpires());
}

TEST(Issue29_CaseInsensitiveHeaders, UppercaseHeaders)
{
    using namespace siddiqsoft;

    std::string sipUppercase =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "VIA: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n"
        "MAX-FORWARDS: 70\r\n"
        "TO: <sip:user@example.com>\r\n"
        "FROM: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "CALL-ID: upper-callid-303\r\n"
        "CSEQ: 1 INVITE\r\n"
        "CONTENT-TYPE: application/sdp\r\n"
        "CONTENT-LENGTH: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipUppercase.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipUppercase.end());

    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("upper-callid-303", sipm.getCallID());
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
}
#endif

TEST(Issue29_CaseInsensitiveHeaders, CompactHeaderNames)
{
    using namespace siddiqsoft;

    std::string sipCompact =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "v: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n"
        "t: <sip:user@example.com>\r\n"
        "f: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "i: compact-callid-404\r\n"
        "CSeq: 1 INVITE\r\n"
        "c: application/sdp\r\n"
        "l: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipCompact.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipCompact.end());

    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("compact-callid-404", sipm.getCallID());
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
}

// ============================================================================
// GITHUB ISSUE #30 TESTS: RFC 3261 Compact Header Names Support
// ============================================================================

TEST(Issue30_CompactHeaders, AllTenCompactHeaders)
{
    using namespace siddiqsoft;

    // SIP message using all 10 RFC 3261 compact header forms:
    // v -> Via
    // f -> From
    // t -> To
    // i -> Call-ID
    // m -> Contact
    // c -> Content-Type
    // l -> Content-Length
    // s -> Subject
    // e -> Content-Encoding
    // k -> Supported
    std::string sipCompactAll =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "v: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n"
        "f: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "t: Bob <sip:bob@example.com>\r\n"
        "i: compact-all-callid-101\r\n"
        "m: <sip:alice@pc33.example.com>\r\n"
        "c: application/sdp\r\n"
        "l: 132\r\n"
        "s: Regression Test Subject\r\n"
        "e: gzip\r\n"
        "k: 100rel\r\n"
        "CSeq: 1 INVITE\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipCompactAll.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipCompactAll.end());

    // Verify getters and canonical header names in sipm["h"]
    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("compact-all-callid-101", sipm.getCallID());
    EXPECT_EQ("Alice <sip:alice@example.com>;tag=1928301774", sipm["h"]["From"].get<std::string>());
    EXPECT_EQ("Bob <sip:bob@example.com>", sipm["h"]["To"].get<std::string>());
    EXPECT_EQ("<sip:alice@pc33.example.com>", sipm["h"]["Contact"].get<std::string>());
    EXPECT_EQ("Regression Test Subject", sipm["h"]["Subject"].get<std::string>());
    EXPECT_EQ("gzip", sipm["h"]["Content-Encoding"].get<std::string>());

    // Via and Supported are multi-line headers, stored as arrays
    EXPECT_TRUE(sipm["h"]["Via"].is_array());
    EXPECT_EQ(1u, sipm["h"]["Via"].size());
    EXPECT_EQ("SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a", sipm["h"]["Via"][0].get<std::string>());

    EXPECT_TRUE(sipm["h"]["Supported"].is_array());
    EXPECT_EQ(1u, sipm["h"]["Supported"].size());
    EXPECT_EQ("100rel", sipm["h"]["Supported"][0].get<std::string>());

    // Verify SDP body was parsed correctly due to c: application/sdp
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));

    // Verify raw compact header keys were NOT created in sipm["h"]
    EXPECT_FALSE(sipm["h"].contains("v"));
    EXPECT_FALSE(sipm["h"].contains("f"));
    EXPECT_FALSE(sipm["h"].contains("t"));
    EXPECT_FALSE(sipm["h"].contains("i"));
    EXPECT_FALSE(sipm["h"].contains("m"));
    EXPECT_FALSE(sipm["h"].contains("c"));
    EXPECT_FALSE(sipm["h"].contains("l"));
    EXPECT_FALSE(sipm["h"].contains("s"));
    EXPECT_FALSE(sipm["h"].contains("e"));
    EXPECT_FALSE(sipm["h"].contains("k"));
}

TEST(Issue30_CompactHeaders, MixedCompactAndLongForm)
{
    using namespace siddiqsoft;

    std::string sipMixed =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP proxy1.example.com;branch=z9hG4bK1\r\n"
        "v: SIP/2.0/UDP proxy2.example.com;branch=z9hG4bK2\r\n"
        "From: Alice <sip:alice@example.com>;tag=123\r\n"
        "t: Bob <sip:bob@example.com>\r\n"
        "i: mixed-compact-callid-202\r\n"
        "CSeq: 1 INVITE\r\n"
        "c: application/sdp\r\n"
        "Content-Length: 132\r\n"
        "s: Test Mixed Form\r\n"
        "Supported: path\r\n"
        "k: 100rel\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipMixed.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipMixed.end());

    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("mixed-compact-callid-202", sipm.getCallID());
    EXPECT_EQ("Alice <sip:alice@example.com>;tag=123", sipm["h"]["From"].get<std::string>());
    EXPECT_EQ("Bob <sip:bob@example.com>", sipm["h"]["To"].get<std::string>());

    // Check Via array contains both long-form and compact-form entries in order
    EXPECT_TRUE(sipm["h"]["Via"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Via"].size());
    EXPECT_EQ("SIP/2.0/UDP proxy1.example.com;branch=z9hG4bK1", sipm["h"]["Via"][0].get<std::string>());
    EXPECT_EQ("SIP/2.0/UDP proxy2.example.com;branch=z9hG4bK2", sipm["h"]["Via"][1].get<std::string>());

    // Check Supported array contains both long-form and compact-form entries in order
    EXPECT_TRUE(sipm["h"]["Supported"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Supported"].size());
    EXPECT_EQ("path", sipm["h"]["Supported"][0].get<std::string>());
    EXPECT_EQ("100rel", sipm["h"]["Supported"][1].get<std::string>());

    EXPECT_FALSE(sipm["h"].contains("v"));
    EXPECT_FALSE(sipm["h"].contains("t"));
    EXPECT_FALSE(sipm["h"].contains("i"));
    EXPECT_FALSE(sipm["h"].contains("c"));
    EXPECT_FALSE(sipm["h"].contains("k"));
}

TEST(Issue30_CompactHeaders, MultipleCompactHeadersArray)
{
    using namespace siddiqsoft;

    std::string sipMultiCompact =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "v: SIP/2.0/UDP proxy1.example.com;branch=z9hG4bK1\r\n"
        "v: SIP/2.0/UDP proxy2.example.com;branch=z9hG4bK2\r\n"
        "f: Alice <sip:alice@example.com>;tag=123\r\n"
        "t: Bob <sip:bob@example.com>\r\n"
        "i: multi-compact-callid-303\r\n"
        "CSeq: 1 INVITE\r\n"
        "m: <sip:user1@10.0.0.1:5060>\r\n"
        "m: <sip:user2@10.0.0.2:5060>\r\n"
        "k: 100rel\r\n"
        "k: timer\r\n"
        "l: 0\r\n\r\n";

    auto start = sipMultiCompact.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipMultiCompact.end());

    EXPECT_EQ(0u, sipm.getContentLength());
    EXPECT_EQ("multi-compact-callid-303", sipm.getCallID());

    // Via
    EXPECT_TRUE(sipm["h"]["Via"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Via"].size());
    EXPECT_EQ("SIP/2.0/UDP proxy1.example.com;branch=z9hG4bK1", sipm["h"]["Via"][0].get<std::string>());
    EXPECT_EQ("SIP/2.0/UDP proxy2.example.com;branch=z9hG4bK2", sipm["h"]["Via"][1].get<std::string>());

    // Contact
    EXPECT_TRUE(sipm["h"]["Contact"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Contact"].size());
    EXPECT_EQ("<sip:user1@10.0.0.1:5060>", sipm["h"]["Contact"][0].get<std::string>());
    EXPECT_EQ("<sip:user2@10.0.0.2:5060>", sipm["h"]["Contact"][1].get<std::string>());

    // Supported
    EXPECT_TRUE(sipm["h"]["Supported"].is_array());
    EXPECT_EQ(2u, sipm["h"]["Supported"].size());
    EXPECT_EQ("100rel", sipm["h"]["Supported"][0].get<std::string>());
    EXPECT_EQ("timer", sipm["h"]["Supported"][1].get<std::string>());

    EXPECT_FALSE(sipm["h"].contains("v"));
    EXPECT_FALSE(sipm["h"].contains("m"));
    EXPECT_FALSE(sipm["h"].contains("k"));
}

TEST(Issue30_CompactHeaders, CompactHeaderSerialization)
{
    using namespace siddiqsoft;

    std::string sipCompact =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "v: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n"
        "f: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "t: Bob <sip:bob@example.com>\r\n"
        "i: compact-serial-404\r\n"
        "CSeq: 1 INVITE\r\n"
        "m: <sip:alice@pc33.example.com>\r\n"
        "c: application/sdp\r\n"
        "s: Test Subject\r\n"
        "e: gzip\r\n"
        "k: 100rel\r\n"
        "l: 0\r\n"
        "\r\n";

    auto start = sipCompact.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipCompact.end());

    std::string serialized = sip2json::serialize(sipm);

    // Serialization should format canonical header names instead of compact single-character keys
    EXPECT_TRUE(serialized.find("Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("From: Alice <sip:alice@example.com>;tag=1928301774\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("To: Bob <sip:bob@example.com>\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Call-ID: compact-serial-404\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Contact: <sip:alice@pc33.example.com>\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: application/sdp\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Subject: Test Subject\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Encoding: gzip\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Supported: 100rel\r\n") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Length: 0\r\n") != std::string::npos);

    EXPECT_FALSE(serialized.find("\r\nv: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\nf: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\nt: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\ni: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\nm: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\nc: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\ns: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\ne: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\nk: ") != std::string::npos);
    EXPECT_FALSE(serialized.find("\r\nl: ") != std::string::npos);
}



// ============================================================================
// GITHUB ISSUE #29 TESTS: RFC 3261 Case-Insensitive Header Matching & Normalization
// ============================================================================

TEST(Issue29_CaseInsensitiveHeaders, LowercaseContentLengthAndSDP)
{
    using namespace siddiqsoft;

    std::string sipLowercase =
        "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
        "via: SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
        "max-forwards: 70\r\n"
        "to: Bob <sip:bob@biloxi.com>\r\n"
        "from: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
        "call-id: lower-callid-101\r\n"
        "cseq: 314159 INVITE\r\n"
        "contact: <sip:alice@pc33.atlanta.com>\r\n"
        "content-type: application/sdp\r\n"
        "content-length: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipLowercase.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipLowercase.end());

    // Verify getter functions resolve correctly with lowercase headers
    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("lower-callid-101", sipm.getCallID());
    EXPECT_TRUE(sipm.contains("b"));
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
}

#if defined(sip2json_HEADERKEY_MODE_INSENSITIVE)
TEST(Issue29_CaseInsensitiveHeaders, MixedCaseHeaders)
{
    using namespace siddiqsoft;

    std::string sipMixedCase =
        "REGISTER sip:example.com SIP/2.0\r\n"
        "vIa: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK-reg\r\n"
        "tO: <sip:user@example.com>\r\n"
        "fRoM: <sip:user@example.com>;tag=reg123\r\n"
        "cAlL-iD: mixed-callid-202\r\n"
        "cSeq: 1 REGISTER\r\n"
        "eXpIrEs: 3600\r\n"
        "cOnTeNt-LeNgTh: 0\r\n\r\n";

    auto start = sipMixedCase.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipMixedCase.end());

    EXPECT_EQ(0u, sipm.getContentLength());
    EXPECT_EQ("mixed-callid-202", sipm.getCallID());
    EXPECT_EQ(3600u, sipm.getExpires());
}

TEST(Issue29_CaseInsensitiveHeaders, UppercaseHeaders)
{
    using namespace siddiqsoft;

    std::string sipUppercase =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "VIA: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n"
        "MAX-FORWARDS: 70\r\n"
        "TO: <sip:user@example.com>\r\n"
        "FROM: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "CALL-ID: upper-callid-303\r\n"
        "CSEQ: 1 INVITE\r\n"
        "CONTENT-TYPE: application/sdp\r\n"
        "CONTENT-LENGTH: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipUppercase.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipUppercase.end());

    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("upper-callid-303", sipm.getCallID());
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
}
#endif

TEST(Issue29_CaseInsensitiveHeaders, CompactHeaderNames)
{
    using namespace siddiqsoft;

    std::string sipCompact =
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "v: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776a\r\n"
        "t: <sip:user@example.com>\r\n"
        "f: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "i: compact-callid-404\r\n"
        "CSeq: 1 INVITE\r\n"
        "c: application/sdp\r\n"
        "l: 132\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 127.0.0.1\r\n"
        "s=Talk\r\n"
        "c=IN IP4 127.0.0.1\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";

    auto start = sipCompact.begin();
    auto sipm = sip2json::parseFromBuffer(start, sipCompact.end());

    EXPECT_EQ(132u, sipm.getContentLength());
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ("compact-callid-404", sipm.getCallID());
    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
}



