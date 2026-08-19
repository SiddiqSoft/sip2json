#include <gtest/gtest.h>
#include "../include/siddiqsoft/sip2json.hpp"

TEST(security_tests, Test_negative_content_length_throws)
{
    std::string msg = "INVITE sip:user@example.com SIP/2.0\r\n"
                      "Via: SIP/2.0/TCP localhost;branch=z9hG4bK-12345\r\n"
                      "From: <sip:caller@example.com>;tag=123\r\n"
                      "To: <sip:user@example.com>\r\n"
                      "Call-ID: test1234567890-call-id-string\r\n"
                      "CSeq: 1 INVITE\r\n"
                      "Contact: <sip:caller@example.com>\r\n"
                      "Content-Length: -1\r\n\r\n";
    auto start = msg.begin();
    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(start, msg.end()), siddiqsoft::invalid_document_error);
}

TEST(security_tests, Test_negative_expires_throws)
{
    std::string msg = "REGISTER sip:example.com SIP/2.0\r\n"
                      "Via: SIP/2.0/TCP localhost;branch=z9hG4bK-12345\r\n"
                      "From: <sip:caller@example.com>;tag=123\r\n"
                      "To: <sip:user@example.com>\r\n"
                      "Call-ID: test1244567890-call-id-string\r\n"
                      "CSeq: 1 REGISTER\r\n"
                      "Contact: <sip:caller@example.com>\r\n"
                      "Expires: -100\r\n\r\n";
    auto start = msg.begin();
    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(start, msg.end()), siddiqsoft::invalid_document_error);
}

TEST(security_tests, Test_sdp_without_v_zero_throws)
{
    std::string msg = "INVITE sip:user@example.com SIP/2.0\r\n"
                      "Via: SIP/2.0/TCP localhost;branch=z9hG4bK-12345\r\n"
                      "From: <sip:caller@example.com>;tag=123\r\n"
                      "To: <sip:user@example.com>\r\n"
                      "Call-ID: test1254567890-call-id-string\r\n"
                      "CSeq: 1 INVITE\r\n"
                      "Contact: <sip:caller@example.com>\r\n"
                      "Content-Type: application/sdp\r\n"
                      "Content-Length: 26\r\n\r\n"
                      "c=IN IP4 192.168.1.1\r\n"
                      "m=audio\r\n";
    auto start = msg.begin();
    EXPECT_THROW(siddiqsoft::sip2json::parseFromBuffer(start, msg.end()), siddiqsoft::invalid_document_error);
}

TEST(security_tests, Test_sdp_json_pointer_escaping)
{
    std::string msg = "INVITE sip:user@example.com SIP/2.0\r\n"
                      "Via: SIP/2.0/TCP localhost;branch=z9hG4bK-12345\r\n"
                      "From: <sip:caller@example.com>;tag=123\r\n"
                      "To: <sip:user@example.com>\r\n"
                      "Call-ID: test1264567890-call-id-string\r\n"
                      "CSeq: 1 INVITE\r\n"
                      "Contact: <sip:caller@example.com>\r\n"
                      "Content-Type: application/sdp\r\n"
                      "Content-Length: 32\r\n\r\n"
                      "v=0\r\n"
                      "a=custom/nested/key:value\r\n";
    auto start = msg.begin();
    siddiqsoft::sipmessage parsed;
    EXPECT_NO_THROW(parsed = siddiqsoft::sip2json::parseFromBuffer(start, msg.end()));
    EXPECT_TRUE(parsed.contains("b"));
    EXPECT_TRUE(parsed["b"].contains("sdp"));
    EXPECT_TRUE(parsed["b"]["sdp"][0]["a"].contains("custom/nested/key"));
    EXPECT_EQ("value", parsed["b"]["sdp"][0]["a"]["custom/nested/key"]);
}

TEST(security_tests, Test_serialize_exact_method_validation)
{
    siddiqsoft::sipmessage msg("E", "sip:user@example.com");
    msg.setHeader("Via", "SIP/2.0/TCP localhost");
    EXPECT_THROW(siddiqsoft::sip2json::serialize(msg), siddiqsoft::invalid_document_error);
}

TEST(security_tests, Test_serialize_crlf_injection_in_uri_throws)
{
    siddiqsoft::sipmessage msg("INVITE", "sip:user@example.com\r\nX-Injected: evil");
    msg.setHeader("Via", "SIP/2.0/TCP localhost");
    EXPECT_THROW(siddiqsoft::sip2json::serialize(msg), siddiqsoft::invalid_document_error);
}

TEST(security_tests, Test_serialize_crlf_injection_in_header_throws)
{
    siddiqsoft::sipmessage msg("INVITE", "sip:user@example.com");
    msg.setHeader("User-Agent", "sip2json\r\nInjected-Header: evil");
    EXPECT_THROW(siddiqsoft::sip2json::serialize(msg), siddiqsoft::invalid_document_error);
}
