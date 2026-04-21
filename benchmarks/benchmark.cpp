/*
    sip2json Benchmarks
    Measures performance of parsing, serialization, and utility functions.
*/

#include <string>
#include <vector>
#include <format>

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
    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);
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
    siddiqsoft::sipmessage sipm("INVITE", "sip:bob@biloxi.com", siddiqsoft::createCallId(), 1);
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
        siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", callId, 1);
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
