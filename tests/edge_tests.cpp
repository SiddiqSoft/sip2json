/*
  Edge Case Tests for sip2json
  https://github.com/siddiqsoftware/sip2json/
*/

#include <string>
#include <chrono>
#include <iostream>
#include <string_view>
#include <set>
#include <vector>
#include <sstream>
#include <format>

#include "nlohmann/json.hpp"
#include "../include/siddiqsoft/sip2json.hpp"
#include "gtest/gtest.h"
#include <thread>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif


// ============================================================================
// SDP PARSING AND ROUND-TRIP EDGE CASES
// ============================================================================

static std::string makeSdpInvite()
{
    std::string sdpBody {
            "v=0\r\n"
            "o=alice 2890844526 2890844526 IN IP4 pc33.atlanta.com\r\n"
            "s=Session SDP\r\n"
            "i=\"Alice\" (1234) voice\r\n"
            "c=IN IP4 pc33.atlanta.com\r\n"
            "t=0 0\r\n"
            "m=audio 49170 RTP/AVP 0\r\n"
            "a=rtpmap:0 PCMU/8000\r\n"
            "a=sendrecv\r\n"};

    auto cl = sdpBody.size();

    return std::format(
            "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
            "To: sip:bob@biloxi.com\r\n"
            "From: sip:alice@atlanta.com;tag=1928301774\r\n"
            "Call-ID: sdptest@pc33.atlanta.com\r\n"
            "CSeq: 1 INVITE\r\n"
            "Contact: sip:alice@pc33.atlanta.com\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}",
            cl,
            sdpBody);
}

TEST(edge_sdp, Test_parse_sdp_body)
{
    auto buffer = makeSdpInvite();
    auto bs     = buffer.begin();

    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.isMessageRequest());
    EXPECT_EQ("INVITE", sipm.getMethod());
    EXPECT_EQ("application/sdp", sipm.getContentType());

    EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
    auto& sdp0 = sipm["b"]["sdp"][0];
    EXPECT_EQ(0, sdp0["v"].get<int>());
    EXPECT_TRUE(sdp0.contains("o"));
    EXPECT_TRUE(sdp0.contains("s"));
    EXPECT_TRUE(sdp0.contains("i"));
    EXPECT_TRUE(sdp0.contains("c"));
    EXPECT_TRUE(sdp0.contains("t"));
    EXPECT_TRUE(sdp0.contains("m"));
    EXPECT_TRUE(sdp0.contains("a"));

    // o-line parsed as object
    EXPECT_TRUE(sdp0["o"].is_object());
    EXPECT_EQ("alice", sdp0["o"]["user"].get<std::string>());
    EXPECT_EQ("pc33.atlanta.com", sdp0["o"]["host"].get<std::string>());

    // c-line parsed as object
    EXPECT_TRUE(sdp0["c"].is_object());
    EXPECT_EQ("IN", sdp0["c"]["type"].get<std::string>());
    EXPECT_EQ("IP4", sdp0["c"]["subtype"].get<std::string>());

    // i-line parsed as object
    EXPECT_TRUE(sdp0["i"].is_object());
    EXPECT_EQ("Alice", sdp0["i"]["name"].get<std::string>());
    EXPECT_EQ("1234", sdp0["i"]["dn"].get<std::string>());

    // t-line parsed as array [0, 0]
    EXPECT_TRUE(sdp0["t"].is_array());
    EXPECT_EQ(0u, sdp0["t"][0].get<uint32_t>());
    EXPECT_EQ(0u, sdp0["t"][1].get<uint32_t>());

    // a-line attributes
    EXPECT_TRUE(sdp0["a"].contains("rtpmap"));
    EXPECT_TRUE(sdp0["a"].contains("sendrecv"));
}

