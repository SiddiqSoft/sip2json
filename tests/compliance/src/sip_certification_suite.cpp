/*
    A SIP Parser for Modern C++: Comprehensive SIP Standard Certification Suite
    Version 1.0.0
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License
    Copyright (c) 2003-2024, Abdelkareem Siddiq
*/

#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "siddiqsoft/sip2json.hpp"

namespace siddiqsoft
{
    //-------------------------------------------------------------------------
    // Certification Group 1: RFC 3261 Core Startline & Method Certification
    //-------------------------------------------------------------------------
    TEST(SIP_Certification, CERT_RFC3261_StartLine_RequestMethods)
    {
        const std::vector<std::pair<std::string_view, std::string_view>> testCases = {
                {siddiqsoft::METHOD_INVITE, "sip:bob@biloxi.example.com"},
                {"ACK", "sip:bob@biloxi.example.com"},
                {"OPTIONS", "sip:carol@chicago.example.com"},
                {"BYE", "sip:alice@atlanta.example.com"},
                {"CANCEL", "sip:bob@biloxi.example.com"},
                {siddiqsoft::METHOD_REGISTER, "sip:registrar.biloxi.example.com"},
                {siddiqsoft::METHOD_SUBSCRIBE, "sip:user@example.com"},
                {"NOTIFY", "sip:user@example.com"},
                {"REFER", "sip:user@example.com"},
                {"PUBLISH", "sip:user@example.com"},
                {"UPDATE", "sip:user@example.com"},
                {"PRACK", "sip:user@example.com"},
                {"INFO", "sip:user@example.com"},
                {"MESSAGE", "sip:user@example.com"}};

        for (const auto& [method, uri] : testCases)
        {
            std::string rawMsg = std::format("{} {} SIP/2.0\r\n"
                                             "Via: SIP/2.0/UDP pc33.atlanta.com:5060;branch=z9hG4bKnashds8\r\n"
                                             "Max-Forwards: 70\r\n"
                                             "To: Bob <sip:bob@biloxi.example.com>\r\n"
                                             "From: Alice <sip:alice@atlanta.example.com>;tag=1928301774\r\n"
                                             "Call-ID: cert-startline-{}\r\n"
                                             "CSeq: 314159 {}\r\n"
                                             "Contact: <sip:alice@pc33.atlanta.com>\r\n"
                                             "Content-Length: 0\r\n\r\n",
                                             method,
                                             uri,
                                             method,
                                             method);

            auto       bs   = rawMsg.begin();
            sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

            EXPECT_TRUE(sipm.isMessageRequest()) << "Failed for method: " << method;
            EXPECT_EQ(method, sipm.getMethod());
            EXPECT_EQ(method, sipm.getMethodView());
            EXPECT_EQ(uri, sipm.getUri());
            EXPECT_EQ(uri, sipm.getUriView());
            EXPECT_EQ("SIP/2.0", sipm.value("/s/version"_json_pointer, ""));
        }
    }

    TEST(SIP_Certification, CERT_RFC3261_StartLine_ResponseClasses)
    {
        const std::vector<std::tuple<uint32_t, std::string_view, std::string_view>> responseCases = {
                {100, "Trying", "1xx Informational"},
                {180, "Ringing", "1xx Informational"},
                {183, "Session Progress", "1xx Informational"},
                {200, "OK", "2xx Success"},
                {202, "Accepted", "2xx Success"},
                {301, "Moved Permanently", "3xx Redirection"},
                {302, "Moved Temporarily", "3xx Redirection"},
                {400, "Bad Request", "4xx Client Error"},
                {401, "Unauthorized", "4xx Client Error"},
                {403, "Forbidden", "4xx Client Error"},
                {404, "Not Found", "4xx Client Error"},
                {407, "Proxy Authentication Required", "4xx Client Error"},
                {408, "Request Timeout", "4xx Client Error"},
                {486, "Busy Here", "4xx Client Error"},
                {500, "Server Internal Error", "5xx Server Error"},
                {502, "Bad Gateway", "5xx Server Error"},
                {503, "Service Unavailable", "5xx Server Error"},
                {600, "Busy Everywhere", "6xx Global Failure"},
                {603, "Decline", "6xx Global Failure"}};

        for (const auto& [code, reason, category] : responseCases)
        {
            std::string rawMsg = std::format("SIP/2.0 {} {}\r\n"
                                             "Via: SIP/2.0/UDP pc33.atlanta.com:5060;branch=z9hG4bKnashds8\r\n"
                                             "From: Alice <sip:alice@atlanta.example.com>;tag=1928301774\r\n"
                                             "To: Bob <sip:bob@biloxi.example.com>;tag=a6c85cf\r\n"
                                             "Call-ID: cert-response-{}\r\n"
                                             "CSeq: 314159 INVITE\r\n"
                                             "Content-Length: 0\r\n\r\n",
                                             code,
                                             reason,
                                             code);

            auto       bs   = rawMsg.begin();
            sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

            EXPECT_TRUE(sipm.isMessageResponse()) << "Failed for status code: " << code;
            EXPECT_EQ(code, sipm.getStatusCode());
            EXPECT_EQ(reason, sipm.getReason());
            EXPECT_EQ(reason, sipm.getReasonView());
        }
    }

