/*
    sip2json Benchmarks
    Measures performance of parsing, serialization, and utility functions.
*/

#include <iostream>
#include <string>
#include <vector>
#include <format>
#include <fstream>
#include <chrono>
#include <ctime>
#include <limits>
#include <thread>
#include <future>
#include <atomic>
#include <numeric>

#include "nlohmann/json.hpp"
#include "siddiqsoft/sip2json.hpp"

#include <benchmark/benchmark.h>


// ============================================================================
// Test data: realistic SIP messages of varying complexity
// ============================================================================

// Minimal SIP response (headers only, no body)
static const std::string SIP_RESPONSE_MINIMAL = "SIP/2.0 200 OK\r\n"
                                                "Via: SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
                                                "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
                                                "From: sip:alice@atlanta.com;tag=1928301774\r\n"
                                                "Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
                                                "CSeq: 314159 INVITE\r\n"
                                                "Contact: sip:bob@192.0.2.4\r\n"
                                                "Content-Length: 0\r\n"
                                                "\r\n";

// SIP REGISTER request (headers only)
static const std::string SIP_REGISTER_REQUEST = "REGISTER sip:registrar.biloxi.com SIP/2.0\r\n"
                                                "Via: SIP/2.0/TCP bobspc.biloxi.com:5060;branch=z9hG4bKnashds7\r\n"
                                                "Max-Forwards: 70\r\n"
                                                "To: sip:bob@biloxi.com\r\n"
                                                "From: sip:bob@biloxi.com;tag=456248\r\n"
                                                "Call-ID: 843817637684230@998sdasdh09\r\n"
                                                "CSeq: 1826 REGISTER\r\n"
                                                "Contact: sip:bob@192.0.2.4\r\n"
                                                "Expires: 7200\r\n"
                                                "Content-Length: 0\r\n"
                                                "\r\n";

// SIP INVITE with SDP body
static const std::string SIP_INVITE_WITH_SDP = "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
                                               "Via: SIP/2.0/TCP pc33.atlanta.com;branch=z9hG4bKnashds8\r\n"
                                               "Via: SIP/2.0/TCP proxy.atlanta.com;branch=z9hG4bK77ef4c2312983.1\r\n"
                                               "Max-Forwards: 70\r\n"
                                               "To: Bob <sip:bob@biloxi.com>\r\n"
                                               "From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
                                               "Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
                                               "CSeq: 314159 INVITE\r\n"
                                               "Contact: sip:alice@pc33.atlanta.com\r\n"
                                               "Content-Type: application/sdp\r\n"
                                               "Content-Length: 142\r\n"
                                               "\r\n"
                                               "v=0\r\n"
                                               "o=alice 2890844526 2890844526 IN IP4 pc33.atlanta.com\r\n"
                                               "s=Session SDP\r\n"
                                               "c=IN IP4 pc33.atlanta.com\r\n"
                                               "t=0 0\r\n"
                                               "m=audio 49170 RTP/AVP 0\r\n"
                                               "a=rtpmap:0 PCMU/8000\r\n";

// SIP INVITE with complex SDP (multiple a= lines, i= line)
static const std::string SIP_INVITE_COMPLEX_SDP = "INVITE sip:conference@server.example.com SIP/2.0\r\n"
                                                  "Via: SIP/2.0/TCP client.example.com:5060;branch=z9hG4bK74bf9\r\n"
                                                  "Via: SIP/2.0/TCP proxy.example.com:5060;branch=z9hG4bK2d4790.1\r\n"
                                                  "Via: SIP/2.0/TCP edge.example.com:5060;branch=z9hG4bK3f5e12.2\r\n"
                                                  "Max-Forwards: 68\r\n"
                                                  "To: Conference <sip:conference@server.example.com>\r\n"
                                                  "From: User <sip:user@client.example.com>;tag=9fxced76sl\r\n"
                                                  "Call-ID: 3848276298220188511@client.example.com\r\n"
                                                  "CSeq: 1 INVITE\r\n"
                                                  "Contact: sip:user@client.example.com\r\n"
                                                  "User-Agent: TestClient/1.0\r\n"
                                                  "Accept: application/sdp\r\n"
                                                  "Content-Type: application/sdp\r\n"
                                                  "Content-Length: 266\r\n"
                                                  "\r\n"
                                                  "v=0\r\n"
                                                  "o=user 53655765 2353687637 IN IP4 client.example.com\r\n"
                                                  "s=Conference Call\r\n"
                                                  "i=\"John Doe\" (5551234567) CallByPhone-URL\r\n"
                                                  "c=IN IP4 client.example.com\r\n"
                                                  "t=0 0\r\n"
                                                  "m=audio 49170 RTP/AVP 0 8 97\r\n"
                                                  "a=rtpmap:0 PCMU/8000\r\n"
                                                  "a=rtpmap:8 PCMA/8000\r\n"
                                                  "a=rtpmap:97 iLBC/8000\r\n"
                                                  "a=sendrecv\r\n"
                                                  "a=ptime:20\r\n";

