/*
    A SIP Parser for Modern C++: RFC 3261 Core Compliance Test Suite
    Version 1.0.0
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License
    Copyright (c) 2003-2024, Abdelkareem Siddiq
*/

#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "siddiqsoft/sip2json.hpp"

namespace siddiqsoft
{
    //-------------------------------------------------------------------------
    // RFC 3261 Section 7.1: Request Line Compliance
    //-------------------------------------------------------------------------
    TEST(RFC3261_Compliance, RequestLine_StandardMethods)
    {
        const std::vector<std::string_view> rfcMethods = {"INVITE",
                                                          "ACK",
                                                          "OPTIONS",
                                                          "BYE",
                                                          "CANCEL",
                                                          "REGISTER",
                                                          "SUBSCRIBE",
                                                          "NOTIFY",
                                                          "REFER",
                                                          "PUBLISH",
                                                          "UPDATE",
                                                          "PRACK",
                                                          "INFO",
                                                          "MESSAGE"};

        for (auto method : rfcMethods)
        {
            std::string rawMsg =
                    std::format("{} sip:user@example.com SIP/2.0\r\nVia: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\nFrom: "
                                "<sip:user@example.com>;tag=1\r\nTo: <sip:user@example.com>\r\nCall-ID: test-callid-123\r\nCSeq: 1 "
                                "{}\r\nContent-Length: 0\r\n\r\n",
                                method,
                                method);
            auto bs = rawMsg.begin();

            sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
            EXPECT_TRUE(sipm.isMessageRequest());
            EXPECT_FALSE(sipm.isMessageResponse());
            EXPECT_EQ(method, sipm.getMethod());
            EXPECT_EQ(method, sipm.getMethodView());
            EXPECT_EQ("sip:user@example.com", sipm.getUri());
            EXPECT_EQ("sip:user@example.com", sipm.getUriView());
            EXPECT_EQ("SIP/2.0", sipm.value("/s/version"_json_pointer, ""));
        }
    }

