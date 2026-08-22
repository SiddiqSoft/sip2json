/*
  Stress Tests for sip2json
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
#include <numeric>

#include "nlohmann/json.hpp"
#include "siddiqsoft/sip2json.hpp"
#include "gtest/gtest.h"
#include <thread>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif


// Helper: build a well-formed SIP request with enough headers to exceed minimum buffer size
static std::string makeRequest(const std::string& method,
                               const std::string& callId,
                               uint32_t           cseq        = 1,
                               const std::string& contentType = {},
                               const std::string& body        = {})
{
    std::string msg = std::format("{} sip:user@example.com SIP/2.0\r\n"
                                  "Via: SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK{}\r\n"
                                  "To: sip:user@example.com\r\n"
                                  "From: sip:caller@example.com;tag=tag{}\r\n"
                                  "Call-ID: {}\r\n"
                                  "CSeq: {} {}\r\n"
                                  "Contact: sip:caller@10.0.0.1\r\n",
                                  method,
                                  callId,
                                  callId,
                                  callId,
                                  cseq,
                                  method);

    if (!contentType.empty())
    {
        msg += std::format("Content-Type: {}\r\n", contentType);
        msg += std::format("Content-Length: {}\r\n", body.size());
    }
    else
    {
        msg += "Content-Length: 0\r\n";
    }

    msg += "\r\n";
    msg += body;
    return msg;
}

static std::string
makeResponse(uint32_t statusCode, const std::string& callId, uint32_t cseq = 1, const std::string& method = "INVITE")
{
    return std::format("SIP/2.0 {} {}\r\n"
                       "Via: SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK{}\r\n"
                       "To: sip:user@example.com;tag=resp{}\r\n"
                       "From: sip:caller@example.com;tag=tag{}\r\n"
                       "Call-ID: {}\r\n"
                       "CSeq: {} {}\r\n"
                       "Contact: sip:user@10.0.0.2\r\n"
                       "Content-Length: 0\r\n"
                       "\r\n",
                       statusCode,
                       siddiqsoft::getReasonPhrase(statusCode),
                       callId,
                       callId,
                       callId,
                       callId,
                       cseq,
                       method);
}


// ============================================================================
// STRESS: RAPID PARSE OF MANY MESSAGES
// ============================================================================

TEST(stress, Test_parse_1000_requests)
{
    for (int i = 0; i < 1000; i++)
    {
        auto        callId = std::format("stress-req-{}", i);
        std::string buffer = makeRequest("INVITE", callId, i + 1);
        auto        bs     = buffer.begin();

        siddiqsoft::sipmessage sipm;
        ASSERT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end())) << "Failed at iteration " << i;
        ASSERT_EQ("INVITE", sipm.getMethod()) << "Method mismatch at iteration " << i;
        ASSERT_EQ(callId, sipm.getCallID()) << "CallID mismatch at iteration " << i;
    }
}

TEST(stress, Test_parse_1000_responses)
{
    std::vector<uint32_t> codes = {100, 180, 200, 302, 400, 401, 404, 486, 500, 503};

    for (int i = 0; i < 1000; i++)
    {
        auto        code   = codes[i % codes.size()];
        auto        callId = std::format("stress-resp-{}", i);
        std::string buffer = makeResponse(code, callId);
        auto        bs     = buffer.begin();

        siddiqsoft::sipmessage sipm;
        ASSERT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()))
                << "Failed at iteration " << i << " code=" << code;
        ASSERT_EQ(code, static_cast<uint32_t>(sipm.getStatusCode())) << "Status mismatch at iteration " << i;
    }
}

TEST(stress, Test_serialize_1000_messages)
{
    std::vector<std::string> methods = {
            "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER", "SUBSCRIBE", "NOTIFY", "MESSAGE", "INFO"};

    for (int i = 0; i < 1000; i++)
    {
        auto& method = methods[i % methods.size()];
        auto  callId = siddiqsoft::createCallId();

        siddiqsoft::sipmessage sipm(method, "sip:test@test.com", callId, i + 1);
        sipm.setHeader("To", "sip:test@test.com");
        sipm.setHeader("From", "sip:sender@sender.com");

        std::string serialized;
        ASSERT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm))
                << "Serialize failed at iteration " << i << " method=" << method;
        ASSERT_TRUE(serialized.find(method) != std::string::npos) << "Method not found in serialized output at iteration " << i;
    }
}


// ============================================================================
// STRESS: PARSE-SERIALIZE ROUND-TRIP
// ============================================================================

TEST(stress, Test_roundtrip_100_requests)
{
    for (int i = 0; i < 100; i++)
    {
        auto        callId = std::format("rt-{}", i);
        std::string buffer = makeRequest("REGISTER", callId, i + 1);
        auto        bs     = buffer.begin();

        // Parse
        siddiqsoft::sipmessage sipm;
        ASSERT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

        // Serialize
        std::string serialized;
        ASSERT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm));

        // Re-parse the serialized output
        auto                   bs2 = serialized.begin();
        siddiqsoft::sipmessage sipm2;
        ASSERT_NO_THROW(sipm2 = siddiqsoft::sip2json::parseFromBuffer(bs2, serialized.end()))
                << "Re-parse failed at iteration " << i;

        ASSERT_EQ(sipm.getMethod(), sipm2.getMethod());
        ASSERT_EQ(sipm.getCallID(), sipm2.getCallID());
        ASSERT_EQ(sipm.getUri(), sipm2.getUri());
    }
}

TEST(stress, Test_roundtrip_sdp_50_messages)
{
    std::string sdpTemplate {"v=0\r\n"
                             "o=user{0} {0} {0} IN IP4 10.0.0.{1}\r\n"
                             "s=Call {0}\r\n"
                             "c=IN IP4 10.0.0.{1}\r\n"
                             "t=0 0\r\n"
                             "m=audio {2} RTP/AVP 0\r\n"
                             "a=rtpmap:0 PCMU/8000\r\n"};

    for (int i = 0; i < 50; i++)
    {
        auto sdpBody = std::format("v=0\r\n"
                                   "o=user{0} {0} {0} IN IP4 10.0.0.{1}\r\n"
                                   "s=Call {0}\r\n"
                                   "c=IN IP4 10.0.0.{1}\r\n"
                                   "t=0 0\r\n"
                                   "m=audio {2} RTP/AVP 0\r\n"
                                   "a=rtpmap:0 PCMU/8000\r\n",
                                   i,
                                   (i % 254) + 1,
                                   49170 + i);

        auto callId = std::format("sdp-rt-{}", i);
        auto buffer = makeRequest("INVITE", callId, i + 1, "application/sdp", sdpBody);
        auto bs     = buffer.begin();

        // Parse
        siddiqsoft::sipmessage sipm;
        ASSERT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end())) << "Parse failed at iteration " << i;
        ASSERT_TRUE(sipm.contains("/b/sdp"_json_pointer)) << "SDP not found at iteration " << i;

        // Serialize
        std::string serialized;
        ASSERT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm)) << "Serialize failed at iteration " << i;
        ASSERT_TRUE(serialized.find("v=0") != std::string::npos);
        ASSERT_TRUE(serialized.find("a=rtpmap:0 PCMU/8000") != std::string::npos);
    }
}


// ============================================================================
// STRESS: parseAsync WITH CONCATENATED MESSAGES
// ============================================================================

TEST(stress, Test_parseAsync_20_concatenated_messages)
{
    std::string buffer;
    const int   msgCount = 20;

    for (int i = 0; i < msgCount; i++)
    {
        auto callId = std::format("concat-{}", i);
        if (i % 2 == 0)
            buffer += makeRequest("OPTIONS", callId, i + 1);
        else
            buffer += makeResponse(200, callId, i + 1, "OPTIONS");
    }

    int                      parseCount = 0;
    std::vector<std::string> callIds;

    auto remaining = siddiqsoft::sip2json::parseAsync(buffer,
                                                      [&](auto&& sipm)
                                                      {
                                                          parseCount++;
                                                          callIds.push_back(sipm.getCallID());
                                                      });

    EXPECT_EQ(msgCount, parseCount) << "Expected " << msgCount << " messages, got " << parseCount;
    EXPECT_EQ(0u, remaining.length()) << "Expected empty remaining buffer";

    // Verify all call-IDs are unique and match expected pattern
    std::set<std::string> uniqueIds(callIds.begin(), callIds.end());
    EXPECT_EQ(static_cast<size_t>(msgCount), uniqueIds.size());
}

TEST(stress, Test_parseAsync_mixed_methods_10)
{
    std::vector<std::string> methods = {
            "INVITE", "ACK", "BYE", "CANCEL", "OPTIONS", "REGISTER", "SUBSCRIBE", "NOTIFY", "MESSAGE", "INFO"};
    std::string buffer;

    for (int i = 0; i < 10; i++)
    {
        auto callId = std::format("mixed-{}", i);
        buffer += makeRequest(methods[i], callId, i + 1);
    }

    int                      parseCount = 0;
    std::vector<std::string> parsedMethods;

    auto _ = siddiqsoft::sip2json::parseAsync(buffer,
                                              [&](auto&& sipm)
                                              {
                                                  parseCount++;
                                                  parsedMethods.push_back(sipm.getMethod());
                                              });

    EXPECT_EQ(10, parseCount);
    for (int i = 0; i < 10; i++)
    {
        EXPECT_EQ(methods[i], parsedMethods[i]) << "Method mismatch at index " << i;
    }
}


// ============================================================================
// STRESS: LARGE HEADERS
// ============================================================================

TEST(stress, Test_parse_many_custom_headers)
{
    // Build a message with 50 custom headers
    std::string buffer {"OPTIONS sip:test@test.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\r\n"
                        "To: sip:test@test.com\r\n"
                        "From: sip:sender@sender.com;tag=1928301774\r\n"
                        "Call-ID: manyheaders@client.com\r\n"
                        "CSeq: 1 OPTIONS\r\n"
                        "Contact: sip:sender@client.com\r\n"};

    for (int i = 0; i < 50; i++)
    {
        buffer += std::format("X-Custom-Header-{}: value-{}\r\n", i, i);
    }
    buffer += "Content-Length: 0\r\n\r\n";

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_EQ("OPTIONS", sipm.getMethod());
    EXPECT_EQ("manyheaders@client.com", sipm.getCallID());

    // Verify all custom headers were parsed
    for (int i = 0; i < 50; i++)
    {
        auto key      = std::format("X-Custom-Header-{}", i);
        auto expected = std::format("value-{}", i);
        EXPECT_EQ(expected, sipm.getHeader<std::string>(key)) << "Header mismatch for " << key;
    }
}

TEST(stress, Test_parse_long_header_value)
{
    // Header value of 2000 characters
    std::string longValue(2000, 'A');

    std::string buffer = std::format("OPTIONS sip:test@test.com SIP/2.0\r\n"
                                     "Via: SIP/2.0/TCP client.com:5060;branch=z9hG4bK776asdhds\r\n"
                                     "To: sip:test@test.com\r\n"
                                     "From: sip:sender@sender.com;tag=1928301774\r\n"
                                     "Call-ID: longval@client.com\r\n"
                                     "CSeq: 1 OPTIONS\r\n"
                                     "Contact: sip:sender@client.com\r\n"
                                     "X-Long-Header: {}\r\n"
                                     "Content-Length: 0\r\n"
                                     "\r\n",
                                     longValue);

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    auto parsed = sipm.getHeader<std::string>("X-Long-Header");
    EXPECT_EQ(2000u, parsed.size());
    EXPECT_EQ(longValue, parsed);
}


// ============================================================================
// STRESS: LARGE SDP BODY
// ============================================================================

TEST(stress, Test_parse_sdp_with_many_attributes)
{
    // Build SDP with 30 attribute lines
    std::string sdpBody {"v=0\r\n"
                         "o=stress 1000 1000 IN IP4 10.0.0.1\r\n"
                         "s=Stress Test\r\n"
                         "c=IN IP4 10.0.0.1\r\n"
                         "t=0 0\r\n"
                         "m=audio 49170 RTP/AVP 0 8 96 97 98\r\n"
                         "a=rtpmap:0 PCMU/8000\r\n"
                         "a=rtpmap:8 PCMA/8000\r\n"
                         "a=rtpmap:96 opus/48000/2\r\n"
                         "a=rtpmap:97 iLBC/8000\r\n"
                         "a=rtpmap:98 telephone-event/8000\r\n"
                         "a=fmtp:96 minptime=10;useinbandfec=1\r\n"
                         "a=fmtp:98 0-16\r\n"
                         "a=ptime:20\r\n"
                         "a=maxptime:150\r\n"
                         "a=sendrecv\r\n"};

    auto cl     = sdpBody.size();
    auto callId = "sdp-stress-attrs";

    std::string buffer = std::format("INVITE sip:user@example.com SIP/2.0\r\n"
                                     "Via: SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                                     "To: sip:user@example.com\r\n"
                                     "From: sip:caller@example.com;tag=stress1\r\n"
                                     "Call-ID: {}\r\n"
                                     "CSeq: 1 INVITE\r\n"
                                     "Contact: sip:caller@10.0.0.1\r\n"
                                     "Content-Type: application/sdp\r\n"
                                     "Content-Length: {}\r\n"
                                     "\r\n"
                                     "{}",
                                     callId,
                                     cl,
                                     sdpBody);

    auto                   bs = buffer.begin();
    siddiqsoft::sipmessage sipm;
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    auto& sdp0 = sipm["b"]["sdp"][0];
    EXPECT_TRUE(sdp0.contains("a"));

    // rtpmap should be an array with 5 entries
    auto& rtpmap = sdp0["a"]["rtpmap"];
    EXPECT_TRUE(rtpmap.is_array());
    EXPECT_EQ(5u, rtpmap.size());

    // fmtp should be an array with 2 entries
    auto& fmtp = sdp0["a"]["fmtp"];
    EXPECT_TRUE(fmtp.is_array());
    EXPECT_EQ(2u, fmtp.size());

    // sendrecv should be a boolean true (flag attribute)
    EXPECT_TRUE(sdp0["a"]["sendrecv"].get<bool>());

    // Serialize and verify round-trip
    std::string serialized;
    EXPECT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm));
    EXPECT_TRUE(serialized.find("a=rtpmap:0 PCMU/8000") != std::string::npos);
    EXPECT_TRUE(serialized.find("a=sendrecv") != std::string::npos);
}


// ============================================================================
// STRESS: createCallId UNIQUENESS UNDER RAPID GENERATION
// ============================================================================

TEST(stress, Test_createCallId_1000_unique)
{
    std::set<std::string> ids;
    for (int i = 0; i < 1000; i++)
    {
        auto id = siddiqsoft::createCallId();
        ASSERT_EQ(44u, id.length()) << "Wrong length at iteration " << i;
        auto [it, inserted] = ids.insert(id);
        ASSERT_TRUE(inserted) << "Duplicate CallId at iteration " << i << ": " << id;
    }
    EXPECT_EQ(1000u, ids.size());
}


// ============================================================================
// STRESS: EMPTY AND BOUNDARY BUFFERS
// ============================================================================

TEST(stress, Test_parseFromBuffer_empty_string)
{
    std::string buffer;
    auto        bs = buffer.begin();

    siddiqsoft::sipmessage sipm;
    // Empty buffer: bufferStart == bufferEnd, skips parsing entirely, returns default sipmessage.
    // Default sipmessage constructor sets "meta" key, so it won't be json-empty,
    // but it should not be a request or response.
    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_FALSE(sipm.isMessageRequest());
    EXPECT_FALSE(sipm.isMessageResponse());
}

TEST(stress, Test_parseFromBuffer_exactly_minimal_length)
{
    // Buffer exactly equal to SIP_SAMPLE_MINIMAL_MESSAGE length should throw
    // (the check is strictly greater than)
    auto        minLen = siddiqsoft::SIP_SAMPLE_MINIMAL_MESSAGE.length();
    std::string buffer(minLen, 'A');

    auto bs = buffer.begin();
    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()), siddiqsoft::incomplete_buffer_for_parse_error);
}

TEST(stress, Test_parseAsync_empty_buffer)
{
    std::string buffer;
    int         parseCount = 0;

    auto remaining = siddiqsoft::sip2json::parseAsync(buffer, [&](auto&&) { parseCount++; });

    EXPECT_EQ(0, parseCount);
    EXPECT_EQ(0u, remaining.length());
}

TEST(stress, Test_parseAsync_partial_message_preserved)
{
    // First message is complete, second is truncated
    auto callId1 = "partial-test-1";
    auto msg1    = makeRequest("OPTIONS", callId1);

    // Truncated second message (just the start line, not enough for a full parse)
    std::string partial = "INVITE sip:bob@biloxi.com SIP/2.0\r\nVia: SIP";

    std::string buffer = msg1 + partial;

    int  parseCount = 0;
    auto _          = siddiqsoft::sip2json::parseAsync(buffer,
                                                       [&](auto&& sipm)
                                                       {
                                                  parseCount++;
                                                  EXPECT_EQ(callId1, sipm.getCallID());
                                                       });

    // Should have parsed exactly 1 message
    EXPECT_EQ(1, parseCount);
    // The remaining buffer should contain the partial second message
    // (parseAsync erases consumed content)
    // Note: the partial message is shorter than SIP_SAMPLE_MINIMAL_MESSAGE
    // so it will be left in the buffer
    std::println(std::cerr, "Remaining buffer: {}", buffer);
    EXPECT_EQ(partial, buffer);
}


// ============================================================================
// STRESS: SERIALIZE THEN PARSE CONSISTENCY
// ============================================================================

TEST(stress, Test_serialize_parse_response_codes)
{
    std::vector<uint32_t> codes = {100, 180, 200, 302, 400, 401, 403, 404, 408, 486, 500, 503, 603, 608};

    for (auto code : codes)
    {
        siddiqsoft::sipmessage sipm(code);
        sipm.setHeader("Call-ID", std::format("code-{}", code));
        sipm.setHeader("CSeq", "1 INVITE");
        sipm.setHeader("Via", "SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK776");
        sipm.setHeader("To", "sip:user@example.com");
        sipm.setHeader("From", "sip:caller@example.com;tag=abc");
        sipm.setHeader("Contact", "sip:user@10.0.0.2");

        std::string serialized;
        ASSERT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm)) << "Serialize failed for code " << code;

        auto                   bs2 = serialized.begin();
        siddiqsoft::sipmessage sipm2;
        ASSERT_NO_THROW(sipm2 = siddiqsoft::sip2json::parseFromBuffer(bs2, serialized.end()))
                << "Re-parse failed for code " << code;

        ASSERT_EQ(code, static_cast<uint32_t>(sipm2.getStatusCode())) << "Status code mismatch for " << code;
        ASSERT_EQ(siddiqsoft::getReasonPhrase(code), sipm2.getReason()) << "Reason mismatch for " << code;
    }
}