// SIP NOTIFY with multiple Via headers (LF line endings)
static const std::string SIP_NOTIFY_LF = "NOTIFY sip:user@example.com SIP/2.0\n"
                                         "Via: SIP/2.0/TCP proxy1.example.com:5060;branch=z9hG4bK1234\n"
                                         "Via: SIP/2.0/TCP proxy2.example.com:5060;branch=z9hG4bK5678\n"
                                         "Via: SIP/2.0/TCP proxy3.example.com:5060;branch=z9hG4bK9abc\n"
                                         "To: sip:user@example.com;tag=xyz123\n"
                                         "From: sip:server@example.com;tag=abc456\n"
                                         "Call-ID: notify-test-12345@example.com\n"
                                         "CSeq: 42 NOTIFY\n"
                                         "Subscription-State: active\n"
                                         "Content-Length: 0\n"
                                         "\n";

// Multiple concatenated messages (for multi-message parse testing)
static std::string createMultiMessageBuffer(int count)
{
    std::string buffer;
    buffer.reserve(count * SIP_REGISTER_REQUEST.size());
    for (int i = 0; i < count; ++i)
    {
        buffer += SIP_REGISTER_REQUEST;
    }
    return buffer;
}

// Helper function to create large SIP message with extended SDP
static std::string createLargeSIPMessageWithExtendedSDP()
{
    std::string msg = "INVITE sip:conference@server.example.com SIP/2.0\r\n"
                      "Via: SIP/2.0/TCP client.example.com:5060;branch=z9hG4bK74bf9\r\n"
                      "Via: SIP/2.0/TCP proxy1.example.com:5060;branch=z9hG4bK2d4790.1\r\n"
                      "Via: SIP/2.0/TCP proxy2.example.com:5060;branch=z9hG4bK3f5e12.2\r\n"
                      "Via: SIP/2.0/TCP proxy3.example.com:5060;branch=z9hG4bK4g6f23.3\r\n"
                      "Max-Forwards: 68\r\n"
                      "To: Conference <sip:conference@server.example.com>\r\n"
                      "From: User <sip:user@client.example.com>;tag=9fxced76sl\r\n"
                      "Call-ID: 3848276298220188511@client.example.com\r\n"
                      "CSeq: 1 INVITE\r\n"
                      "Contact: sip:user@client.example.com\r\n"
                      "User-Agent: TestClient/1.0\r\n"
                      "Accept: application/sdp\r\n"
                      "Content-Type: application/sdp\r\n";

    // Build extended SDP with multiple media streams
    std::string sdp = "v=0\r\n"
                      "o=user 53655765 2353687637 IN IP4 client.example.com\r\n"
                      "s=Conference Call with Multiple Streams\r\n"
                      "i=\"John Doe\" (5551234567) CallByPhone-URL\r\n"
                      "c=IN IP4 client.example.com\r\n"
                      "t=0 0\r\n";

    // Add multiple audio streams
    for (int i = 0; i < 5; ++i)
    {
        sdp += std::format("m=audio {} RTP/AVP 0 8 97\r\n", 49170 + i * 2);
        sdp += "a=rtpmap:0 PCMU/8000\r\n";
        sdp += "a=rtpmap:8 PCMA/8000\r\n";
        sdp += "a=rtpmap:97 iLBC/8000\r\n";
        sdp += "a=sendrecv\r\n";
        sdp += "a=ptime:20\r\n";
    }

    // Add video stream
    sdp += "m=video 49180 RTP/AVP 96\r\n"
           "a=rtpmap:96 H264/90000\r\n"
           "a=fmtp:96 profile-level-id=42e01e\r\n"
           "a=sendrecv\r\n";

    msg += std::format("Content-Length: {}\r\n\r\n", sdp.size());
    msg += sdp;

    return msg;
}