TEST(edge_sdp, Test_serialize_sdp_roundtrip)
{
    auto buffer = makeSdpInvite();
    auto bs     = buffer.begin();

    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    std::string serialized;
    EXPECT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm));

    EXPECT_TRUE(serialized.find("v=0") != std::string::npos);
    EXPECT_TRUE(serialized.find("o=alice") != std::string::npos);
    EXPECT_TRUE(serialized.find("s=Session SDP") != std::string::npos);
    EXPECT_TRUE(serialized.find("c=IN IP4 pc33.atlanta.com") != std::string::npos);
    EXPECT_TRUE(serialized.find("t=0 0") != std::string::npos);
    EXPECT_TRUE(serialized.find("m=audio 49170 RTP/AVP 0") != std::string::npos);
    EXPECT_TRUE(serialized.find("a=rtpmap:0 PCMU/8000") != std::string::npos);
    EXPECT_TRUE(serialized.find("a=sendrecv") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: application/sdp") != std::string::npos);
}

TEST(edge_sdp, Test_sdp_content_type_zero_content_length)
{
    std::string buffer {
            "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
            "To: sip:bob@biloxi.com\r\n"
            "From: sip:alice@atlanta.com;tag=1928301774\r\n"
            "Call-ID: sdpzero@pc33.atlanta.com\r\n"
            "CSeq: 1 INVITE\r\n"
            "Contact: sip:alice@pc33.atlanta.com\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: 0\r\n"
            "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ("application/sdp", sipm.getContentType());
    EXPECT_EQ(0u, sipm.getContentLength());
}

TEST(edge_sdp, Test_sdp_multiple_rtpmap_attributes)
{
    std::string sdpBody {
            "v=0\r\n"
            "o=bob 123 456 IN IP4 192.168.1.1\r\n"
            "s=-\r\n"
            "c=IN IP4 192.168.1.1\r\n"
            "t=0 0\r\n"
            "m=audio 49170 RTP/AVP 0 8\r\n"
            "a=rtpmap:0 PCMU/8000\r\n"
            "a=rtpmap:8 PCMA/8000\r\n"};

    auto cl = sdpBody.size();

    std::string buffer = std::format(
            "INVITE sip:alice@atlanta.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP 192.168.1.1:5060;branch=z9hG4bK776asdhds\r\n"
            "To: sip:alice@atlanta.com\r\n"
            "From: sip:bob@biloxi.com;tag=9876\r\n"
            "Call-ID: multirtpmap@biloxi.com\r\n"
            "CSeq: 1 INVITE\r\n"
            "Contact: sip:bob@192.168.1.1\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}",
            cl,
            sdpBody);

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    auto& rtpmap = sipm["b"]["sdp"][0]["a"]["rtpmap"];
    EXPECT_TRUE(rtpmap.is_array());
    EXPECT_EQ(2u, rtpmap.size());
}


// ============================================================================
// SERIALIZE ERROR EDGE CASES
// ============================================================================

TEST(edge_serialize_errors, Test_serialize_empty_message)
{
    siddiqsoft::sipmessage sipm;
    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::empty_message_error);
}

TEST(edge_serialize_errors, Test_serialize_unsupported_method)
{
    siddiqsoft::sipmessage sipm;
    sipm.update(nlohmann::json {
            {"s", {{"type", "request"}, {"method", "FOOBAR"}, {"uri", "sip:test@test.com"}, {"version", "SIP/2.0"}}},
            {"h", {{"Call-ID", "bad123"}, {"CSeq", "1 FOOBAR"}}},
            {"b", nullptr}});

    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);
}

TEST(edge_serialize_errors, Test_serialize_no_headers)
{
    siddiqsoft::sipmessage sipm;
    sipm.update(nlohmann::json {
            {"s", {{"type", "request"}, {"method", "OPTIONS"}, {"uri", "sip:test@test.com"}, {"version", "SIP/2.0"}}},
            {"b", nullptr}});

    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);
}

TEST(edge_serialize_errors, Test_serialize_invalid_type)
{
    siddiqsoft::sipmessage sipm;
    sipm.update(nlohmann::json {
            {"s", {{"type", "notspecified"}, {"method", "OPTIONS"}, {"uri", "sip:test@test.com"}, {"version", "SIP/2.0"}}},
            {"h", {{"Call-ID", "inv123"}}},
            {"b", nullptr}});

    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);
}


// ============================================================================
// PARSING EDGE CASES
// ============================================================================

