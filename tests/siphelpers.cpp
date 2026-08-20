/*
  A SIP Parser for Modern C++ / Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
*/

#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string_view>
#include <filesystem>

#include <iomanip>
#include <sstream>

#include <format>
#include "nlohmann/json.hpp"

#include "../include/siddiqsoft/sip2json.hpp"


#include "gtest/gtest.h"
#include <thread>

#if defined(_WIN32)
#include <Windows.h>
#include <processenv.h>
#else
#include <unistd.h>
#endif


TEST(siphelpers, Test_createRequest)
{
    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);
    auto                   diagContents = registerMessage.flatten().dump(2);

    EXPECT_TRUE(registerMessage.size() != 0);
    EXPECT_TRUE(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(!registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
    EXPECT_TRUE(registerMessage.isMessageRequest());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_createResponse)
{
    siddiqsoft::sipmessage dummyMessage(500);
    auto                   diagContents = dummyMessage.flatten().dump(2);

    EXPECT_TRUE(dummyMessage.size() != 0);
    EXPECT_TRUE(!dummyMessage.value("/s/reason"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(dummyMessage.isMessageResponse());
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_createRequest_then_response)
{
    using namespace nlohmann::json_literals;

    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    EXPECT_TRUE(!registerMessage.value("/h/Date"_json_pointer, std::string {}).empty());

    registerMessage["/h/To"_json_pointer]      = "sip:hello@world.com";
    registerMessage["/h/Contact"_json_pointer] = "sip:hello@world.com";

    EXPECT_TRUE(registerMessage.size() != 0);
    EXPECT_TRUE(registerMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
    EXPECT_EQ(siddiqsoft::SIPMessageType::request,
              registerMessage.value("/s/type"_json_pointer, siddiqsoft::SIPMessageType::notspecified));

    // WARNING
    // As we're passing the registerMessage as parameter to create an inplace response message
    // the original registerMessage object will be clobbered with the items from the
    // response message create function.
    siddiqsoft::sipmessage responseMessage(200, registerMessage);

    EXPECT_TRUE(responseMessage.size() != 0);
    EXPECT_TRUE(responseMessage.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
    EXPECT_EQ(siddiqsoft::SIPMessageType::response,
              responseMessage.value("/s/type"_json_pointer, siddiqsoft::SIPMessageType::notspecified));
    EXPECT_TRUE(!responseMessage.value("/h/Date"_json_pointer, std::string {}).empty());

    EXPECT_EQ(registerMessage.value("/h/Call-ID"_json_pointer, "req"), responseMessage.value("/h/Call-ID"_json_pointer, "resp"));
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_serialize)
{
    auto ll       = __LINE__;
    auto myCallId = siddiqsoft::createCallId();
    ll            = __LINE__;
    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", myCallId, 1);

    ll = __LINE__;
    registerMessage.setHeader("To", "sip:hello@world.com").setHeader("Contact", "sip:hello@world.com");

    try
    {
        ll           = __LINE__;
        auto strsipm = siddiqsoft::sip2json::serialize(registerMessage);

        ll = __LINE__;
        EXPECT_TRUE(strsipm.length() != 0);

        ll                           = __LINE__;
        auto bufferStart             = strsipm.begin();
        ll                           = __LINE__;
        siddiqsoft::sipmessage sipm2 = siddiqsoft::sip2json::parseFromBuffer(bufferStart, strsipm.end());
        EXPECT_TRUE(!sipm2.empty());
        EXPECT_EQ(registerMessage.getContentLength(), sipm2.getContentLength());
        EXPECT_EQ(registerMessage.getCallID(), sipm2.getCallID());

        std::clog << "\n============vvv=\n";
        std::clog << strsipm;
        std::clog << "\n============   =\n";
        std::clog << siddiqsoft::sip2json::serialize(sipm2);
        std::clog << "\n============^^^=\n";

        EXPECT_EQ(strsipm.length(), siddiqsoft::sip2json::serialize(sipm2).length());
    }
    catch (const std::exception& e)
    {
        std::clog << std::format("{}:Exception lastline:{} --> {}\n", __func__, ll, e.what());
        EXPECT_TRUE(true) << L"Unexpected exception.";
    }
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_serialize_empty_mb_fail)
{
    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    registerMessage.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Type", "application/dummy");
    // This will cause serialize to throw!
    registerMessage["b"] = 0;
    EXPECT_THROW(siddiqsoft::sip2json::serialize(registerMessage), std::exception);
}


TEST(siphelpers, Test_serialize_empty_mb_fail_2)
{
    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    registerMessage.setHeader(
            {{"To", "sip:hello@world.com"}, {"Contact", "sip:hello@world.com"}, {"Content-Type", "application/dummy"}});
    // This will cause serialize to throw!
    registerMessage["b"] = 0;
    EXPECT_THROW(siddiqsoft::sip2json::serialize(registerMessage), std::exception);
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_serialize_empty_mb_valid)
{
    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    registerMessage.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP);
    // Should not throw; body is null despite the header being SDP there is no body element set.
    // This is a supported use-case
    EXPECT_NO_THROW(siddiqsoft::sip2json::serialize(registerMessage));
}


TEST(siphelpers, Test_serialize_empty_mb_valid_2)
{
    siddiqsoft::sipmessage registerMessage("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    registerMessage.setHeader({{"To", "sip:hello@world.com"},
                               {"Contact", "sip:hello@world.com"},
                               {"Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP}});
    // Should not throw; body is null despite the header being SDP there is no body element set.
    // This is a supported use-case
    EXPECT_NO_THROW(siddiqsoft::sip2json::serialize(registerMessage));
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_incomplete_buffer_for_parse)
{
    EXPECT_THROW(
            []()
            {
                auto buffer = siddiqsoft::SIP_SAMPLE_MINIMAL_MESSAGE;
                auto bs     = buffer.begin();
                auto dummy  = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
            }(),
            siddiqsoft::incomplete_buffer_for_parse_error)
            << L"Expect exception: incomplete_buffer_for_parse\n";
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_document)
{
    siddiqsoft::sipmessage sipm;
    sipm["dummy"] = "world";
    // We should expect the invalid_document_error trying to serialize this invalid message.
    EXPECT_THROW([&]() { siddiqsoft::sip2json::serialize(sipm); }(), siddiqsoft::invalid_document_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_invalid_document_startline)
{
    EXPECT_THROW(
            []()
            {
                siddiqsoft::sipmessage sipm("ROR", "sip:dummy@world.com");
                siddiqsoft::sip2json::serialize(sipm);
            }(),
            siddiqsoft::invalid_document_error);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_mb)
{
    using namespace nlohmann::json_literals;

    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP);

    // By default the body is null
    EXPECT_TRUE(sipm.body().empty());

    // Force an error by setting the body to something non-SDP
    sipm.body() = "<root></root>";
    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);

    // Reset the invalid body so we can set it to SDP and recheck
    sipm.body() = nullptr; // don't erase()
    // Set some dummy value..
    sipm.setBody("/sdp/0/v"_json_pointer, 0)
            .setBody("/sdp/0/s"_json_pointer, "subject")
            .setBody("/sdp/0/a/access_code"_json_pointer, "0277777")
            .setBody("/sdp/0/t"_json_pointer, nlohmann::json {100001, 200002});

    // Check again for the body. it should be non-null
    EXPECT_TRUE(sipm.body().is_object());

    EXPECT_EQ(0, sipm.body()["sdp"][0]["v"].get<int>());
    EXPECT_EQ("subject", sipm.body()["sdp"][0]["s"].get<std::string>());
    EXPECT_EQ(100001, sipm.body()["sdp"][0]["t"][0].get<int>());
    EXPECT_EQ(200002, sipm.body()["sdp"][0]["t"][1].get<int>());

    sipm.erase("b");
    EXPECT_FALSE(sipm.hasBody()) << sipm.dump(2);

    sipm.setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_TEXT_PLAIN);
    EXPECT_EQ(siddiqsoft::CONTENT_TYPE_TEXT_PLAIN, sipm.getHeader<std::string>(siddiqsoft::HFS_CONTENT_TYPE[1], "unknown"))
            << sipm.dump(2);
}


TEST(siphelpers, Test_empty_mb_2)
{
    using namespace nlohmann::json_literals;

    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader({{"To", "sip:hello@world.com"},
                    {"Contact", "sip:hello@world.com"},
                    {"Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP}});

    // By default the body is null
    EXPECT_TRUE(sipm.body().empty());

    // Force an error by setting the body to something non-SDP
    sipm.body() = "<root></root>";
    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);

    // Reset the invalid body so we can set it to SDP and recheck
    sipm.body() = nullptr; // don't erase()
    // Set some dummy value..
    sipm.setBody({{"sdp",
                   {{{"v", 0},
                     {"s", "subject"},
                     {"t", {100001, 200002}},
                     {"a", {{"server", "media-server"}, {"access_code", "0277777"}}}},

                    {{"v", 0},
                     {"s", "subject-2"},
                     {"t", {133331, 233332}},
                     {"a", {{"server", "media-server-2"}, {"access_code", "2777777"}}}}}}});

    // Check again for the body. it should be non-null
    EXPECT_TRUE(sipm.body().is_object()) << sipm.dump(2);

    //EXPECT_TRUE(false) << sipm.body().dump(2);
    EXPECT_EQ(2, sipm.body().at("sdp").size());

    // Check first sdp
    EXPECT_EQ(0, sipm.getBodyElement("/sdp/0/v"_json_pointer, 99)) << sipm.body().dump(2);
    EXPECT_EQ("subject", sipm.getBodyElement<std::string>("/sdp/0/s"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ(100001, sipm.getBodyElement("/sdp/0/t/0"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ(200002, sipm.getBodyElement("/sdp/0/t/1"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ("media-server", sipm.getBodyElement<std::string>("/sdp/0/a/server"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ("0277777", sipm.getBodyElement<std::string>("/sdp/0/a/access_code"_json_pointer, "")) << sipm.body().dump(2);

    // Check the second sdp
    EXPECT_EQ(0, sipm.getBodyElement("/sdp/1/v"_json_pointer, 99)) << sipm.body().dump(2);
    EXPECT_EQ("subject-2", sipm.getBodyElement<std::string>("/sdp/1/s"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ(133331, sipm.getBodyElement("/sdp/1/t/0"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ(233332, sipm.getBodyElement("/sdp/1/t/1"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ("media-server-2", sipm.getBodyElement<std::string>("/sdp/1/a/server"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ("2777777", sipm.getBodyElement<std::string>("/sdp/1/a/access_code"_json_pointer, "")) << sipm.body().dump(2);


    sipm.erase("b");
    EXPECT_FALSE(sipm.hasBody()) << sipm.dump(2);

    sipm.setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_TEXT_PLAIN);
    EXPECT_EQ(siddiqsoft::CONTENT_TYPE_TEXT_PLAIN, sipm.getHeader<std::string>(siddiqsoft::HFS_CONTENT_TYPE[1], "unknown"))
            << sipm.dump(2);
}


TEST(siphelpers, Test_empty_mb_3)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP);

    // By default the body is null
    EXPECT_TRUE(sipm.body().empty());

    // Force an error by setting the body to something non-SDP
    sipm.body() = "<root></root>";
    EXPECT_THROW(siddiqsoft::sip2json::serialize(sipm), siddiqsoft::invalid_document_error);

    // Reset the invalid body so we can set it to SDP and recheck
    sipm.body() = nullptr; // don't erase()
    // Set some dummy value..
    sipm.setBody("/sdp/0/v"_json_pointer, 0)
            .setBody("/sdp/0/s"_json_pointer, "sssssss")
            .setBody("/sdp/0/a/access_code"_json_pointer, "0000000")
            .setBody("/sdp/0/a/clir"_json_pointer, "false")
            .setBody("/sdp/0/t"_json_pointer, nlohmann::json {999999, 999999});

    // Check again for the body. it should be non-null
    EXPECT_TRUE(sipm.body().is_object()) << sipm.dump(2);

    // Check first sdp
    EXPECT_EQ(0, sipm.getBodyElement("/sdp/0/v"_json_pointer, 99)) << sipm.body().dump(2);
    EXPECT_EQ("sssssss", sipm.getBodyElement<std::string>("/sdp/0/s"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ(999999, sipm.getBodyElement("/sdp/0/t/0"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ(999999, sipm.getBodyElement("/sdp/0/t/1"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ("0000000", sipm.getBodyElement<std::string>("/sdp/0/a/access_code"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ("false", sipm.getBodyElement<std::string>("/sdp/0/a/clir"_json_pointer, "")) << sipm.body().dump(2);

    sipm.setBody({{"sdp",
                   {
                           {{"v", 0},
                            {"s", "subject"},
                            {"t", {100001, 200002}},
                            {"a", {{"server", "media-server"}, {"access_code", "0277777"}}}},
                   }}});

    // Check again for the body. it should be non-null
    EXPECT_TRUE(sipm.body().is_object()) << sipm.dump(2);

    //EXPECT_TRUE(false) << sipm.body().dump(2);
    EXPECT_EQ(1, sipm.body().at("sdp").size());

    // Check first sdp
    EXPECT_EQ(0, sipm.getBodyElement("/sdp/0/v"_json_pointer, 99)) << sipm.body().dump(2);
    EXPECT_EQ("subject", sipm.getBodyElement<std::string>("/sdp/0/s"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ(100001, sipm.getBodyElement("/sdp/0/t/0"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ(200002, sipm.getBodyElement("/sdp/0/t/1"_json_pointer, 0)) << sipm.body().dump(2);
    EXPECT_EQ("media-server", sipm.getBodyElement<std::string>("/sdp/0/a/server"_json_pointer, "")) << sipm.body().dump(2);
    EXPECT_EQ("0277777", sipm.getBodyElement<std::string>("/sdp/0/a/access_code"_json_pointer, "")) << sipm.body().dump(2);
    // The field is removed!
    EXPECT_EQ("", sipm.getBodyElement<std::string>("/sdp/0/a/clir"_json_pointer, "")) << sipm.body().dump(2);

    sipm.erase("b");
    EXPECT_FALSE(sipm.hasBody()) << sipm.dump(2);

    sipm.setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_TEXT_PLAIN);
    EXPECT_EQ(siddiqsoft::CONTENT_TYPE_TEXT_PLAIN, sipm.getHeader<std::string>(siddiqsoft::HFS_CONTENT_TYPE[1], "unknown"))
            << sipm.dump(2);
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_h)
{
    auto                   callId = siddiqsoft::createCallId();
    std::string            cSeq {};
    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", callId, 1);

    sipm.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_TEXT_PLAIN)
            .setHeader("Content-Length", 0);

    std::clog << std::format("{} - contents\n{}\n", __func__, siddiqsoft::sip2json::serialize(sipm));

    // Check that the header exists..
    EXPECT_EQ("sip:hello@world.com", sipm.getHeader<std::string>("To"));
    EXPECT_EQ("sip:hello@world.com", sipm.getHeader<std::string>("Contact"));
    EXPECT_EQ(siddiqsoft::CONTENT_TYPE_TEXT_PLAIN, sipm.getContentType());
    EXPECT_EQ(callId, sipm.getCallID());
    EXPECT_EQ(0, sipm.getContentLength());
    EXPECT_EQ("1 REGISTER", sipm.getHeader<std::string>("CSeq"));

    // Remove the header object
    sipm.headers().erase("To");
    sipm.headers().erase("From");
    sipm.headers().erase("Contact");

    std::clog << std::format("{} - contents\n{}\n", __func__, siddiqsoft::sip2json::serialize(sipm));

    EXPECT_FALSE(sipm.headers().contains("To"));
    EXPECT_FALSE(sipm.headers().contains("From"));
    EXPECT_FALSE(sipm.headers().contains("Contact"));
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_empty_message)
{
    siddiqsoft::sipmessage emptyMessage;

    try
    {
        siddiqsoft::sip2json::serialize(emptyMessage);

        ASSERT_FALSE(false) << L"Expect exception: empty_message\n";
    }
    catch (siddiqsoft::empty_message_error& e)
    {
        std::clog << e.what();
        EXPECT_TRUE(e.errCode == siddiqsoft::sip2jsonErrors::empty_message);
    }
    catch (std::exception& e)
    {
        std::clog << e.what();
        ASSERT_FALSE(false) << L"unknown/unhandled exception.";
    }
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_check_isMessageTypeRequest)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    EXPECT_TRUE(sipm.size() != 0);
    EXPECT_TRUE(!sipm.value("/h/Date"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(!sipm.value("/h/Call-ID"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(sipm.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
    EXPECT_TRUE(sipm.isMessageRequest());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_check_isMessageTypeResponse)
{
    siddiqsoft::sipmessage sipm(608);

    EXPECT_TRUE(sipm.size() != 0);
    EXPECT_TRUE(!sipm.value("/s/reason"_json_pointer, std::string {}).empty());
    EXPECT_EQ(siddiqsoft::SIPMessageType::response, sipm.value("/s/type"_json_pointer, siddiqsoft::SIPMessageType::notspecified));
    EXPECT_EQ(608, sipm.getStatusCode());
    EXPECT_TRUE(sipm.isMessageResponse());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_check_getContentType)
{
    siddiqsoft::sipmessage sipm("INVITE", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    EXPECT_TRUE(sipm.size() != 0);
    EXPECT_TRUE(!sipm.value("/h/Date"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(!sipm.value("/h/Call-ID"_json_pointer, std::string {}).empty());
    EXPECT_TRUE(sipm.value("/h/Call-ID"_json_pointer, std::string {}).length() == 44);
    EXPECT_TRUE(sipm.isMessageRequest());

    // Note the "Content-type" and "Content-Type"; either way the method should return the value.
    sipm["h"]["Content-type"] = "test/test";
    EXPECT_EQ("test/test", sipm.getContentType());

    sipm["h"]["Content-Type"] = "test/test2";
    EXPECT_EQ("test/test2", sipm.getContentType());

    sipm["h"].erase("Content-Type");
    sipm["h"].erase("Content-type");
    EXPECT_TRUE(sipm.getContentType().empty());
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_header_method)
{
    siddiqsoft::sipmessage sipm("REGISTER", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader("To", "sip:hello@world.com")
            .setHeader("Contact", "sip:hello@world.com")
            .setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP);

    EXPECT_EQ("sip:hello@world.com", sipm.getHeader<std::string>("Contact"));
    EXPECT_EQ(siddiqsoft::CONTENT_TYPE_APP_SDP, sipm.getContentType());
}

// NOLINTNEXTLINE
TEST(siphelpers, Test_body_method)
{
    std::string            iName {"MY_INAME"};
    siddiqsoft::sipmessage sipm("INVITE", "sip:hello@world.com", siddiqsoft::createCallId(), 1);

    sipm.setHeader("Content-Type", siddiqsoft::CONTENT_TYPE_APP_SDP);

    EXPECT_TRUE(sipm.body().empty());

    // Set dummy but all required values! v, 0, s, t, m
    sipm.setBody("/sdp/0/v"_json_pointer, 0)
            .setBody("/sdp/0/o"_json_pointer,
                     nlohmann::json {{"user", "sip:hello@world.com"},
                                     {"type", "IN"},
                                     {"subtype", "IP4"},
                                     {"host", "host.name.com"},
                                     {"t1", "900001"},  // these must be string
                                     {"t2", "900009"}}) // must be string
            .setBody("/sdp/0/s"_json_pointer, "subject")
            .setBody("/sdp/0/i/name"_json_pointer, iName)
            .setBody("/sdp/0/i/type"_json_pointer, "CallByPhone-URL")
            .setBody("/sdp/0/i/dn"_json_pointer, "16668661212")
            .setBody("/sdp/0/a/access_code"_json_pointer, "0277777")
            .setBody("/sdp/0/t"_json_pointer, nlohmann::json {100001, 200002})
            .setBody("/sdp/0/m"_json_pointer, "audio voice");

    // Check again for the body. it should be non-null
    EXPECT_TRUE(sipm.body().is_object());

    EXPECT_EQ(iName, sipm.body()["sdp"][0]["i"]["name"].get<std::string>());
    EXPECT_EQ(0, sipm.body()["sdp"][0]["v"].get<int>());
    EXPECT_EQ("subject", sipm.body()["sdp"][0]["s"].get<std::string>());
    EXPECT_EQ(100001, sipm.body()["sdp"][0]["t"][0].get<int>());
    EXPECT_EQ(200002, sipm.body()["sdp"][0]["t"][1].get<int>());

    try
    {
        auto buffer = siddiqsoft::sip2json::serialize(sipm);

        std::cerr << "Serialized:\n" << buffer << "\n";

        // Now that we have a serialized message
        // We will attempt to parse it back into a new message.
        // The callback will be invoked and this is where we will check
        // for our correctness.
        buffer = siddiqsoft::sip2json::parseAsync(buffer,
                                                  [&](auto&& dsipm)
                                                  {
                                                      // We Should check for the iline
                                                      EXPECT_EQ(iName, dsipm.body().value("/sdp/0/i/name"_json_pointer, ""))
                                                              << dsipm.dump(2);

                                                      int expected_v = dsipm.body()["sdp"][0]["v"].template get<int>();
                                                      EXPECT_EQ(0, expected_v);
                                                      std::string expected_s =
                                                              dsipm.body()["sdp"][0]["s"].template get<std::string>();
                                                      EXPECT_EQ("subject", expected_s);
                                                      int expected_t0 = dsipm.body()["sdp"][0]["t"][0].template get<int>();
                                                      EXPECT_EQ(100001, expected_t0);
                                                      int expected_t1 = dsipm.body()["sdp"][0]["t"][1].template get<int>();
                                                      EXPECT_EQ(200002, expected_t1);
                                                  });
        // All the message buffer should be consumed with no lef-overs.
        EXPECT_EQ(0, buffer.length()) << buffer;
    }
    catch (const std::exception& e)
    {
        std::clog << std::format("{}:Exception: {}\n", __func__, e.what());
        EXPECT_FALSE(false) << L"Unexpected exception.";
    }
}


// NOLINTNEXTLINE
TEST(siphelpers, Test_async_incomplete_buffer_for_parse)
{
    auto buffer   = siddiqsoft::SIP_SAMPLE_MINIMAL_MESSAGE;
    bool passTest = false;
    auto bs       = buffer.begin();

    auto remainingBuffer = siddiqsoft::sip2json::parseAsync(
            buffer,
            {},
            [&](const siddiqsoft::sip2json_exception& e, std::string::iterator&, const std::string::iterator&)
            {
                EXPECT_TRUE(e.errCode == siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse);
                passTest = true;
            });
    EXPECT_TRUE(passTest);
}