// Generate a worst-case noisy buffer containing valid SIP messages interleaved with random noise/garbage headers/lines
static std::string createWorstCaseNoisyBuffer(int validMessageCount, int noiseBytesPerMessage = 256)
{
    static const std::string junkLines[] = {
            "NOISE_GARBAGE_HEADER_1234567890: random-junk-data-here-1234567890\r\n",
            "X-CORRUPTED-JUNK-KEY-ABCDEF: 999999999999999999999999\r\n",
            "random junk data without any key value separator\r\n",
            "\r\n\r\n\r\n",
            "GARBAGE_LINE_BEFORE_STARTLINE 123456789\r\n",
            "SIP/2.0 INVALID STATUS LINE WITHOUT REASON\r\n",
            "MALFORMED_HEADER_TEST: \r\n"};

    std::string buffer;
    buffer.reserve(validMessageCount * (SIP_INVITE_WITH_SDP.size() + noiseBytesPerMessage));

    for (int i = 0; i < validMessageCount; ++i)
    {
        // Inject leading junk before valid start-line (forces parser to skip ahead)
        buffer += junkLines[i % 7];
        buffer += junkLines[(i + 3) % 7];

        // Append valid message (either INVITE with SDP, REGISTER, or response)
        if (i % 3 == 0)
            buffer += SIP_INVITE_WITH_SDP;
        else if (i % 3 == 1)
            buffer += SIP_REGISTER_REQUEST;
        else
            buffer += SIP_RESPONSE_MINIMAL;

        // Inject trailing noise after message body
        buffer += junkLines[(i + 5) % 7];
    }

    return buffer;
}


// ============================================================================
// PARSING BENCHMARKS WITH JSON OUTPUT VALIDATION
// ============================================================================

// Helper function to validate parsed JSON by accessing Call-ID and counting items in SDP payload
static inline std::pair<std::string, size_t> validateParsedSipMessage(const siddiqsoft::sipmessage& sipm)
{
    using namespace std::string_literals;
    std::string callId {};
    size_t sdpItemCount = 0;

    // 1. Access Call-ID header from headers section
    callId = sipm.getCallID();

    // 2. Count items in SDP payload if present
    if (sipm.contains("b"s) && !sipm.body().is_null() && sipm.contains("/b/sdp"_json_pointer))
    {
        const auto& sdpArray = sipm.at("/b/sdp"_json_pointer);
        if (sdpArray.is_array())
        {
            for (const auto& block : sdpArray)
            {
                if (block.is_object())
                {
                    sdpItemCount += block.size();
                    if (block.contains("a") && block["a"].is_object())
                    {
                        sdpItemCount += block["a"].size();
                    }
                }
            }
        }
    }

    return {callId, sdpItemCount};
}

// Parse a minimal SIP response (no body)
static void BM_ParseMinimalResponse(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string buffer = SIP_RESPONSE_MINIMAL;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SIP_RESPONSE_MINIMAL.size());
}
BENCHMARK(BM_ParseMinimalResponse);

// Parse a REGISTER request (no body)
static void BM_ParseRegisterRequest(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string buffer = SIP_REGISTER_REQUEST;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SIP_REGISTER_REQUEST.size());
}
BENCHMARK(BM_ParseRegisterRequest);

// Parse an INVITE with SDP body
static void BM_ParseInviteWithSDP(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string buffer = SIP_INVITE_WITH_SDP;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SIP_INVITE_WITH_SDP.size());
}
BENCHMARK(BM_ParseInviteWithSDP);

// Parse an INVITE with complex SDP (multiple a= lines, i= line)
static void BM_ParseInviteComplexSDP(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string buffer = SIP_INVITE_COMPLEX_SDP;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SIP_INVITE_COMPLEX_SDP.size());
}
BENCHMARK(BM_ParseInviteComplexSDP);

// Parse a NOTIFY with LF line endings
static void BM_ParseNotifyLF(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string buffer = SIP_NOTIFY_LF;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SIP_NOTIFY_LF.size());
}
BENCHMARK(BM_ParseNotifyLF);


// ============================================================================
// MULTI-MESSAGE PARSING BENCHMARKS
// ============================================================================

// Parse multiple concatenated messages using parse()
static void BM_ParseMultipleMessages(benchmark::State& state)
{
    const int   msgCount = static_cast<int>(state.range(0));
    std::string buffer   = createMultiMessageBuffer(msgCount);

    for (auto _ : state)
    {
        std::string copy = buffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (const auto& sipm : msgs)
        {
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        }
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_ParseMultipleMessages)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

// Parse multiple messages using parseAsync()
static void BM_ParseAsyncMultipleMessages(benchmark::State& state)
{
    const int   msgCount = static_cast<int>(state.range(0));
    std::string buffer   = createMultiMessageBuffer(msgCount);

    for (auto _ : state)
    {
        std::string copy      = buffer;
        int         count     = 0;
        auto        remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            count++;
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        });
        benchmark::DoNotOptimize(remaining);
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_ParseAsyncMultipleMessages)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


// ============================================================================
// SERIALIZATION BENCHMARKS
// ============================================================================

// Serialize a simple REGISTER request
static void BM_SerializeRegister(benchmark::State& state)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader("To", "sip:hello@world.com").setHeader("Contact", "sip:hello@world.com").setHeader("Content-Length", 0);

    for (auto _ : state)
    {
        auto result = siddiqsoft::sip2json::serialize(sipm);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SerializeRegister);

