/*
    sip2json Benchmarks
    Measures performance of parsing, serialization, and utility functions.
*/

#include <string>
#include <vector>
#include <format>
#include <fstream>
#include <chrono>
#include <ctime>
#include <limits>

#include "nlohmann/json.hpp"
#include "siddiqsoft/sip2json.hpp"

#include <benchmark/benchmark.h>


// ============================================================================
// Test data: realistic SIP messages of varying complexity
// ============================================================================

// Minimal SIP response (headers only, no body)
static const std::string SIP_RESPONSE_MINIMAL =
        "SIP/2.0 200 OK\r\n"
        "Via: SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK776asdhds\r\n"
        "To: sip:bob@biloxi.com;tag=a6c85cf\r\n"
        "From: sip:alice@atlanta.com;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: sip:bob@192.0.2.4\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

// SIP REGISTER request (headers only)
static const std::string SIP_REGISTER_REQUEST =
        "REGISTER sip:registrar.biloxi.com SIP/2.0\r\n"
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
static const std::string SIP_INVITE_WITH_SDP =
        "INVITE sip:bob@biloxi.com SIP/2.0\r\n"
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
static const std::string SIP_INVITE_COMPLEX_SDP =
        "INVITE sip:conference@server.example.com SIP/2.0\r\n"
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
static const std::string SIP_NOTIFY_LF =
        "NOTIFY sip:user@example.com SIP/2.0\n"
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


// ============================================================================
// PARSING BENCHMARKS
// ============================================================================

// Parse a minimal SIP response (no body)
static void BM_ParseMinimalResponse(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string buffer = SIP_RESPONSE_MINIMAL;
        auto        bs     = buffer.begin();
        auto        sipm   = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
        benchmark::DoNotOptimize(sipm);
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
        benchmark::DoNotOptimize(sipm);
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
        benchmark::DoNotOptimize(sipm);
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
        benchmark::DoNotOptimize(sipm);
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
        benchmark::DoNotOptimize(sipm);
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
        benchmark::DoNotOptimize(msgs);
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
        std::string copy  = buffer;
        int         count = 0;
        auto remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) { count++; });
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
    sipm.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Length", 0);

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
            .setBody("/sdp/0/c"_json_pointer,
                     nlohmann::json {{"type", "IN"}, {"subtype", "IP4"}, {"dn", "pc33.atlanta.com"}})
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

// getHeader on a parsed message
static void BM_GetHeader(benchmark::State& state)
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
BENCHMARK(BM_GetHeader);

// setHeader on a message
static void BM_SetHeader(benchmark::State& state)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    for (auto _ : state)
    {
        sipm.setHeader("X-Custom-Header", "benchmark-value");
        benchmark::DoNotOptimize(sipm);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SetHeader);


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
        benchmark::DoNotOptimize(sipm);
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
        benchmark::DoNotOptimize(msgs);
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
        std::string copy  = buffer;
        int         count = 0;
        auto remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) { count++; });
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
        benchmark::DoNotOptimize(msgs);
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
        std::string copy  = buffer;
        int         count = 0;
        auto remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) { count++; });
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
        benchmark::DoNotOptimize(msgs);
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
        std::string copy  = buffer;
        int         count = 0;
        auto remaining = siddiqsoft::sip2json::parseAsync(copy, [&](auto&& sipm) { count++; });
        benchmark::DoNotOptimize(remaining);
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * packetCount);
    state.SetBytesProcessed(state.iterations() * buffer.size());
}
BENCHMARK(BM_VariableSizeAsyncStressTest)->Arg(10)->Arg(50)->Arg(100)->Arg(500);


// ============================================================================
// CUSTOM BENCHMARK REPORTER FOR HTML AND JSON OUTPUT
// ============================================================================

/// Custom reporter that generates HTML and JSON reports from benchmark results
class BenchmarkReporter : public benchmark::BenchmarkReporter
{
public:
    BenchmarkReporter() = default;

    bool ReportContext(const Context& context) override
    {
        return true;
    }

    void ReportRuns(const std::vector<Run>& reports) override
    {
        if (reports.empty()) return;

        // Collect all benchmark data
        nlohmann::json benchmarks = nlohmann::json::array();
        
        for (const auto& run : reports)
        {
            if (!run.report_label.empty()) continue; // Skip aggregate runs
            
            nlohmann::json benchmark_data;
            benchmark_data["name"] = run.benchmark_name;
            benchmark_data["iterations"] = run.iterations;
            benchmark_data["real_time"] = run.real_accumulated_time;
            benchmark_data["cpu_time"] = run.cpu_accumulated_time;
            benchmark_data["time_unit"] = "ns";
            benchmark_data["items_per_second"] = run.items_per_second;
            benchmark_data["bytes_per_second"] = run.bytes_per_second;
            
            benchmarks.push_back(benchmark_data);
        }

        // Generate JSON report
        generateJsonReport(benchmarks);
        
        // Generate HTML report
        generateHtmlReport(benchmarks);
    }