    TEST(RFC3261_Compliance, RequestLine_InvalidVersion_ThrowsException)
    {
        std::string rawMsg = "INVITE sip:user@example.com SIP/1.0\r\nVia: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\nFrom: "
                             "<sip:user@example.com>;tag=1\r\nTo: <sip:user@example.com>\r\nCall-ID: test-callid-123\r\nCSeq: 1 "
                             "INVITE\r\nContent-Length: 0\r\n\r\n";
        auto        bs     = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_startline_error);
    }

    //-------------------------------------------------------------------------
    // RFC 3261 Section 7.2: Status Line Compliance
    //-------------------------------------------------------------------------
    TEST(RFC3261_Compliance, StatusLine_StandardResponseCodes)
    {
        struct TestStatus
        {
            uint32_t    code;
            std::string reason;
        };

        const std::vector<TestStatus> statusCases = {{100, "Trying"},
                                                     {180, "Ringing"},
                                                     {200, "OK"},
                                                     {202, "Accepted"},
                                                     {302, "Moved Temporarily"},
                                                     {400, "Bad Request"},
                                                     {401, "Unauthorized"},
                                                     {404, "Not Found"},
                                                     {486, "Busy Here"},
                                                     {500, "Server Internal Error"},
                                                     {600, "Busy Everywhere"}};

        for (const auto& tc : statusCases)
        {
            std::string rawMsg = std::format("SIP/2.0 {} {}\r\nVia: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\nFrom: "
                                             "<sip:user@example.com>;tag=1\r\nTo: <sip:user@example.com>;tag=2\r\nCall-ID: "
                                             "test-callid-123\r\nCSeq: 1 INVITE\r\nContent-Length: 0\r\n\r\n",
                                             tc.code,
                                             tc.reason);
            auto        bs     = rawMsg.begin();

            sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
            EXPECT_TRUE(sipm.isMessageResponse());
            EXPECT_FALSE(sipm.isMessageRequest());
            EXPECT_EQ(tc.code, sipm.getStatusCode());
            EXPECT_EQ(tc.reason, sipm.getReason());
            EXPECT_EQ(tc.reason, sipm.getReasonView());
        }
    }

    //-------------------------------------------------------------------------
    // RFC 3261 Section 7.3: Header Fields & Compact Names
    //-------------------------------------------------------------------------
    TEST(RFC3261_Compliance, HeaderFields_CaseInsensitivity)
    {
        std::string rawMsg =
                "INVITE sip:user@example.com SIP/2.0\r\n"
                "vIa: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\n"
                "fRoM: <sip:user@example.com>;tag=1\r\n"
                "tO: <sip:user@example.com>\r\n"
                "cALL-iD: test-callid-123\r\n"
                "cSeQ: 1 INVITE\r\n"
                "cONTENT-lENGTH: 0\r\n\r\n";
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_TRUE(sipm.headers().contains("Via"));
        EXPECT_TRUE(sipm.headers().contains("From"));
        EXPECT_TRUE(sipm.headers().contains("To"));
        EXPECT_TRUE(sipm.headers().contains("Call-ID"));
        EXPECT_TRUE(sipm.headers().contains("CSeq"));
        EXPECT_TRUE(sipm.headers().contains("Content-Length"));
    }

    TEST(RFC3261_Compliance, HeaderFields_PrecomputedHashMatching)
    {
        // Verify constexpr 64-bit FNV-1a hash matching evaluates correctly
        EXPECT_EQ(0x7f845078d7a5c0b5ULL, hash_header_key("from"));
        EXPECT_EQ(0x08c83907b56ac0a4ULL, hash_header_key("to"));
        EXPECT_EQ(0x68e8f7194eba5d73ULL, hash_header_key("via"));
        EXPECT_EQ(0x82acdf99cfbd03c1ULL, hash_header_key("call-id"));
        EXPECT_EQ(0x1a1d7b9090b95b09ULL, hash_header_key("cseq"));
        EXPECT_EQ(0x2d69a1e6ee916e7dULL, hash_header_key("content-length"));
        EXPECT_EQ(0x0f4dd5cf6a7a0235ULL, hash_header_key("content-type"));

        // Verify lookup maps to static references
        EXPECT_EQ(&HFS_FROM, &canonicalizeHeaderKey("fRoM"));
        EXPECT_EQ(&HFS_VIA, &canonicalizeHeaderKey("vIa"));
        EXPECT_EQ(&HFS_CONTENT_LENGTH, &canonicalizeHeaderKey("cONTENT-lENGTH"));
    }

    TEST(RFC3261_Compliance, HeaderFields_CompactNames)
    {
        // RFC 3261 compact form header names:
        // v = Via, f = From, t = To, i = Call-ID, c = Content-Type, l = Content-Length, m = Contact
        std::string rawMsg = "INVITE sip:user@example.com SIP/2.0\r\nv: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\nf: "
                             "<sip:alice@example.com>;tag=1\r\nt: <sip:bob@example.com>\r\ni: compact-callid-999\r\nCSeq: 1 "
                             "INVITE\r\nm: <sip:alice@192.0.2.1:5060>\r\nc: application/sdp\r\nl: 0\r\n\r\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("compact-callid-999", sipm.getCallID());
        EXPECT_EQ("compact-callid-999", sipm.getCallIDView());
        EXPECT_EQ(0, sipm.getContentLength());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.headers().contains("Via"));
        EXPECT_TRUE(sipm.headers().contains("From"));
        EXPECT_TRUE(sipm.headers().contains("To"));
        EXPECT_TRUE(sipm.headers().contains("Contact"));
    }

    //-------------------------------------------------------------------------
    // RFC 3261 Section 7.4: Message Body & Content-Length
    //-------------------------------------------------------------------------
    TEST(RFC3261_Compliance, MessageBody_SDP_Parsing)
    {
        std::string sdpBody = "v=0\r\no=alice 2890844526 2890844526 IN IP4 192.0.2.1\r\ns=SDP Seminar\r\nc=IN IP4 192.0.2.1\r\nt=0 "
                              "0\r\nm=audio 49170 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\n";
        std::string rawMsg =
                std::format("INVITE sip:bob@example.com SIP/2.0\r\nVia: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\nFrom: "
                            "<sip:alice@example.com>;tag=1\r\nTo: <sip:bob@example.com>\r\nCall-ID: sdp-body-test\r\nCSeq: 1 "
                            "INVITE\r\nContent-Type: application/sdp\r\nContent-Length: {}\r\n\r\n{}",
                            sdpBody.length(),
                            sdpBody);
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ(sdpBody.length(), sipm.getContentLength());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.contains("b"));
        EXPECT_TRUE(sipm.contains("/b/sdp"_json_pointer));
    }

    TEST(RFC3261_Compliance, MessageBody_UnixLFLineEndings)
    {
        std::string rawMsg = "REGISTER sip:example.com SIP/2.0\nVia: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\nFrom: "
                             "<sip:user@example.com>;tag=1\nTo: <sip:user@example.com>\nCall-ID: lf-line-ending-123\nCSeq: 1 "
                             "REGISTER\nContent-Length: 0\n\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("lf-line-ending-123", sipm.getCallID());
        EXPECT_EQ(siddiqsoft::METHOD_REGISTER, sipm.getMethod());
        EXPECT_EQ(0, sipm.getContentLength());
    }
} // namespace siddiqsoft