// Serialize an INVITE with SDP body
static void BM_SerializeInviteWithSDP(benchmark::State& state)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_INVITE, "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);
    sipm.setHeader("To", "Bob <sip:bob@biloxi.com>")
            .setHeader("Contact", "sip:alice@pc33.atlanta.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP);

    sipm.setBody("/sdp/0/v"_json_pointer, 0)
            .setBody("/sdp/0/o"_json_pointer,
                     nlohmann::json {{"user", "alice"},
                                     {"type", "IN"},
                                     {"subtype", "IP4"},
                                     {"host", "pc33.atlanta.com"},
                                     {"t1", "2890844526"},
                                     {"t2", "2890844526"}})
            .setBody("/sdp/0/s"_json_pointer, "Session SDP")
            .setBody("/sdp/0/c"_json_pointer, nlohmann::json {{"type", "IN"}, {"subtype", "IP4"}, {"dn", "pc33.atlanta.com"}})
            .setBody("/sdp/0/t"_json_pointer, nlohmann::json {0, 0})
            .setBody("/sdp/0/m"_json_pointer, "audio 49170 RTP/AVP 0")
            .setBody("/sdp/0/a/rtpmap"_json_pointer, "0 PCMU/8000");

    for (auto _ : state)
    {
        auto result = siddiqsoft::sip2json::serialize(sipm);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SerializeInviteWithSDP);


// ============================================================================
// ROUND-TRIP BENCHMARKS (parse then serialize)
// ============================================================================

// Round-trip: parse then serialize a REGISTER
static void BM_RoundTripRegister(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Parse
        std::string buffer = SIP_REGISTER_REQUEST;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        // Serialize
        auto result = siddiqsoft::sip2json::serialize(sipm);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RoundTripRegister);

// Round-trip: parse then serialize an INVITE with SDP
static void BM_RoundTripInviteWithSDP(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Parse
        std::string buffer = SIP_INVITE_WITH_SDP;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        // Serialize
        auto result = siddiqsoft::sip2json::serialize(sipm);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RoundTripInviteWithSDP);


// ============================================================================
// SIPMESSAGE CONSTRUCTION BENCHMARKS
// ============================================================================

// Construct a default sipmessage
static void BM_ConstructDefaultSipmessage(benchmark::State& state)
{
    for (auto _ : state)
    {
        siddiqsoft::sipmessage sipm;
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ConstructDefaultSipmessage);

// Construct a request sipmessage
static void BM_ConstructRequestSipmessage(benchmark::State& state)
{
    auto callId = siddiqsoft::createCallId();
    for (auto _ : state)
    {
        siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com", callId, 1);
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ConstructRequestSipmessage);

// Construct a response sipmessage
static void BM_ConstructResponseSipmessage(benchmark::State& state)
{
    for (auto _ : state)
    {
        siddiqsoft::sipmessage sipm(200);
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ConstructResponseSipmessage);

// Construct a response from a request
static void BM_ConstructResponseFromRequest(benchmark::State& state)
{
    siddiqsoft::sipmessage req("INVITE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);
    req.setHeader("To", "sip:bob@biloxi.com").setHeader("Contact", "sip:bob@biloxi.com");

    for (auto _ : state)
    {
        siddiqsoft::sipmessage resp(200, req);
        benchmark::DoNotOptimize(resp);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ConstructResponseFromRequest);


// ============================================================================
// UTILITY FUNCTION BENCHMARKS
// ============================================================================

// TimeAsRFC1123
static void BM_TimeAsRFC1123(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = siddiqsoft::TimeAsRFC1123();
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_TimeAsRFC1123);

// TimeAsRFC3339
static void BM_TimeAsRFC3339(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = siddiqsoft::TimeAsRFC3339();
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_TimeAsRFC3339);

// TimeAsISO8601
static void BM_TimeAsISO8601(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = siddiqsoft::TimeAsISO8601();
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_TimeAsISO8601);

// createCallId
static void BM_CreateCallId(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = siddiqsoft::createCallId();
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_CreateCallId);


// ============================================================================
// ACCESSOR BENCHMARKS
// ============================================================================

// getContentType on a parsed message
static void BM_GetContentType(benchmark::State& state)
{
    std::string            buffer = SIP_INVITE_WITH_SDP;
    auto                   bs     = buffer.begin();
    siddiqsoft::sipmessage sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());

    for (auto _ : state)
    {
        auto ct = sipm.getContentType();
        benchmark::DoNotOptimize(ct);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetContentType);

// getMethod on a parsed message
static void BM_GetMethod(benchmark::State& state)
{
    std::string            buffer = SIP_INVITE_WITH_SDP;
    auto                   bs     = buffer.begin();
    siddiqsoft::sipmessage sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());

    for (auto _ : state)
    {
        auto m = sipm.getMethod();
        benchmark::DoNotOptimize(m);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetMethod);

// getHeader using library static constant (Optimal Path)
static void BM_GetHeader_LibraryConstant(benchmark::State& state)
{
    std::string            buffer = SIP_INVITE_WITH_SDP;
    auto                   bs     = buffer.begin();
    siddiqsoft::sipmessage sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());

    for (auto _ : state)
    {
        auto v = sipm.getHeader<std::string>(siddiqsoft::HF_CALLID);
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetHeader_LibraryConstant);

// getHeader using raw string literal (Ad-hoc Path)
static void BM_GetHeader_StringLiteral(benchmark::State& state)
{
    std::string            buffer = SIP_INVITE_WITH_SDP;
    auto                   bs     = buffer.begin();
    siddiqsoft::sipmessage sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());

    for (auto _ : state)
    {
        auto v = sipm.getHeader<std::string>("Call-ID");
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetHeader_StringLiteral);

// getHeader for custom un-canonicalized header
static void BM_GetHeader_CustomHeader(benchmark::State& state)
{
    std::string            buffer = SIP_INVITE_WITH_SDP;
    auto                   bs     = buffer.begin();
    siddiqsoft::sipmessage sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
    sipm.setHeader("X-Custom-Header", "custom-val");

    for (auto _ : state)
    {
        auto v = sipm.getHeader<std::string>("X-Custom-Header");
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetHeader_CustomHeader);

// setHeader using library static constant (Optimal Path)
static void BM_SetHeader_LibraryConstant(benchmark::State& state)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    for (auto _ : state)
    {
        sipm.setHeader(siddiqsoft::HF_CALLID, "call-12345");
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SetHeader_LibraryConstant);

// setHeader using raw string literal (Ad-hoc Path)
static void BM_SetHeader_StringLiteral(benchmark::State& state)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    for (auto _ : state)
    {
        sipm.setHeader("Call-ID", "call-12345");
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SetHeader_StringLiteral);

// setHeader using custom header name
static void BM_SetHeader_CustomHeader(benchmark::State& state)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    for (auto _ : state)
    {
        sipm.setHeader("X-Custom-Header", "benchmark-value");
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SetHeader_CustomHeader);

// Construct request using library constants (Optimal Path)
static void BM_ConstructRequest_LibraryConstant(benchmark::State& state)
{
    auto callId = siddiqsoft::createCallId();
    for (auto _ : state)
    {
        siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_INVITE, "sip:bob@biloxi.com", callId, 1);
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ConstructRequest_LibraryConstant);

// Construct request using raw string literal (Ad-hoc Path)
static void BM_ConstructRequest_StringLiteral(benchmark::State& state)
{
    auto callId = siddiqsoft::createCallId();
    for (auto _ : state)
    {
        siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", callId, 1);
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ConstructRequest_StringLiteral);


// ============================================================================
// STRESS/HIGH-FREQUENCY DECODE BENCHMARKS FOR LARGE PACKETS
// ============================================================================

// High-frequency decode: parse large SIP message with extended SDP repeatedly
static void BM_HighFrequencyDecodeLargePacket(benchmark::State& state)
{
    const std::string largeMsg = createLargeSIPMessageWithExtendedSDP();

    for (auto _ : state)
    {
        std::string buffer = largeMsg;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * largeMsg.size());
}
BENCHMARK(BM_HighFrequencyDecodeLargePacket);

// Stress test: parse 100 large packets in sequence
static void BM_StressTest100LargePackets(benchmark::State& state)
{
    const std::string largeMsg = createLargeSIPMessageWithExtendedSDP();
    std::string       buffer;
    buffer.reserve(100 * largeMsg.size());
    for (int i = 0; i < 100; ++i)
    {
        buffer += largeMsg;
    }

    for (auto _ : state)
    {
        std::string copy = buffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (const auto& sipm : msgs)
        {
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_StressTest100LargePackets);

// Stress test: async parse 100 large packets
static void BM_StressTestAsyncParse100LargePackets(benchmark::State& state)
{
    const std::string largeMsg = createLargeSIPMessageWithExtendedSDP();
    std::string       buffer;
    buffer.reserve(100 * largeMsg.size());
    for (int i = 0; i < 100; ++i)
    {
        buffer += largeMsg;
    }

    for (auto _ : state)
    {
        std::string copy      = buffer;
        int         count     = 0;
        auto        remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            count++;
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        });
        benchmark::DoNotOptimize(remaining);
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * 100);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_StressTestAsyncParse100LargePackets);

// Stress test: parse 1000 large packets (extreme stress)
static void BM_StressTest1000LargePackets(benchmark::State& state)
{
    const std::string largeMsg = createLargeSIPMessageWithExtendedSDP();
    std::string       buffer;
    buffer.reserve(1000 * largeMsg.size());
    for (int i = 0; i < 1000; ++i)
    {
        buffer += largeMsg;
    }

    for (auto _ : state)
    {
        std::string copy = buffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (const auto& sipm : msgs)
        {
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        }
    }
    state.SetItemsProcessed(state.iterations() * 1000);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_StressTest1000LargePackets);

// Stress test: async parse 1000 large packets (extreme stress)
static void BM_StressTestAsyncParse1000LargePackets(benchmark::State& state)
{
    const std::string largeMsg = createLargeSIPMessageWithExtendedSDP();
    std::string       buffer;
    buffer.reserve(1000 * largeMsg.size());
    for (int i = 0; i < 1000; ++i)
    {
        buffer += largeMsg;
    }

    for (auto _ : state)
    {
        std::string copy      = buffer;
        int         count     = 0;
        auto        remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            count++;
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        });
        benchmark::DoNotOptimize(remaining);
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * 1000);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_StressTestAsyncParse1000LargePackets);

// Variable-size stress test: parse N large packets (parameterized)
static void BM_VariableSizeStressTest(benchmark::State& state)
{
    const int         packetCount = static_cast<int>(state.range(0));
    const std::string largeMsg    = createLargeSIPMessageWithExtendedSDP();
    std::string       buffer;
    buffer.reserve(packetCount * largeMsg.size());
    for (int i = 0; i < packetCount; ++i)
    {
        buffer += largeMsg;
    }

    for (auto _ : state)
    {
        std::string copy = buffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (const auto& sipm : msgs)
        {
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        }
    }
    state.SetItemsProcessed(state.iterations() * packetCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_VariableSizeStressTest)->Arg(10)->Arg(50)->Arg(100)->Arg(500);

// High-frequency async decode with variable packet counts
static void BM_VariableSizeAsyncStressTest(benchmark::State& state)
{
    const int         packetCount = static_cast<int>(state.range(0));
    const std::string largeMsg    = createLargeSIPMessageWithExtendedSDP();
    std::string       buffer;
    buffer.reserve(packetCount * largeMsg.size());
    for (int i = 0; i < packetCount; ++i)
    {
        buffer += largeMsg;
    }

    for (auto _ : state)
    {
        std::string copy      = buffer;
        int         count     = 0;
        auto        remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            count++;
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        });
        benchmark::DoNotOptimize(remaining);
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * packetCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_VariableSizeAsyncStressTest)->Arg(10)->Arg(50)->Arg(100)->Arg(500);


// ============================================================================
// WORST-CASE NOISY BUFFER PARSING BENCHMARKS (Pipelined Noise & Garbage Skipping)
// ============================================================================

// Worst-case benchmark: parse a buffer containing valid messages interleaved with garbage lines
static void BM_WorstCaseNoisyBufferParsing(benchmark::State& state)
{
    const int         msgCount = static_cast<int>(state.range(0));
    const std::string buffer   = createWorstCaseNoisyBuffer(msgCount);

    for (auto _ : state)
    {
        std::string copy = buffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (const auto& sipm : msgs)
        {
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        }
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_WorstCaseNoisyBufferParsing)->Arg(10)->Arg(100)->Arg(500)->Arg(1000);

// Worst-case async benchmark: parseAsync over a noisy buffer skipping noise & invoking callback
static void BM_WorstCaseNoisyAsyncParsing(benchmark::State& state)
{
    const int         msgCount = static_cast<int>(state.range(0));
    const std::string buffer   = createWorstCaseNoisyBuffer(msgCount);

    for (auto _ : state)
    {
        std::string copy  = buffer;
        int         count = 0;
        siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            count++;
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        });
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_WorstCaseNoisyAsyncParsing)->Arg(10)->Arg(100)->Arg(500)->Arg(1000);


// ============================================================================
// MULTI-THREADED ASYNC CALLBACK BENCHMARKS (Parallel Stream Decoding)
// ============================================================================

// Multi-threaded benchmark: Parallel stream parsing across N worker threads using parseAsync
static void BM_MultiThreadedAsyncParsing(benchmark::State& state)
{
    const int numThreads = static_cast<int>(state.range(0));
    const int msgCountPerThread = 500;
    const std::string threadBuffer = createMultiMessageBuffer(msgCountPerThread);

    for (auto _ : state)
    {
        std::atomic<size_t> totalParsedCount{0};
        std::vector<std::future<size_t>> futures;
        futures.reserve(numThreads);

        for (int t = 0; t < numThreads; ++t)
        {
            futures.push_back(std::async(std::launch::async, [threadBuffer]() {
                std::string copy = threadBuffer;
                size_t parsedCount = 0;
                siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
                    parsedCount++;
                    auto [callId, sdpCount] = validateParsedSipMessage(sipm);
                    benchmark::DoNotOptimize(callId);
                    benchmark::DoNotOptimize(sdpCount);
                });
                return parsedCount;
            }));
        }

        for (auto& f : futures)
        {
            totalParsedCount += f.get();
        }
        benchmark::DoNotOptimize(totalParsedCount);
    }

    state.SetItemsProcessed(state.iterations() * msgCountPerThread * numThreads);
    state.SetBytesProcessed(state.iterations() * threadBuffer.size() * numThreads);
}
BENCHMARK(BM_MultiThreadedAsyncParsing)->Arg(2)->Arg(4)->Arg(8)->Arg(16);

// Multi-threaded worst-case benchmark: Parallel parseAsync over noisy streams across N worker threads
static void BM_MultiThreadedNoisyAsyncParsing(benchmark::State& state)
{
    const int numThreads = static_cast<int>(state.range(0));
    const int msgCountPerThread = 500;
    const std::string noisyThreadBuffer = createWorstCaseNoisyBuffer(msgCountPerThread);

    for (auto _ : state)
    {
        std::atomic<size_t> totalParsedCount{0};
        std::vector<std::future<size_t>> futures;
        futures.reserve(numThreads);

        for (int t = 0; t < numThreads; ++t)
        {
            futures.push_back(std::async(std::launch::async, [noisyThreadBuffer]() {
                std::string copy = noisyThreadBuffer;
                size_t parsedCount = 0;
                siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
                    parsedCount++;
                    auto [callId, sdpCount] = validateParsedSipMessage(sipm);
                    benchmark::DoNotOptimize(callId);
                    benchmark::DoNotOptimize(sdpCount);
                });
                return parsedCount;
            }));
        }

        for (auto& f : futures)
        {
            totalParsedCount += f.get();
        }
        benchmark::DoNotOptimize(totalParsedCount);
    }

    state.SetItemsProcessed(state.iterations() * msgCountPerThread * numThreads);
    state.SetBytesProcessed(state.iterations() * noisyThreadBuffer.size() * numThreads);
}
BENCHMARK(BM_MultiThreadedNoisyAsyncParsing)->Arg(2)->Arg(4)->Arg(8)->Arg(16);


// ============================================================================
// SINGLE STREAM COMPARISON: parseAsync vs parse (Single Thread vs Thread Pool Offload)
// ============================================================================

// Helper thread-safe queue for thread pool offloading benchmark
struct ConcurrentSipMessageQueue
{
    std::vector<siddiqsoft::sipmessage> queue;
    std::mutex                          mutex;
    std::condition_variable             cv;
    bool                                finished{false};

    void push(siddiqsoft::sipmessage&& msg)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push_back(std::move(msg));
        }
        cv.notify_one();
    }

    void setFinished()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            finished = true;
        }
        cv.notify_all();
    }

    bool pop(siddiqsoft::sipmessage& msg)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return !queue.empty() || finished; });
        if (queue.empty())
            return false;
        msg = std::move(queue.front());
        queue.erase(queue.begin());
        return true;
    }
};