TEST(edge_parsing, Test_multiple_via_headers)
{
    std::string buffer {
            "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP proxy1.atlanta.com;branch=z9hG4bK776asdhds\r\n"
            "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bK87asdks7\r\n"
            "To: sip:bob@biloxi.com\r\n"
            "From: sip:alice@atlanta.com;tag=1928301774\r\n"
            "Call-ID: multivia@atlanta.com\r\n"
            "CSeq: 1 INVITE\r\n"
            "Contact: sip:alice@pc33.atlanta.com\r\n"
            "Content-Length: 0\r\n"
            "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    auto& via = sipm["h"]["Via"];
    EXPECT_TRUE(via.is_array());
    EXPECT_EQ(2u, via.size());
    EXPECT_TRUE(via[0].get<std::string>().find("proxy1") != std::string::npos);
    EXPECT_TRUE(via[1].get<std::string>().find("pc33") != std::string::npos);
}

TEST(edge_parsing, Test_content_type_normalization)
{
    // "Content-type" (lowercase t) should be normalized to "Content-Type"
    // Use application/sdp with Content-Length: 0 to avoid unsupported_contenttype_error
    std::string buffer {
            "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
            "To: sip:bob@biloxi.com\r\n"
            "From: sip:alice@atlanta.com;tag=1928301774\r\n"
            "Call-ID: ctnorm@atlanta.com\r\n"
            "CSeq: 1 INVITE\r\n"
            "Contact: sip:alice@pc33.atlanta.com\r\n"
            "Content-type: application/sdp\r\n"
            "Content-Length: 0\r\n"
            "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    // Should be normalized to "Content-Type" (capital T)
    EXPECT_TRUE(sipm.headers().contains("Content-Type"));
    EXPECT_EQ("application/sdp", sipm.getContentType());
}

TEST(edge_parsing, Test_lf_only_line_endings)
{
    std::string buffer {
            "OPTIONS sip:test@test.com SIP/2.0\n"
            "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\n"
            "To: sip:test@test.com\n"
            "From: sip:sender@sender.com;tag=1928301774\n"
            "Call-ID: lfonly@client.com\n"
            "CSeq: 1 OPTIONS\n"
            "Contact: sip:sender@client.com\n"
            "Content-Length: 0\n"
            "\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.isMessageRequest());
    EXPECT_EQ("OPTIONS", sipm.getMethod());
    EXPECT_EQ("lfonly@client.com", sipm.getCallID());
}

TEST(edge_parsing, Test_incomplete_buffer_for_parse)
{
    std::string buffer {"SIP/2.0 200 OK\r\n"};

    auto bs = buffer.begin();
    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()),
                 siddiqsoft::incomplete_buffer_for_parse_error);
}

TEST(edge_parsing, Test_parseAsync_no_error_callback)
{
    std::string buffer {"too short"};

    int  parseCount = 0;
    auto remaining  = siddiqsoft::sip2json::parseAsync(
            buffer,
            [&](auto&&) { parseCount++; });

    EXPECT_EQ(0, parseCount);
}

TEST(edge_parsing, Test_parseAsync_invalid_startline_callback)
{
    std::string buffer(200, 'X');

    bool                       errorCaught = false;
    siddiqsoft::sip2jsonErrors caughtError = siddiqsoft::sip2jsonErrors::ok;

    siddiqsoft::sip2json::parseAsync(
            buffer,
            [](auto&&) {},
            [&](const siddiqsoft::sip2json_exception& e, std::string::iterator&, const std::string::iterator&)
            {
                errorCaught = true;
                caughtError = e.errCode;
            });

    EXPECT_TRUE(errorCaught);
    EXPECT_EQ(siddiqsoft::sip2jsonErrors::invalid_startline, caughtError);
}

TEST(edge_parsing, Test_parse_throws_when_nothing_parsed)
{
    std::string buffer(200, 'Z');

    auto bs = buffer.begin();
    EXPECT_THROW(siddiqsoft::sip2json::parse(bs, buffer.end()), std::invalid_argument);
}

TEST(edge_parsing, Test_serialize_via_array)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);
    sipm["h"]["Via"] = nlohmann::json::array(
            {"SIP/2.0/TCP proxy1.com;branch=z9hG4bK1", "SIP/2.0/TCP proxy2.com;branch=z9hG4bK2"});

    auto serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.find("Via: SIP/2.0/TCP proxy1.com") != std::string::npos);
    EXPECT_TRUE(serialized.find("Via: SIP/2.0/TCP proxy2.com") != std::string::npos);
}