    void Finalize() override {}

private:
    void generateJsonReport(const nlohmann::json& benchmarks)
    {
        nlohmann::json report;
        report["benchmarks"] = benchmarks;
        report["context"]["date"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        std::ofstream json_file("benchmark_report.json");
        json_file << report.dump(2);
        json_file.close();
        
        std::cout << "✓ JSON report generated: benchmark_report.json\n";
    }

    void generateHtmlReport(const nlohmann::json& benchmarks)
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::string timestamp;
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        timestamp = buffer;

        double total_iterations = 0;
        double fastest_time = std::numeric_limits<double>::max();
        double slowest_time = 0;

        for (const auto& b : benchmarks)
        {
            total_iterations += b["iterations"].get<double>();
            double real_time = b["real_time"].get<double>();
            fastest_time = std::min(fastest_time, real_time);
            slowest_time = std::max(slowest_time, real_time);
        }

        std::string html = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>sip2json Benchmark Report</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif; background: #f5f5f5; color: #333; }
        .container { max-width: 1400px; margin: 0 auto; padding: 20px; }
        header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 30px; border-radius: 8px; margin-bottom: 30px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        header h1 { font-size: 2.5em; margin-bottom: 10px; }
        header p { font-size: 1.1em; opacity: 0.9; }
        .summary { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .summary-card { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-left: 4px solid #667eea; }
        .summary-card h3 { color: #667eea; margin-bottom: 10px; font-size: 0.9em; text-transform: uppercase; }
        .summary-card .value { font-size: 2em; font-weight: bold; color: #333; }
        .summary-card .unit { font-size: 0.9em; color: #999; margin-left: 5px; }
        table { width: 100%; border-collapse: collapse; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 30px; }
        thead { background: #f8f9fa; border-bottom: 2px solid #dee2e6; }
        th { padding: 15px; text-align: left; font-weight: 600; color: #495057; }
        td { padding: 12px 15px; border-bottom: 1px solid #dee2e6; }
        tbody tr:hover { background: #f8f9fa; }
        .benchmark-name { font-family: "Courier New", monospace; font-size: 0.9em; color: #667eea; font-weight: 500; }
        .time-value { font-family: "Courier New", monospace; text-align: right; }
        .iterations { text-align: center; color: #666; }
        footer { text-align: center; color: #999; margin-top: 40px; padding-top: 20px; border-top: 1px solid #dee2e6; }
        .section-title { font-size: 1.5em; font-weight: 600; color: #333; margin: 30px 0 20px 0; }
        .stress-tests { background: #fff3cd; border-left: 4px solid #ffc107; padding: 15px; border-radius: 4px; margin-bottom: 20px; }
        .stress-tests strong { color: #856404; }
        .stress-tests ul { margin-left: 20px; margin-top: 10px; }
        a { color: #667eea; text-decoration: none; }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>sip2json Benchmark Report</h1>
            <p>Performance Analysis - Generated on )";
        
        html += timestamp;
        html += R"(</p>
        </header>
        
        <div class="summary">
            <div class="summary-card">
                <h3>Total Benchmarks</h3>
                <div class="value">)";
        html += std::to_string(benchmarks.size());
        html += R"(</div>
            </div>
            <div class="summary-card">
                <h3>Total Iterations</h3>
                <div class="value">)";
        html += std::to_string(static_cast<long long>(total_iterations));
        html += R"(</div>
            </div>
            <div class="summary-card">
                <h3>Fastest Benchmark</h3>
                <div class="value">)";
        html += std::format("{:.2f}", fastest_time);
        html += R"( <span class="unit">ns</span></div>
            </div>
            <div class="summary-card">
                <h3>Slowest Benchmark</h3>
                <div class="value">)";
        html += std::format("{:.2f}", slowest_time);
        html += R"( <span class="unit">ns</span></div>
            </div>
        </div>
        
        <h2 class="section-title">Benchmark Results</h2>
        <table>
            <thead>
                <tr>
                    <th>Benchmark Name</th>
                    <th>Real Time (ns)</th>
                    <th>CPU Time (ns)</th>
                    <th>Iterations</th>
                    <th>Items/Sec</th>
                </tr>
            </thead>
            <tbody>
)";

        for (const auto& b : benchmarks)
        {
            html += "                <tr>\n";
            html += "                    <td><span class=\"benchmark-name\">" + b["name"].get<std::string>() + "</span></td>\n";
            html += std::format("                    <td class=\"time-value\">{:.2f}</td>\n", b["real_time"].get<double>());
            html += std::format("                    <td class=\"time-value\">{:.2f}</td>\n", b["cpu_time"].get<double>());
            html += "                    <td class=\"iterations\">" + std::to_string(b["iterations"].get<long long>()) + "</td>\n";
            html += std::format("                    <td class=\"time-value\">{:.0f}</td>\n", b["items_per_second"].get<double>());
            html += "                </tr>\n";
        }

        html += R"(            </tbody>
        </table>
        
        <h2 class="section-title">Stress Test Benchmarks</h2>
        <div class="stress-tests">
            <strong>Note:</strong> The following benchmarks test high-frequency decoding with large packets:
            <ul>
                <li>High-Frequency Decode: Single large packet parsing</li>
                <li>Stress Tests (100/1000): Multiple large packets in sequence</li>
                <li>Variable-Size Tests: Parameterized stress testing (10, 50, 100, 500 packets)</li>
            </ul>
        </div>
        
        <footer>
            <p>sip2json Benchmark Suite | C++20 Header-Only SIP Parser</p>
            <p>For more information, visit: <a href="https://github.com/siddiqsoftware/sip2json">github.com/siddiqsoftware/sip2json</a></p>
        </footer>
    </div>
</body>
</html>
)";

        std::ofstream html_file("benchmark_report.html");
        html_file << html;
        html_file.close();
        
        std::cout << "✓ HTML report generated: benchmark_report.html\n";
    }
};