// Scenario A: Single-Thread parseAsync (Direct In-Line Stream Parsing)
static void BM_SimulatedStream_ParseAsync_SingleThread(benchmark::State& state)
{
    const int         msgCount = 1000;
    const std::string streamBuffer = createMultiMessageBuffer(msgCount);

    for (auto _ : state)
    {
        std::string copy  = streamBuffer;
        size_t      parsedCount = 0;
        siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            parsedCount++;
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        });
        benchmark::DoNotOptimize(parsedCount);
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * streamBuffer.size());
}
BENCHMARK(BM_SimulatedStream_ParseAsync_SingleThread);

// Scenario B: Single-Thread parse (Vector Allocation & Sequential Iteration)
static void BM_SimulatedStream_Parse_SingleThread(benchmark::State& state)
{
    const int         msgCount = 1000;
    const std::string streamBuffer = createMultiMessageBuffer(msgCount);

    for (auto _ : state)
    {
        std::string copy = streamBuffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (const auto& sipm : msgs)
        {
            auto [callId, sdpCount] = validateParsedSipMessage(sipm);
            benchmark::DoNotOptimize(callId);
            benchmark::DoNotOptimize(sdpCount);
        }
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * streamBuffer.size());
}
BENCHMARK(BM_SimulatedStream_Parse_SingleThread);

