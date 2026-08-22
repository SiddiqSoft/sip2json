/*
  Additional Coverage Tests for sip2json
  https://github.com/siddiqsoftware/sip2json/
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


// ============================================================================
// PARSING COVERAGE TESTS
// ============================================================================

TEST(coverage_parsing, Test_parseFromBuffer_response)
{
    std::string buffer {"SIP/2.0 200 OK\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
                        "CSeq: 314159 INVITE\r\n"
                        "Contact: sip:bob@192.0.2.4\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.isMessageResponse());
    EXPECT_FALSE(sipm.isMessageRequest());
    EXPECT_EQ(200, sipm.getStatusCode());
    EXPECT_EQ("OK", sipm.getReason());
    EXPECT_EQ("a84b4c76e66710@pc33.atlanta.com", sipm.getCallID());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_100)
{
    std::string buffer {"SIP/2.0 100 Trying\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test100@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(100, sipm.getStatusCode());
    EXPECT_EQ("Trying", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_180)
{
    std::string buffer {"SIP/2.0 180 Ringing\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test180@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(180, sipm.getStatusCode());
    EXPECT_EQ("Ringing", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_302)
{
    std::string buffer {"SIP/2.0 302 Moved Temporarily\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test302@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(302, sipm.getStatusCode());
    EXPECT_EQ("Moved Temporarily", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_400)
{
    std::string buffer {"SIP/2.0 400 Bad Request\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test400@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(400, sipm.getStatusCode());
    EXPECT_EQ("Bad Request", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_401)
{
    std::string buffer {"SIP/2.0 401 Unauthorized\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test401@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(401, sipm.getStatusCode());
    EXPECT_EQ("Unauthorized", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_404)
{
    std::string buffer {"SIP/2.0 404 Not Found\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test404@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(404, sipm.getStatusCode());
    EXPECT_EQ("Not Found", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_500)
{
    std::string buffer {"SIP/2.0 500 Internal Server Error\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test500@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(500, sipm.getStatusCode());
    EXPECT_EQ("Internal Server Error", sipm.getReason());
}


TEST(coverage_parsing, Test_parseFromBuffer_response_503)
{
    std::string buffer {"SIP/2.0 503 Service Unavailable\r\n"
                        "Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: test503@pc33.atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_EQ(503, sipm.getStatusCode());
    EXPECT_EQ("Service Unavailable", sipm.getReason());
}


TEST(coverage_parsing, Test_parse_vector_single_message)
{
    std::string buffer {"REGISTER sip:registrar.biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP bobspc.biloxi.com:5060\r\n"
                        "To: sip:bob@biloxi.com\r\n"
                        "From: sip:bob@biloxi.com;tag=456248\r\n"
                        "Call-ID: 843817637684230@998sdasdh09\r\n"
                        "CSeq: 1826 REGISTER\r\n"
                        "Contact: sip:bob@192.0.2.4\r\n"
                        "Expires: 7200\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto bs   = buffer.begin();
    auto msgs = siddiqsoft::sip2json::parse(bs, buffer.end());

    EXPECT_EQ(1, msgs.size());
    EXPECT_TRUE(msgs[0].isMessageRequest());
    EXPECT_EQ("REGISTER", msgs[0].getMethod());
    EXPECT_EQ(7200, msgs[0].getExpires());
}


TEST(coverage_parsing, Test_parse_vector_multiple_messages)
{
    std::string buffer {"REGISTER sip:server.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@server.com\r\n"
                        "From: sip:bob@server.com;tag=456248\r\n"
                        "Call-ID: msg1@client.com\r\n"
                        "CSeq: 1 REGISTER\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "SIP/2.0 200 OK\r\n"
                        "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@server.com;tag=a6c85cf\r\n"
                        "From: sip:bob@server.com;tag=456248\r\n"
                        "Call-ID: msg1@client.com\r\n"
                        "CSeq: 1 REGISTER\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto bs   = buffer.begin();
    auto msgs = siddiqsoft::sip2json::parse(bs, buffer.end());

    EXPECT_GE(msgs.size(), 1u);
    EXPECT_TRUE(msgs[0].isMessageRequest());
    if (msgs.size() > 1) { EXPECT_TRUE(msgs[1].isMessageResponse()); }
}


TEST(coverage_parsing, Test_parseAsync_multiple_messages)
{
    std::string buffer {"INVITE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: invite1@atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Contact: sip:alice@pc33.atlanta.com\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "ACK sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                        "Call-ID: invite1@atlanta.com\r\n"
                        "CSeq: 1 ACK\r\n"
                        "Contact: sip:alice@pc33.atlanta.com\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    int                      parseCount = 0;
    std::vector<std::string> methods;

    auto remaining = siddiqsoft::sip2json::parseAsync(buffer,
                                                      [&](auto&& sipm)
                                                      {
                                                          parseCount++;
                                                          methods.push_back(sipm.getMethod());
                                                      });

    EXPECT_EQ(2, parseCount);
    EXPECT_EQ(0, remaining.length());
    EXPECT_EQ("INVITE", methods[0]);
    EXPECT_EQ("ACK", methods[1]);
}


TEST(coverage_parsing, Test_header_normalization_uthorization)
{
    std::string buffer {"REGISTER sip:registrar.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP client.com:5060\r\n"
                        "Call-ID: auth123\r\n"
                        "CSeq: 1 REGISTER\r\n"
                        "uthorization: Digest username=\"alice\"\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.headers().contains("Authorization"));
    EXPECT_EQ("Digest username=\"alice\"", sipm.getHeader<std::string>("Authorization"));
}


TEST(coverage_parsing, Test_header_empty_value)
{
    std::string buffer {"OPTIONS sip:test@test.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP client.com:5060\r\n"
                        "Call-ID: empty123\r\n"
                        "CSeq: 1 OPTIONS\r\n"
                        "X-Empty-Header: \r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.headers().contains("X-Empty-Header"));
    EXPECT_EQ("", sipm.getHeader<std::string>("X-Empty-Header"));
}


TEST(coverage_parsing, Test_folded_headers)
{
    std::string buffer {"INVITE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com\r\n"
                        "Subject: This is a very long subject line\r\n"
                        " that continues on the next line\r\n"
                        " and even continues further\r\n"
                        "Call-ID: folded123\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    auto subject = sipm.getHeader<std::string>("Subject");
    EXPECT_TRUE(subject.find("very long subject line") != std::string::npos);
    EXPECT_TRUE(subject.find("continues on the next line") != std::string::npos);
    EXPECT_TRUE(subject.find("continues further") != std::string::npos);
}


TEST(coverage_parsing, Test_folded_headers_tab)
{
    std::string buffer {"INVITE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com\r\n"
                        "Subject: Line one\r\n"
                        "\tLine two with tab\r\n"
                        "Call-ID: foldedtab123\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"};

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    auto subject = sipm.getHeader<std::string>("Subject");
    EXPECT_TRUE(subject.find("Line one") != std::string::npos);
    EXPECT_TRUE(subject.find("Line two with tab") != std::string::npos);
}


TEST(coverage_parsing, Test_all_supported_methods)
{
    std::vector<std::string> methods = {
            "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER", "SUBSCRIBE", "NOTIFY", "MESSAGE", "INFO", "REFER", "PUBLISH", "UPDATE", "PRACK"};

    for (const auto& method : methods)
    {
        std::string buffer = std::format("{} sip:test@test.com SIP/2.0\r\n"
                                         "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\r\n"
                                         "To: sip:test@test.com\r\n"
                                         "From: sip:sender@sender.com;tag=1928301774\r\n"
                                         "Call-ID: method-{}@client.com\r\n"
                                         "CSeq: 1 {}\r\n"
                                         "Contact: sip:sender@client.com\r\n"
                                         "Content-Length: 0\r\n"
                                         "\r\n",
                                         method,
                                         method,
                                         method);

        auto                   bs = buffer.begin();
        siddiqsoft::sipmessage sipm;

        EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end())) << "Failed for method: " << method;
        EXPECT_EQ(method, sipm.getMethod()) << "Method mismatch for: " << method;

        std::string serialized;
        EXPECT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm)) << "Serialization failed for method: " << method;
        EXPECT_TRUE(serialized.find(method) != std::string::npos) << "Serialized output missing method: " << method;
    }
}


TEST(coverage_parsing, Test_custom_method_tokens_rejected)
{
    std::vector<std::string> customTokens = {
            "CUSTOMMETHOD", "FOOBAR", "BENCHMARK", "HEARTBEAT", "GET", "POST", "UNKNOWN"};

    for (const auto& customMethod : customTokens)
    {
        std::string buffer = std::format("{} sip:test@test.com SIP/2.0\r\n"
                                         "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\r\n"
                                         "To: sip:test@test.com\r\n"
                                         "From: sip:sender@sender.com;tag=1928301774\r\n"
                                         "Call-ID: custom-{}@client.com\r\n"
                                         "CSeq: 1 {}\r\n"
                                         "Content-Length: 0\r\n"
                                         "\r\n",
                                         customMethod,
                                         customMethod,
                                         customMethod);

        auto bs = buffer.begin();
        EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()), siddiqsoft::invalid_startline_error)
                << "Expected custom method to be rejected in parsing: " << customMethod;
    }
}


TEST(coverage_serialization, Test_custom_method_tokens_serialize_rejected)
{
    std::vector<std::string> customTokens = {
            "CUSTOMMETHOD", "FOOBAR", "BENCHMARK", "HEARTBEAT", "GET", "POST", "UNKNOWN"};

    for (const auto& customMethod : customTokens)
    {
        siddiqsoft::sipmessage sipm(customMethod, "sip:user@example.com", "call-id-custom", 1);
        EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error)
                << "Expected custom method to be rejected in serialization: " << customMethod;
    }
}


TEST(coverage_sipmessage, Test_string_view_accessors)
{
    siddiqsoft::sipmessage req("INVITE", "sip:alice@example.com", "callid-view-100", 1);
    EXPECT_EQ("INVITE", req.getMethodView());
    EXPECT_EQ("sip:alice@example.com", req.getUriView());
    EXPECT_EQ("callid-view-100", req.getCallIDView());

    siddiqsoft::sipmessage resp(200);
    EXPECT_EQ("OK", resp.getReasonView());
}


// ============================================================================
// ERROR COVERAGE TESTS
// ============================================================================

TEST(coverage_errors, Test_incomplete_buffer_for_content)
{
    std::string buffer {"INVITE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com\r\n"
                        "Call-ID: invite1@atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Type: application/sdp\r\n"
                        "Content-Length: 1000\r\n"
                        "\r\n"
                        "v=0\r\n"
                        "s=short\r\n"};

    auto bs = buffer.begin();

    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()), siddiqsoft::incomplete_buffer_for_content_error);
}


TEST(coverage_errors, Test_unsupported_contenttype)
{
    std::string buffer {"MESSAGE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com\r\n"
                        "Call-ID: msg1@atlanta.com\r\n"
                        "CSeq: 1 MESSAGE\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: 15\r\n"
                        "\r\n"
                        "{\"test\":\"data\"}"};

    auto bs = buffer.begin();

    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()), siddiqsoft::unsupported_contenttype_error);
}


TEST(coverage_errors, Test_parseAsync_unsupported_contenttype_callback)
{
    std::string buffer {"MESSAGE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com\r\n"
                        "Call-ID: msg1@atlanta.com\r\n"
                        "CSeq: 1 MESSAGE\r\n"
                        "Content-Type: application/xml\r\n"
                        "Content-Length: 10\r\n"
                        "\r\n"
                        "<root></root>"};

    bool                       errorCaught = false;
    siddiqsoft::sip2jsonErrors caughtError = siddiqsoft::sip2jsonErrors::ok;

    auto _= siddiqsoft::sip2json::parseAsync(
            buffer,
            [](auto&&) { },
            [&](const siddiqsoft::sip2json_exception& e, std::string::iterator&, const std::string::iterator&)
            {
                errorCaught = true;
                caughtError = e.errCode;
            });

    EXPECT_TRUE(errorCaught);
    EXPECT_EQ(siddiqsoft::sip2jsonErrors::unsupported_contenttype, caughtError);
}


TEST(coverage_errors, Test_parseAsync_incomplete_content_callback)
{
    std::string buffer {"INVITE sip:bob@biloxi.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP pc33.atlanta.com\r\n"
                        "Call-ID: invite1@atlanta.com\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Type: application/sdp\r\n"
                        "Content-Length: 5000\r\n"
                        "\r\n"
                        "v=0\r\n"};

    bool                       errorCaught = false;
    siddiqsoft::sip2jsonErrors caughtError = siddiqsoft::sip2jsonErrors::ok;

    auto _ = siddiqsoft::sip2json::parseAsync(
            buffer,
            [](auto&&) { },
            [&](const siddiqsoft::sip2json_exception& e, std::string::iterator&, const std::string::iterator&)
            {
                errorCaught = true;
                caughtError = e.errCode;
            });

    EXPECT_TRUE(errorCaught);
    EXPECT_EQ(siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content, caughtError);
}


TEST(coverage_errors, Test_missing_required_sdp_element)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader(siddiqsoft::HF_CONTENT_TYPE, siddiqsoft::CONTENT_TYPE_APP_SDP);

    sipm["b"]["sdp"][0]["x"] = "incomplete";

    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::missing_required_element);
}


TEST(coverage_errors, Test_sip2json_exception_from_std_exception)
{
    std::runtime_error             stdErr("Standard error message");
    siddiqsoft::sip2json_exception sipErr(stdErr);

    EXPECT_EQ(std::string("Standard error message"), std::string(sipErr.what()));
    EXPECT_EQ(siddiqsoft::sip2jsonErrors::unknown, sipErr.errCode);
}


// ============================================================================
// SIPMESSAGE COVERAGE TESTS
// ============================================================================


TEST(coverage_sipmessage, Test_move_constructor)
{
    auto                   callId = siddiqsoft::createCallId();
    siddiqsoft::sipmessage original("INVITE", "sip:bob@biloxi.com", callId, 1);

    siddiqsoft::sipmessage moved(std::move(original));

    EXPECT_EQ("INVITE", moved.getMethod());
    EXPECT_EQ("sip:bob@biloxi.com", moved.getUri());
    EXPECT_EQ(callId, moved.getCallID());
}


TEST(coverage_sipmessage, Test_construct_from_json)
{
    nlohmann::json j = {{"s", {{"type", "request"}, {"method", "OPTIONS"}, {"uri", "sip:test@test.com"}, {"version", "SIP/2.0"}}},
                        {"h", {{"Call-ID", "test123"}, {"CSeq", "1 OPTIONS"}}},
                        {"b", nullptr},
                        {"meta", {{"version", "test"}}}};

    siddiqsoft::sipmessage sipm(j);

    EXPECT_EQ("OPTIONS", sipm.getMethod());
    EXPECT_EQ("sip:test@test.com", sipm.getUri());
    EXPECT_EQ("test123", sipm.getCallID());
}


TEST(coverage_sipmessage, Test_move_assign_from_json)
{
    nlohmann::json j = {{"s", {{"type", "response"}, {"status", 404}, {"reason", "Not Found"}, {"version", "SIP/2.0"}}},
                        {"h", {{"Call-ID", "notfound123"}}},
                        {"b", nullptr}};

    siddiqsoft::sipmessage sipm;
    sipm = std::move(j);

    EXPECT_TRUE(sipm.isMessageResponse());
    EXPECT_EQ(404, sipm.getStatusCode());
    EXPECT_EQ("Not Found", sipm.getReason());
}


TEST(coverage_sipmessage, Test_json_conversion_operator)
{
    siddiqsoft::sipmessage sipm("BYE", "sip:bob@biloxi.com", "callid123", 2);

    nlohmann::json& jref = static_cast<nlohmann::json&>(sipm);

    EXPECT_EQ("BYE", jref["s"]["method"].get<std::string>());
    EXPECT_EQ("callid123", jref["h"]["Call-ID"].get<std::string>());
}


TEST(coverage_sipmessage, Test_setUserAgent_empty)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com");

    sipm.setUserAgent("");

    auto ua = sipm.getUserAgent();
    EXPECT_TRUE(ua.find("sip2json") != std::string::npos);
    EXPECT_TRUE(ua.find("schema:") != std::string::npos);
}


TEST(coverage_sipmessage, Test_accessors)
{
    siddiqsoft::sipmessage request("CANCEL", "sip:alice@atlanta.com", "cancel123", 5);
    EXPECT_EQ("CANCEL", request.getMethod());
    EXPECT_EQ("sip:alice@atlanta.com", request.getUri());
    EXPECT_EQ("", request.getReason());

    siddiqsoft::sipmessage response(486);
    EXPECT_EQ("", response.getMethod());
    EXPECT_EQ("", response.getUri());
    EXPECT_EQ("Busy Here", response.getReason());
    EXPECT_EQ(486, response.getStatusCode());
}


TEST(coverage_sipmessage, Test_getExpires)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:registrar.com", "reg123", 1);
    sipm.setHeader(siddiqsoft::HF_EXPIRES, 3600);

    EXPECT_EQ(3600, sipm.getExpires());
}


TEST(coverage_sipmessage, Test_flatten)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", "flatten123", 1);
    sipm.setHeader(siddiqsoft::HF_TO, "sip:bob@biloxi.com");

    auto flattened = sipm.flatten();

    EXPECT_TRUE(flattened.contains("/s/method"));
    EXPECT_TRUE(flattened.contains("/h/Call-ID"));
    EXPECT_EQ("INVITE", flattened["/s/method"].get<std::string>());
}


TEST(coverage_sipmessage, Test_response_from_request)
{
    siddiqsoft::sipmessage request("INVITE", "sip:bob@biloxi.com", "invite-callid-123", 1);
    request.setHeader(siddiqsoft::HF_TO, "sip:bob@biloxi.com");
    request.setHeader(siddiqsoft::HF_FROM, "sip:alice@atlanta.com");
    request.setHeader(siddiqsoft::HF_VIA, "SIP/2.0/TCP pc33.atlanta.com");

    siddiqsoft::sipmessage response(180, request);

    EXPECT_TRUE(response.isMessageResponse());
    EXPECT_EQ(180, response.getStatusCode());
    EXPECT_EQ("Ringing", response.getReason());
    EXPECT_EQ("invite-callid-123", response.getCallID());
}


// ============================================================================
// SERIALIZE COVERAGE TESTS
// ============================================================================

TEST(coverage_serialize, Test_serialize_text_plain_body)
{
    siddiqsoft::sipmessage sipm("MESSAGE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader(siddiqsoft::HF_TO, "sip:bob@biloxi.com");
    sipm.setHeader(siddiqsoft::HF_FROM, "sip:alice@atlanta.com");
    sipm.setHeader(siddiqsoft::HF_CONTENT_TYPE, siddiqsoft::CONTENT_TYPE_TEXT_PLAIN);

    sipm.body() = "Hello, World!";

    auto serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.find("MESSAGE sip:bob@biloxi.com SIP/2.0") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: text/plain") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Length: 13") != std::string::npos);
    EXPECT_TRUE(serialized.find("Hello, World!") != std::string::npos);
}


TEST(coverage_serialize, Test_serialize_boolean_header)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader("X-Custom-Bool", true);
    sipm.setHeader("X-Custom-Bool-False", false);

    auto serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.find("X-Custom-Bool: true") != std::string::npos);
    EXPECT_TRUE(serialized.find("X-Custom-Bool-False: false") != std::string::npos);
}


TEST(coverage_serialize, Test_serialize_null_header)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    sipm["h"]["X-Null-Header"] = nullptr;

    auto serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.find("X-Null-Header: \r\n") != std::string::npos);
}


TEST(coverage_serialize, Test_serialize_float_header)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader("X-Float-Value", 3.14f);

    auto serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.find("X-Float-Value:") != std::string::npos);
}


TEST(coverage_serialize, Test_serialize_response)
{
    siddiqsoft::sipmessage sipm(200);
    sipm.setHeader(siddiqsoft::HF_TO, "sip:bob@biloxi.com");
    sipm.setHeader(siddiqsoft::HF_FROM, "sip:alice@atlanta.com");
    sipm.setHeader(siddiqsoft::HF_CALLID, "response123");
    sipm.setHeader(siddiqsoft::HF_CSEQ, "1 INVITE");

    auto serialized = siddiqsoft::sip2json::serialize(sipm);

    EXPECT_TRUE(serialized.find("SIP/2.0 200 OK") != std::string::npos);
}


TEST(coverage_serialize, Test_serialize_all_methods)
{
    std::vector<std::string> methods = {
            "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER", "SUBSCRIBE", "NOTIFY", "MESSAGE", "INFO"};

    for (const auto& method : methods)
    {
        siddiqsoft::sipmessage sipm(method, "sip:test@test.com", siddiqsoft::createCallId(), 1);
        sipm.setHeader(siddiqsoft::HF_TO, "sip:test@test.com");
        sipm.setHeader(siddiqsoft::HF_FROM, "sip:sender@sender.com");

        std::string serialized;
        EXPECT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm)) << "Failed for method: " << method;
        EXPECT_TRUE(serialized.find(method + " sip:test@test.com SIP/2.0") != std::string::npos)
                << "Serialization mismatch for: " << method;
    }
}


// ============================================================================
// RESPONSE CODES COVERAGE TESTS
// ============================================================================

TEST(coverage_response_codes, Test_getReasonPhrase)
{
    EXPECT_EQ("Trying", siddiqsoft::getReasonPhrase(100));
    EXPECT_EQ("Ringing", siddiqsoft::getReasonPhrase(180));
    EXPECT_EQ("OK", siddiqsoft::getReasonPhrase(200));
    EXPECT_EQ("Moved Temporarily", siddiqsoft::getReasonPhrase(302));
    EXPECT_EQ("Bad Request", siddiqsoft::getReasonPhrase(400));
    EXPECT_EQ("Unauthorized", siddiqsoft::getReasonPhrase(401));
    EXPECT_EQ("Forbidden", siddiqsoft::getReasonPhrase(403));
    EXPECT_EQ("Not Found", siddiqsoft::getReasonPhrase(404));
    EXPECT_EQ("Request Timeout", siddiqsoft::getReasonPhrase(408));
    EXPECT_EQ("Busy Here", siddiqsoft::getReasonPhrase(486));
    EXPECT_EQ("Internal Server Error", siddiqsoft::getReasonPhrase(500));
    EXPECT_EQ("Service Unavailable", siddiqsoft::getReasonPhrase(503));
    EXPECT_EQ("Decline", siddiqsoft::getReasonPhrase(603));
    EXPECT_EQ("Rejected", siddiqsoft::getReasonPhrase(608));
}


// ============================================================================
// UTILS COVERAGE TESTS
// ============================================================================

TEST(coverage_utils, Test_createCallId_uniqueness)
{
    std::set<std::string> callIds;
    const int             numIds = 100;

    for (int i = 0; i < numIds; i++)
    {
        auto id = siddiqsoft::createCallId();
        EXPECT_EQ(44, id.length());
        callIds.insert(id);
    }

    EXPECT_EQ(static_cast<size_t>(numIds), callIds.size());
}


TEST(coverage_utils, Test_TimeAsRFC1123_no_args)
{
    auto timeStr = siddiqsoft::TimeAsRFC1123();

    EXPECT_FALSE(timeStr.empty());
    EXPECT_TRUE(timeStr.find("GMT") != std::string::npos);
}


TEST(coverage_utils, Test_TimeAsRFC3339_no_args)
{
    auto timeStr = siddiqsoft::TimeAsRFC3339();

    EXPECT_FALSE(timeStr.empty());
    EXPECT_TRUE(timeStr.find("T") != std::string::npos);
    EXPECT_TRUE(timeStr.find("Z") != std::string::npos);
}


TEST(coverage_utils, Test_InvokeOnDestruct)
{
    long long capturedDelta = -1;

    {
        siddiqsoft::InvokeOnDestruct timer([&](long long delta) { capturedDelta = delta; });

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        EXPECT_GE(timer.ttx(), 0);
    }

    EXPECT_GE(capturedDelta, 0);
}


// ============================================================================
// FORMATTERS COVERAGE TESTS
// ============================================================================

TEST(coverage_formatters, Test_sipmessage_formatter)
{
    siddiqsoft::sipmessage sipm("OPTIONS", "sip:test@test.com", "fmt123", 1);

    auto formatted = std::format("{}", sipm);

    EXPECT_TRUE(formatted.find("OPTIONS") != std::string::npos);
    EXPECT_TRUE(formatted.find("fmt123") != std::string::npos);
}


TEST(coverage_formatters, Test_sip2jsonErrors_formatter)
{
    EXPECT_EQ("ok", std::format("{}", siddiqsoft::sip2jsonErrors::ok));
    EXPECT_EQ("incomplete_buffer_for_parse", std::format("{}", siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse));
    EXPECT_EQ("invalid_startline", std::format("{}", siddiqsoft::sip2jsonErrors::invalid_startline));
    EXPECT_EQ("empty_message", std::format("{}", siddiqsoft::sip2jsonErrors::empty_message));
    EXPECT_EQ("unknown", std::format("{}", siddiqsoft::sip2jsonErrors::unknown));
}


TEST(coverage_formatters, Test_SIPMessageType_ostream)
{
    std::stringstream ss;
    ss << siddiqsoft::SIPMessageType::request;
    EXPECT_EQ("request", ss.str());

    ss.str("");
    ss << siddiqsoft::SIPMessageType::response;
    EXPECT_EQ("response", ss.str());

    ss.str("");
    ss << siddiqsoft::SIPMessageType::notspecified;
    EXPECT_EQ("unknown", ss.str());
}


TEST(coverage_formatters, Test_sip2jsonErrors_ostream)
{
    std::stringstream ss;

    ss << siddiqsoft::sip2jsonErrors::ok;
    EXPECT_EQ("ok", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse;
    EXPECT_EQ("incomplete_buffer_for_parse", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content;
    EXPECT_EQ("incomplete_buffer_for_content", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header;
    EXPECT_EQ("incomplete_buffer_for_header", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::invalid_startline;
    EXPECT_EQ("invalid_startline", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::unsupported_contenttype;
    EXPECT_EQ("unsupported_contenttype", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::missing_required_element;
    EXPECT_EQ("missing_required_element", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::invalid_document;
    EXPECT_EQ("invalid_document", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::invalid_document_unsupported_method;
    EXPECT_EQ("invalid_document_unsupported_method", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::invalid_document_unsupported_content;
    EXPECT_EQ("invalid_document_unsupported_content", ss.str());

    ss.str("");
    ss << siddiqsoft::sip2jsonErrors::empty_message;
    EXPECT_EQ("empty_message", ss.str());

    ss.str("");
    ss << static_cast<siddiqsoft::sip2jsonErrors>(9999);
    EXPECT_EQ("unknown", ss.str());
}


// ============================================================================
// JSON COVERAGE TESTS
// ============================================================================

TEST(coverage_json, Test_SIPMessageType_json_serialization)
{
    nlohmann::json j;

    j["type"] = siddiqsoft::SIPMessageType::request;
    EXPECT_EQ("request", j["type"].get<std::string>());

    j["type"] = siddiqsoft::SIPMessageType::response;
    EXPECT_EQ("response", j["type"].get<std::string>());

    j["type"] = siddiqsoft::SIPMessageType::notspecified;
    EXPECT_EQ("notspecified", j["type"].get<std::string>());
}


TEST(coverage_json, Test_sip2jsonErrors_json_serialization)
{
    nlohmann::json j;

    j["err"] = siddiqsoft::sip2jsonErrors::ok;
    EXPECT_EQ("ok", j["err"].get<std::string>());

    j["err"] = siddiqsoft::sip2jsonErrors::empty_message;
    EXPECT_EQ("empty_message", j["err"].get<std::string>());

    j["err"] = siddiqsoft::sip2jsonErrors::invalid_document;
    EXPECT_EQ("invalid_document", j["err"].get<std::string>());
}


//
