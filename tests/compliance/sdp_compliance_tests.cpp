/*
    A SIP Parser for Modern C++: SDP Compliance Test Suite (RFC 4566 / RFC 8866 / RFC 3264)
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
    // SDP Compliance Group 1: RFC 4566 / RFC 8866 Session-Level Syntax
    //-------------------------------------------------------------------------
    TEST(SDP_Compliance, CERT_SDP_SessionLevel_OriginAndConnection)
    {
        // Tests IPv4, IPv6, anonymous username (-), and multicast connection lines
        std::string sdpBody =
            "v=0\r\n"
            "o=- 2890844526 2890844526 IN IP6 2001:db8::1\r\n"
            "s=SDP IPv6 & Multicast Session\r\n"
            "i=Session Information line test\r\n"
            "u=http://www.example.com/sdp.pdf\r\n"
            "e=support@example.com (Technical Support)\r\n"
            "p=+1 555 0199\r\n"
            "c=IN IP6 2001:db8::1\r\n"
            "t=0 0\r\n"
            "m=audio 49170 RTP/AVP 0 8\r\n"
            "a=rtpmap:0 PCMU/8000\r\n";

        std::string rawMsg = std::format(
            "INVITE sip:user@example.com SIP/2.0\r\n"
            "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKsdp1\r\n"
            "From: <sip:caller@example.com>;tag=1\r\n"
            "To: <sip:user@example.com>\r\n"
            "Call-ID: cert-sdp-ipv6-test\r\n"
            "CSeq: 1 INVITE\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: {}\r\n\r\n{}", sdpBody.length(), sdpBody);

        auto bs = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("cert-sdp-ipv6-test", sipm.getCallID());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.contains("/b/sdp/0"_json_pointer));

        const auto& sdpJson = sipm["/b/sdp/0"_json_pointer];
        EXPECT_TRUE(sdpJson.contains("v"));
        EXPECT_TRUE(sdpJson.contains("o"));
        EXPECT_TRUE(sdpJson.contains("s"));
        EXPECT_TRUE(sdpJson.contains("i"));
        EXPECT_TRUE(sdpJson.contains("u"));
        EXPECT_TRUE(sdpJson.contains("e"));
        EXPECT_TRUE(sdpJson.contains("p"));
        EXPECT_TRUE(sdpJson.contains("c"));
        EXPECT_TRUE(sdpJson.contains("t"));
        EXPECT_TRUE(sdpJson.contains("m"));
    }

    //-------------------------------------------------------------------------
    // SDP Compliance Group 2: RFC 3264 Offer/Answer Direction Attributes
    //-------------------------------------------------------------------------
    TEST(SDP_Compliance, CERT_SDP_OfferAnswer_DirectionAttributes)
    {
        const std::vector<std::string_view> directionAttributes = {
            "sendrecv", "sendonly", "recvonly", "inactive"
        };

        for (auto dir : directionAttributes)
        {
            std::string sdpBody = std::format(
                "v=0\r\n"
                "o=alice 2890844526 2890844526 IN IP4 192.0.2.1\r\n"
                "s=Offer Answer Test\r\n"
                "c=IN IP4 192.0.2.1\r\n"
                "t=0 0\r\n"
                "m=audio 49170 RTP/AVP 0\r\n"
                "a=rtpmap:0 PCMU/8000\r\n"
                "a={}\r\n", dir);

            std::string rawMsg = std::format(
                "INVITE sip:bob@example.com SIP/2.0\r\n"
                "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKdir\r\n"
                "From: <sip:alice@example.com>;tag=1\r\n"
                "To: <sip:bob@example.com>\r\n"
                "Call-ID: sdp-direction-{}\r\n"
                "CSeq: 1 INVITE\r\n"
                "Content-Type: application/sdp\r\n"
                "Content-Length: {}\r\n\r\n{}", dir, sdpBody.length(), sdpBody);

            auto bs = rawMsg.begin();
            sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

            EXPECT_EQ("application/sdp", sipm.getContentType());
            EXPECT_TRUE(sipm.contains("/b/sdp/0"_json_pointer));
            const auto& sdpJson = sipm["/b/sdp/0"_json_pointer];
            EXPECT_TRUE(sdpJson.contains("a"));
        }
    }

    //-------------------------------------------------------------------------
    // SDP Compliance Group 3: WebRTC / ICE / DTLS Media Attributes
    //-------------------------------------------------------------------------
    TEST(SDP_Compliance, CERT_SDP_WebRTC_ICE_DTLS_Attributes)
    {
        // Tests ICE candidates, ice-ufrag, ice-pwd, DTLS fingerprint, and rtcp-mux
        std::string sdpBody =
            "v=0\r\n"
            "o=- 4652431718042462312 2 IN IP4 127.0.0.1\r\n"
            "s=-\r\n"
            "t=0 0\r\n"
            "a=ice-ufrag:f83c\r\n"
            "a=ice-pwd:secreticepwd123\r\n"
            "a=fingerprint:sha-256 4A:AD:B9:B1:3F:24:73:92:74:EC:37:A0:54:50:7B:9C:2C:9D:28:1A:67:8B:77:E5:6C:52:13:96:A2:F5:F4:79\r\n"
            "m=audio 9 UDP/TLS/RTP/SAVPF 111\r\n"
            "c=IN IP4 0.0.0.0\r\n"
            "a=rtcp-mux\r\n"
            "a=rtpmap:111 opus/48000/2\r\n"
            "a=candidate:1 1 UDP 2122260223 192.168.1.100 54321 typ host\r\n";

        std::string rawMsg = std::format(
            "INVITE sip:webrtc@example.com SIP/2.0\r\n"
            "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKwebrtc\r\n"
            "From: <sip:browser@example.com>;tag=1\r\n"
            "To: <sip:webrtc@example.com>\r\n"
            "Call-ID: sdp-webrtc-cert-123\r\n"
            "CSeq: 1 INVITE\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: {}\r\n\r\n{}", sdpBody.length(), sdpBody);

        auto bs = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("sdp-webrtc-cert-123", sipm.getCallID());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.contains("/b/sdp/0"_json_pointer));

        const auto& sdpJson = sipm["/b/sdp/0"_json_pointer];
        EXPECT_TRUE(sdpJson.contains("a"));
        EXPECT_TRUE(sdpJson.contains("m"));
    }

    //-------------------------------------------------------------------------
    // SDP Compliance Group 4: Multiple SDP Sessions (v=0 Demarcation)
    //-------------------------------------------------------------------------
    TEST(SDP_Compliance, CERT_SDP_Multiple_Sessions_Demarcation)
    {
        // Tests multiple SDP session blocks separated by v=0 lines
        std::string sdpBody =
            "v=0\r\n"
            "o=alice 100 100 IN IP4 192.0.2.1\r\n"
            "s=Session 1\r\n"
            "t=0 0\r\n"
            "m=audio 49170 RTP/AVP 0\r\n"
            "v=0\r\n"
            "o=alice 100 101 IN IP4 192.0.2.1\r\n"
            "s=Session 2\r\n"
            "t=0 0\r\n"
            "m=video 51372 RTP/AVP 99\r\n";

        std::string rawMsg = std::format(
            "INVITE sip:bob@example.com SIP/2.0\r\n"
            "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKmultsdp\r\n"
            "From: <sip:alice@example.com>;tag=1\r\n"
            "To: <sip:bob@example.com>\r\n"
            "Call-ID: sdp-multiple-sessions-cert\r\n"
            "CSeq: 1 INVITE\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: {}\r\n\r\n{}", sdpBody.length(), sdpBody);

        auto bs = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("sdp-multiple-sessions-cert", sipm.getCallID());
        EXPECT_TRUE(sipm.contains("/b/sdp/0"_json_pointer));
        EXPECT_TRUE(sipm.contains("/b/sdp/1"_json_pointer));
    }

    //-------------------------------------------------------------------------
    // SDP Compliance Group 5: Line Endings (CRLF vs LF)
    //-------------------------------------------------------------------------
    TEST(SDP_Compliance, CERT_SDP_UNIX_LF_LineEndings)
    {
        std::string sdpBody =
            "v=0\n"
            "o=alice 2890844526 2890844526 IN IP4 192.0.2.1\n"
            "s=SDP Unix LF Test\n"
            "c=IN IP4 192.0.2.1\n"
            "t=0 0\n"
            "m=audio 49170 RTP/AVP 0\n"
            "a=rtpmap:0 PCMU/8000\n";

        std::string rawMsg = std::format(
            "INVITE sip:bob@example.com SIP/2.0\n"
            "Via: SIP/2.0/UDP 192.0.2.1:5060;branch=z9hG4bKlf\n"
            "From: <sip:alice@example.com>;tag=1\n"
            "To: <sip:bob@example.com>\n"
            "Call-ID: sdp-unix-lf-cert\n"
            "CSeq: 1 INVITE\n"
            "Content-Type: application/sdp\n"
            "Content-Length: {}\n\n{}", sdpBody.length(), sdpBody);

        auto bs = rawMsg.begin();
        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());

        EXPECT_EQ("sdp-unix-lf-cert", sipm.getCallID());
        EXPECT_EQ("application/sdp", sipm.getContentType());
        EXPECT_TRUE(sipm.contains("/b/sdp/0"_json_pointer));
    }
}