// Scenario C: Stream I/O Thread running parseAsync + Offloading to Worker Thread Pool
static void BM_SimulatedStream_ParseAsync_WithThreadPoolOffload(benchmark::State& state)
{
    const int         msgCount = 1000;
    const int         workerCount = 4;
    const std::string streamBuffer = createMultiMessageBuffer(msgCount);

    for (auto _ : state)
    {
        ConcurrentSipMessageQueue queue;
        std::atomic<size_t>       processedCount{0};
        std::vector<std::thread>  workers;

        // Spawn worker threads to process parsed SIP messages offloaded from callback
        for (int i = 0; i < workerCount; ++i)
        {
            workers.emplace_back([&queue, &processedCount]() {
                siddiqsoft::sipmessage msg;
                while (queue.pop(msg))
                {
                    processedCount++;
                    auto [callId, sdpCount] = validateParsedSipMessage(msg);
                    benchmark::DoNotOptimize(callId);
                    benchmark::DoNotOptimize(sdpCount);
                }
            });
        }

        // Single Stream I/O thread runs parseAsync and pushes parsed messages into queue
        std::string copy = streamBuffer;
        siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) {
            queue.push(std::move(sipm));
        });
        queue.setFinished();

        for (auto& w : workers)
        {
            if (w.joinable())
                w.join();
        }
        benchmark::DoNotOptimize(processedCount);
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * streamBuffer.size());
}
BENCHMARK(BM_SimulatedStream_ParseAsync_WithThreadPoolOffload);