    //-------------------------------------------------------------------------
    // Certification Group 2: RFC 3261 Header Canonicalization & Compact Aliases
    //-------------------------------------------------------------------------
    TEST(SIP_Certification, CERT_RFC3261_CompactHeader_Expansion)
    {
        // Certifies all 10 RFC 3261 compact form header field abbreviations
        std::string rawMsg = "INVITE sip:bob@biloxi.example.com SIP/2.0\r\n"
                             "v: SIP/2.0/UDP pc33.atlanta.com:5060;branch=z9hG4bK-compact-test\r\n"
                             "f: Alice <sip:alice@atlanta.example.com>;tag=1928301774\r\n"
                             "t: Bob <sip:bob@biloxi.example.com>\r\n"
                             "i: cert-compact-callid-100\r\n"
                             "CSeq: 1 INVITE\r\n"
                             "m: <sip:alice@pc33.atlanta.com>\r\n"
                             "c: application/sdp\r\n"
                             "l: 0\r\n"
                             "s: Certified SIP Testing\r\n"
                             "k: 100rel, timer\r\n"
                             "e: gzip\r\n"
                             "\r\n";

        auto       bs   = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("cert-compact-callid-100", sipm.getCallID());
        EXPECT_EQ("cert-compact-callid-100", sipm.getCallIDView());
        EXPECT_EQ(0, sipm.getContentLength());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.headers().contains("Via"));
        EXPECT_TRUE(sipm.headers().contains("From"));
        EXPECT_TRUE(sipm.headers().contains("To"));
        EXPECT_TRUE(sipm.headers().contains("Call-ID"));
        EXPECT_TRUE(sipm.headers().contains("Contact"));
        EXPECT_TRUE(sipm.headers().contains("Content-Type"));
        EXPECT_TRUE(sipm.headers().contains("Content-Length"));
        EXPECT_TRUE(sipm.headers().contains("Subject"));
        EXPECT_TRUE(sipm.headers().contains("Supported"));
        EXPECT_TRUE(sipm.headers().contains("Content-Encoding"));
    }

    TEST(SIP_Certification, CERT_RFC3261_Header_CaseInsensitivity)
    {
        std::string rawMsg = "REGISTER sip:example.com SIP/2.0\r\n"
                             "vIA: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK-case\r\n"
                             "fROM: <sip:user@example.com>;tag=123\r\n"
                             "tO: <sip:user@example.com>\r\n"
                             "cALL-iD: cert-case-insensitive-999\r\n"
                             "cSEq: 1 REGISTER\r\n"
                             "uSER-aGENT: Antigravity-Certified-SIP/1.0\r\n"
                             "eXPIRES: 3600\r\n"
                             "cONTENT-tYPE: application/sdp\r\n"
                             "cONTENT-lENGTH: 0\r\n"
                             "\r\n";

        auto       bs   = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("cert-case-insensitive-999", sipm.getCallID());
        EXPECT_EQ("cert-case-insensitive-999", sipm.getCallIDView());
        EXPECT_EQ(3600, sipm.getExpires());
        EXPECT_EQ(0, sipm.getContentLength());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.getUserAgent().contains("Antigravity-Certified-SIP"));
    }

    //-------------------------------------------------------------------------
    // Certification Group 3: Extension RFC Headers Certification
    //-------------------------------------------------------------------------
    TEST(SIP_Certification, CERT_Extension_RFC_Headers)
    {
        // Validates parsing of RFC 3262 (PRACK/100rel), RFC 6665 (Event/SubState), RFC 3515 (Refer), RFC 3903 (Publish)
        std::string rawMsg = "NOTIFY sip:alice@pc33.atlanta.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKext\r\n"
                             "From: <sip:bob@example.com>;tag=123\r\n"
                             "To: <sip:alice@example.com>;tag=456\r\n"
                             "Call-ID: cert-ext-rfc-headers\r\n"
                             "CSeq: 10 NOTIFY\r\n"
                             "Event: presence\r\n"
                             "Subscription-State: active;expires=600\r\n"
                             "Refer-To: <sip:carol@chicago.example.com>\r\n"
                             "SIP-ETag: 4321-publish-tag\r\n"
                             "RAck: 1 1 100 INVITE\r\n"
                             "RSeq: 1\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";

        auto       bs   = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("NOTIFY", sipm.getMethod());
        EXPECT_EQ("cert-ext-rfc-headers", sipm.getCallID());
        EXPECT_TRUE(sipm.headers().contains("Event"));
        EXPECT_EQ("presence", sipm.getHeader<std::string>("Event"));
        EXPECT_TRUE(sipm.headers().contains("Subscription-State"));
        EXPECT_TRUE(sipm.headers().contains("Refer-To"));
        EXPECT_TRUE(sipm.headers().contains("SIP-ETag"));
        EXPECT_TRUE(sipm.headers().contains("RAck"));
        EXPECT_TRUE(sipm.headers().contains("RSeq"));
    }

    //-------------------------------------------------------------------------
    // Certification Group 4: RFC 4566 SDP Session Description Certification
    //-------------------------------------------------------------------------
    TEST(SIP_Certification, CERT_RFC4566_SDP_FullSpecification)
    {
        std::string sdpData = "v=0\r\n"
                              "o=jdoe 2890844526 2890844526 IN IP4 10.47.16.5\r\n"
                              "s=SDP Seminar\r\n"
                              "i=A Seminar on the session description protocol\r\n"
                              "u=http://www.example.com/seminars/sdp.pdf\r\n"
                              "e=j.doe@example.com (Jane Doe)\r\n"
                              "c=IN IP4 224.2.17.12/127\r\n"
                              "t=2873397496 2873404696\r\n"
                              "m=audio 49170 RTP/AVP 0 8 97\r\n"
                              "a=rtpmap:0 PCMU/8000\r\n"
                              "a=rtpmap:8 PCMA/8000\r\n"
                              "a=rtpmap:97 iLBC/8000\r\n"
                              "a=sendrecv\r\n";

        std::string rawMsg = std::format("INVITE sip:jdoe@example.com SIP/2.0\r\n"
                                         "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKsdpcert\r\n"
                                         "From: <sip:caller@example.com>;tag=1\r\n"
                                         "To: <sip:jdoe@example.com>\r\n"
                                         "Call-ID: sdp-cert-4566\r\n"
                                         "CSeq: 1 INVITE\r\n"
                                         "Content-Type: application/sdp\r\n"
                                         "Content-Length: {}\r\n"
                                         "\r\n"
                                         "{}",
                                         sdpData.length(),
                                         sdpData);

        auto       bs   = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("sdp-cert-4566", sipm.getCallID());
        EXPECT_EQ(sdpData.length(), sipm.getContentLength());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));

        const auto& sdpJson = sipm["/b/sdp/0"_json_pointer];
        EXPECT_TRUE(sdpJson.contains("v"));
        EXPECT_TRUE(sdpJson.contains("o"));
        EXPECT_TRUE(sdpJson.contains("s"));
        EXPECT_TRUE(sdpJson.contains("c"));
        EXPECT_TRUE(sdpJson.contains("m"));
    }

    //-------------------------------------------------------------------------
    // Certification Group 5: Serialization & Round-Trip Fidelity
    //-------------------------------------------------------------------------
    TEST(SIP_Certification, CERT_Serialization_RoundTrip_Fidelity)
    {
        sipmessage originalReq(siddiqsoft::METHOD_INVITE, "sip:bob@biloxi.example.com", "roundtrip-cert-id-555", 101);
        originalReq.setHeader(siddiqsoft::HF_CONTACT, "<sip:alice@pc33.atlanta.com>");
        originalReq.setHeader(siddiqsoft::HF_USER_AGENT, "Antigravity-Certified/1.0");

        // Serialize
        std::string serializedStr = sip2json::serialize(originalReq);
        EXPECT_FALSE(serializedStr.empty());
        EXPECT_TRUE(serializedStr.contains("INVITE sip:bob@biloxi.example.com SIP/2.0\r\n"));

        // Parse back
        auto       bs              = serializedStr.begin();
        sipmessage deserializedReq = sip2json::parseFromBuffer(bs, serializedStr.end());

        EXPECT_EQ(siddiqsoft::METHOD_INVITE, deserializedReq.getMethod());
        EXPECT_EQ("sip:bob@biloxi.example.com", deserializedReq.getUri());
        EXPECT_EQ("roundtrip-cert-id-555", deserializedReq.getCallID());
        EXPECT_EQ("roundtrip-cert-id-555", deserializedReq.getCallIDView());
        EXPECT_TRUE(deserializedReq.getUserAgent().contains("Antigravity-Certified"));
    }
} // namespace siddiqsoft
