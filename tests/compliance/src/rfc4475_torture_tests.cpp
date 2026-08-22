/*
    A SIP Parser for Modern C++: RFC 4475 SIP Torture Test Suite
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
    // RFC 4475 Section 3.1.1.1: Valid Short Message (wshort)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_1_ValidShortMessage)
    {
        // RFC 4475 3.1.1.1: Minimum legal SIP message
        std::string rawMsg = "REGISTER sip:example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKnashds7\r\n"
                             "From: <sip:user@example.com>;tag=123\r\n"
                             "To: <sip:user@example.com>\r\n"
                             "Call-ID: 12345678@192.0.2.1\r\n"
                             "CSeq: 1 REGISTER\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ(siddiqsoft::METHOD_REGISTER, sipm.getMethod());
        EXPECT_EQ("sip:example.com", sipm.getUri());
        EXPECT_EQ("12345678@192.0.2.1", sipm.getCallID());
        EXPECT_EQ(0, sipm.getContentLength());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.2: Extra Whitespace & Case Variations (clnfrn)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_2_WhitespaceAndCaseVariations)
    {
        // RFC 4475 3.1.1.3: Mixed-case header field names and compact forms
        std::string rawMsg = "INVITE sip:user@example.com SIP/2.0\r\n"
                             "v: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK-torture-1\r\n"
                             "f: <sip:alice@example.com>;tag=abc\r\n"
                             "t: <sip:bob@example.com>\r\n"
                             "i: torture-call-id-4475\r\n"
                             "CSeq: 42 INVITE\r\n"
                             "m: <sip:alice@192.0.2.1:5060>\r\n"
                             "c: application/sdp\r\n"
                             "l: 0\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ(siddiqsoft::METHOD_INVITE, sipm.getMethod());
        EXPECT_EQ("torture-call-id-4475", sipm.getCallID());
        EXPECT_EQ("torture-call-id-4475", sipm.getCallIDView());
        EXPECT_EQ(0, sipm.getContentLength());
        EXPECT_EQ("application/sdp", sipm.getContentType());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.3: Multiline Headers & Line Folding (foldhdr)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_3_MultilineHeaderFolding)
    {
        // RFC 4475 header folding with LWSP (\r\n\t and \r\n )
        std::string rawMsg = "OPTIONS sip:user@example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;\r\n"
                             "\tbranch=z9hG4bK-folded-branch\r\n"
                             "From: <sip:caller@example.com>;\r\n"
                             " tag=folded-tag-123\r\n"
                             "To: <sip:user@example.com>\r\n"
                             "Call-ID: folded-header-test-id\r\n"
                             "CSeq: 100 OPTIONS\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("OPTIONS", sipm.getMethod());
        EXPECT_EQ("folded-header-test-id", sipm.getCallID());
        EXPECT_TRUE(sipm.headers().contains("Via"));
        EXPECT_TRUE(sipm.headers().contains("From"));
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.2.1: Unknown / Extension Headers (unkhdr)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_2_1_UnknownHeadersPreserved)
    {
        // Unknown extension headers should be preserved in headers map
        std::string rawMsg = "REGISTER sip:example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\n"
                             "From: <sip:user@example.com>;tag=1\r\n"
                             "To: <sip:user@example.com>\r\n"
                             "Call-ID: unkhdr-test-id\r\n"
                             "CSeq: 1 REGISTER\r\n"
                             "X-Custom-Header: Custom-Value-123\r\n"
                             "Unknown-Extension-Field: 999\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("unkhdr-test-id", sipm.getCallID());
        EXPECT_TRUE(sipm.headers().contains("X-Custom-Header"));
        EXPECT_EQ("Custom-Value-123", sipm.getHeader<std::string>("X-Custom-Header"));
        EXPECT_TRUE(sipm.headers().contains("Unknown-Extension-Field"));
        EXPECT_EQ("999", sipm.getHeader<std::string>("Unknown-Extension-Field"));
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.2.2: Multiple Via Headers (multvia)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_2_2_MultipleViaHeadersAsArray)
    {
        std::string rawMsg = "INVITE sip:user@example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP proxy1.example.com:5060;branch=z9hG4bK111\r\n"
                             "Via: SIP/2.0/UDP client.example.com:5060;branch=z9hG4bK222\r\n"
                             "From: <sip:alice@example.com>;tag=1\r\n"
                             "To: <sip:bob@example.com>\r\n"
                             "Call-ID: multvia-test-id\r\n"
                             "CSeq: 1 INVITE\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("multvia-test-id", sipm.getCallID());
        EXPECT_TRUE(sipm.headers().contains("Via"));
        EXPECT_TRUE(sipm.headers()["Via"].is_array());
        EXPECT_EQ(2, sipm.headers()["Via"].size());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.2.3: Non-Standard Custom Method (badact)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_2_3_UnsupportedCustomToken_ThrowsException)
    {
        std::string rawMsg = "BENCHMARK sip:user@example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\n"
                             "From: <sip:user@example.com>;tag=1\r\n"
                             "To: <sip:user@example.com>\r\n"
                             "Call-ID: badact-test-id\r\n"
                             "CSeq: 1 BENCHMARK\r\n"
                             "Content-Length: 0\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_startline_error);
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.2.4: Negative Content-Length (badlen)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_2_4_NegativeContentLength_ThrowsException)
    {
        std::string rawMsg = "REGISTER sip:example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\n"
                             "From: <sip:user@example.com>;tag=1\r\n"
                             "To: <sip:user@example.com>\r\n"
                             "Call-ID: badlen-test-id\r\n"
                             "CSeq: 1 REGISTER\r\n"
                             "Content-Length: -10\r\n"
                             "\r\n";
        auto        bs     = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_document_error);
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.2.5: Incomplete Buffer / Truncated Stream (shortbuf)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_2_5_IncompleteHeaderDelimiter_ThrowsException)
    {
        // Truncated buffer missing \r\n\r\n ending
        std::string rawMsg = "REGISTER sip:example.com SIP/2.0\r\n"
                             "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bK123\r\n"
                             "From: <sip:user@example.com>;tag=1\r\n"
                             "Call-ID: incomplete-buffer-id\r\n";
        auto        bs     = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), incomplete_buffer_for_header_error);
    }
} // namespace siddiqsoft