// Scenario D: Stream I/O Thread running parse() to Vector + Handoff to Worker Thread Pool
static void BM_SimulatedStream_Parse_WithThreadPoolHandoff(benchmark::State& state)
{
    const int         msgCount = 1000;
    const int         workerCount = 4;
    const std::string streamBuffer = createMultiMessageBuffer(msgCount);

    for (auto _ : state)
    {
        ConcurrentSipMessageQueue queue;
        std::atomic<size_t>       processedCount{0};
        std::vector<std::thread>  workers;

        for (int i = 0; i < workerCount; ++i)
        {
            workers.emplace_back([&queue, &processedCount]() {
                siddiqsoft::sipmessage msg;
                while (queue.pop(msg))
                {
                    processedCount++;
                    auto [callId, sdpCount] = validateParsedSipMessage(msg);
                    benchmark::DoNotOptimize(callId);
                    benchmark::DoNotOptimize(sdpCount);
                }
            });
        }

        // Single Stream I/O thread runs parse() into vector first, then pushes to queue
        std::string copy = streamBuffer;
        auto        bs   = copy.begin();
        auto        msgs = siddiqsoft::sip2json::parse(bs, copy.end());
        for (auto& sipm : msgs)
        {
            queue.push(std::move(sipm));
        }
        queue.setFinished();

        for (auto& w : workers)
        {
            if (w.joinable())
                w.join();
        }
        benchmark::DoNotOptimize(processedCount);
    }
    state.SetItemsProcessed(state.iterations() * msgCount);
    state.SetBytesProcessed(state.iterations() * streamBuffer.size());
}
BENCHMARK(BM_SimulatedStream_Parse_WithThreadPoolHandoff);