// ============================================================================
// SIPMESSAGE MUTATOR EDGE CASES
// ============================================================================

TEST(edge_mutators, Test_setHeader_json_merge)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);

    nlohmann::json headers = {{"X-Custom-1", "value1"}, {"X-Custom-2", "value2"}};
    sipm.setHeader(headers);

    EXPECT_EQ("value1", sipm.getHeader<std::string>("X-Custom-1"));
    EXPECT_EQ("value2", sipm.getHeader<std::string>("X-Custom-2"));
}

TEST(edge_mutators, Test_setBody_json_pointer)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);

    sipm.setBody("/custom"_json_pointer, "custom_value");

    EXPECT_EQ("custom_value", sipm.getBodyElement<std::string>("/custom"_json_pointer, ""));
}

TEST(edge_mutators, Test_setBody_json_merge)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);

    // The request constructor sets "b" to nullptr.
    // setBody(json) checks if "b" exists: if so, it calls at("b").update(arg).
    // Since "b" is null (not an object), update() would throw.
    // setBody only works correctly when "b" doesn't exist yet, or is already an object.
    // First, remove the null body so setBody creates it fresh.
    sipm.erase("b");

    nlohmann::json bodyData = {{"key1", "val1"}, {"key2", 42}};
    sipm.setBody(bodyData);

    EXPECT_EQ("val1", sipm.getBodyElement<std::string>("/key1"_json_pointer, ""));
    EXPECT_EQ(42, sipm.getBodyElement<int>("/key2"_json_pointer, 0));
}

TEST(edge_mutators, Test_hasBody)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    EXPECT_TRUE(sipm.hasBody());

    siddiqsoft::sipmessage sipm2;
    EXPECT_FALSE(sipm2.hasBody());
}

TEST(edge_mutators, Test_getContentLength)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader("Content-Length", 42);
    EXPECT_EQ(42u, sipm.getContentLength());
}

TEST(edge_mutators, Test_getContentType_empty)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    EXPECT_EQ("", sipm.getContentType());
}

TEST(edge_mutators, Test_setUserAgent_custom)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com");
    sipm.setUserAgent("MyApp/1.0");

    auto ua = sipm.getUserAgent();
    EXPECT_TRUE(ua.find("sip2json") != std::string::npos);
    EXPECT_TRUE(ua.find("MyApp/1.0") != std::string::npos);
}

TEST(edge_mutators, Test_serialize_integer_header)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader("Max-Forwards", 70);

    auto serialized = siddiqsoft::sip2json::serialize(sipm);
    EXPECT_TRUE(serialized.find("Max-Forwards: 70") != std::string::npos);
}


// ============================================================================
// EXCEPTION HIERARCHY EDGE CASES
// ============================================================================

TEST(edge_exceptions, Test_all_exception_types)
{
    {
        siddiqsoft::incomplete_buffer_for_parse_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse, e.errCode);
        EXPECT_EQ(std::string("test"), std::string(e.what()));
    }
    {
        siddiqsoft::incomplete_buffer_for_content_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content, e.errCode);
    }
    {
        siddiqsoft::incomplete_buffer_for_header_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header, e.errCode);
    }
    {
        siddiqsoft::invalid_startline_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::invalid_startline, e.errCode);
    }
    {
        siddiqsoft::unsupported_contenttype_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::unsupported_contenttype, e.errCode);
    }
    {
        siddiqsoft::invalid_document_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::invalid_document, e.errCode);
    }
    {
        siddiqsoft::empty_message_error e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::empty_message, e.errCode);
    }
    {
        siddiqsoft::missing_required_element e("test");
        EXPECT_EQ(siddiqsoft::sip2jsonErrors::missing_required_element, e.errCode);
    }
}