// ============================================================================
// Header Matching & Canonicalization Benchmarks: Case-Insensitive vs Case-Sensitive
// ============================================================================

static const std::vector<std::string> BENCHMARK_HEADER_KEYS = {
    "Via", "via", "VIA", "v",
    "Content-Length", "content-length", "CONTENT-LENGTH", "l",
    "Content-Type", "content-type", "CONTENT-TYPE", "c",
    "Call-ID", "call-id", "CALL-ID", "i",
    "Contact", "contact", "CONTACT", "m",
    "From", "from", "FROM", "f",
    "To", "to", "TO", "t",
    "CSeq", "cseq", "CSEQ",
    "Expires", "expires", "EXPIRES",
    "Max-Forwards", "max-forwards", "MAX-FORWARDS",
    "X-Custom-Trace-ID", "x-custom-trace-id", "X-CUSTOM-TRACE-ID"
};

// Measures speed of header canonicalization & key matching
static void BM_HeaderCanonicalization(benchmark::State& state)
{
    size_t keyCount = BENCHMARK_HEADER_KEYS.size();
    size_t idx = 0;

    for (auto _ : state)
    {
        const auto& key = BENCHMARK_HEADER_KEYS[idx % keyCount];
        auto result = siddiqsoft::canonicalizeHeaderKey(key);
        benchmark::DoNotOptimize(result);
        idx++;
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_HeaderCanonicalization);

// Measures parsing speed of frames with lowercase/mixed-case headers
static void BM_ParseLowercaseAndMixedCaseHeaders(benchmark::State& state)
{
    static const std::string sipMixedCase =
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

    for (auto _ : state)
    {
        std::string copy = sipMixedCase;
        auto bs = copy.begin();
        auto sipm = siddiqsoft::sip2json::parseFromBuffer(bs, copy.end());
        auto [callId, sdpCount] = validateParsedSipMessage(sipm);
        benchmark::DoNotOptimize(callId);
        benchmark::DoNotOptimize(sdpCount);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sipMixedCase.size());
}
BENCHMARK(BM_ParseLowercaseAndMixedCaseHeaders);



