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


// NOLINTNEXTLINE
TEST(synthetics, Check_async_invalid_startline_CRLF)
{
    std::string buffer {
            "\r\npreceeding junk\r\nNOTiFY "
            "sip:lab.mediaserver.rc.116.voip@mediaco.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.mediaco-corp.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP "
            "localhost:780;branch=conference.wizard@local.host__mediaserver_sep__lab.mediaserver.rc.116.voip@mediaco.co;received=127.0.0.1:"
            "48114:442357920\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;branch=something@somewhere.host;received=127.0.0.1:32900:215271\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21111111\r\n"
            "Content-Length: 0\r\n"
            "Content-type: application/sdp\r\n"
            "\r\n"};
    bool passTest = false;
    auto bs       = buffer.begin();

    auto remainingBuffer = siddiqsoft::sip2json::parseAsync(
            buffer,
            {},
            [&](const siddiqsoft::sip2json_exception& e, std::string::iterator&, const std::string::iterator&)
            {
                std::cerr << "errCode: " << e.errCode << " what: " << e.what() << std::endl;
                EXPECT_TRUE(e.errCode == siddiqsoft::sip2jsonErrors::invalid_startline);
                passTest = true;
            });
    EXPECT_TRUE(passTest);
}

// NOLINTNEXTLINE
TEST(synthetics, Check_header_array_CRLF)
{
    std::string buffer {
            "NOTIFY sip:lab.mediaserver.rc.116.voip@mediaco.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.mediaco-corp.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP "
            "localhost:780;branch=conference.wizard@local.host__mediaserver_sep__lab.mediaserver.rc.116.voip@mediaco.co;received=127.0.0.1:"
            "48114:442357920\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;branch=something@somewhere.host;received=127.0.0.1:32900:215271\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21111111\r\n"
            "Content-Length: 0\r\n"
            "Content-type: application/sdp\r\n"
            "\r\n"};
    auto                   bs         = buffer.begin();
    auto                   parseCount = 0;
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.contains("/h/Via"_json_pointer));
    {
        auto via = sipm["h"]["Via"];

        std::cerr << "Log the parseFromBuffer output: " << sipm.dump(1) << std::endl;
        EXPECT_TRUE(via.is_array()) << via.dump(1);

        if (via.is_string())
        {
            // swap out an push to array
            auto previous = via.get<std::string>();
            sipm["h"].erase("Via");
            sipm["h"]["Via"].push_back(previous);
            sipm["h"]["Via"].push_back(std::format("SIP/2.0/TCP {}", "x@y.z"));
            EXPECT_EQ(sipm["h"]["Via"].get<std::vector<std::string>>().size(), 4) << sipm["h"]["Via"].dump();
            auto elemJustAdded = sipm["h"]["Via"].get<std::vector<std::string>>()[1];
            EXPECT_TRUE(elemJustAdded.find("x@y.z") != std::string::npos) << sipm["h"]["Via"].dump();
        }
        else if (via.is_array())
        {
            // Add another element..
            sipm["h"]["Via"].push_back(std::format("SIP/2.0/TCP {}", "a@b.c"));
            EXPECT_EQ(sipm["h"]["Via"].get<std::vector<std::string>>().size(), 5) << sipm["h"]["Via"].dump();
        }
    }
}

/*
TEST(synthetics, Check_header_array_LF)
{
    std::string buffer {
            "NOTIFY sip:lab.mediaserver.rc.116.voip@mediaco.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.mediaco-corp.com SIP/2.0\n"
            "Via: SIP/2.0/TCP "
            "localhost:780;branch=conference.wizard@local.host__mediaserver_sep__lab.mediaserver.rc.116.voip@mediaco.co;received=127.0.0.1:"
            "48114:442357920\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;branch=something@somewhere.host;received=127.0.0.1:32900:215271\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21010108\n"
            "Content-Length: 0\n"
            "Content-type: application/sdp\n"
            "\n"};
    auto                   bs         = buffer.begin();
    auto                   parseCount = 0;
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
    EXPECT_TRUE(sipm.contains("/h/Via"_json_pointer));
    {
        auto via = sipm["h"]["Via"];

        std::cerr << "Log the parseFromBuffer output: " << sipm.dump(1) << std::endl;
        EXPECT_TRUE(via.is_array()) << via.dump(1);

        if (via.is_string())
        {
            // swap out an push to array
            auto previous = via.get<std::string>();
            sipm["h"].erase("Via");
            sipm["h"]["Via"].push_back(previous);
            sipm["h"]["Via"].push_back(std::format("SIP/2.0/TCP {}", "x@y.z"));
            EXPECT_EQ(sipm["h"]["Via"].get<std::vector<std::string>>().size(), 4) << sipm["h"]["Via"].dump();
            auto elemJustAdded = sipm["h"]["Via"].get<std::vector<std::string>>()[1];
            EXPECT_TRUE(elemJustAdded.find("x@y.z") != std::string::npos) << sipm["h"]["Via"].dump();
        }
        else if (via.is_array())
        {
            // Add another element..
            sipm["h"]["Via"].push_back(std::format("SIP/2.0/TCP {}", "a@b.c"));
            EXPECT_EQ(sipm["h"]["Via"].get<std::vector<std::string>>().size(), 5) << sipm["h"]["Via"].dump();
        }
    }
}
*/

TEST(synthetics, Check_startline_precedingjunk_CRLF)
{
    std::string buffer {
            "2024-11-25T11:11:00.000Z Message\r\nNOTIFY "
            "sip:lab.mediaserver.rc.116.voip@mediaco.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.mediaco-corp.com SIP/2.0\r\n"
            "Via: SIP/2.0/TCP "
            "localhost:780;branch=conference.wizard@local.host__mediaserver_sep__lab.mediaserver.rc.116.voip@mediaco.co;received=127.0.0.1:"
            "48114:442357920\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;branch=something@somewhere.host;received=127.0.0.1:32900:215271\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168\r\n"
            "Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21111111\r\n"
            "Content-Length: 0\r\n"
            "Content-type: application/sdp\r\n"
            "\r\n"};
    auto                   bs         = buffer.begin();
    auto                   parseCount = 0;
    siddiqsoft::sipmessage sipm;

    EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));

    EXPECT_TRUE(sipm.contains("/h/Via"_json_pointer));
    {
        auto via = sipm["h"]["Via"];

        std::cerr << "Log the parseFromBuffer output: " << sipm.dump(1) << std::endl;
        EXPECT_TRUE(via.is_array()) << via.dump(1);

        if (via.is_string())
        {
            // swap out an push to array
            auto previous = via.get<std::string>();
            sipm["h"].erase("Via");
            sipm["h"]["Via"].push_back(previous);
            sipm["h"]["Via"].push_back(std::format("SIP/2.0/TCP {}", "x@y.z"));
            EXPECT_EQ(sipm["h"]["Via"].get<std::vector<std::string>>().size(), 4) << sipm["h"]["Via"].dump();
            auto elemJustAdded = sipm["h"]["Via"].get<std::vector<std::string>>()[1];
            EXPECT_TRUE(elemJustAdded.find("x@y.z") != std::string::npos) << sipm["h"]["Via"].dump();
        }
        else if (via.is_array())
        {
            // Add another element..
            sipm["h"]["Via"].push_back(std::format("SIP/2.0/TCP {}", "a@b.c"));
            EXPECT_EQ(sipm["h"]["Via"].get<std::vector<std::string>>().size(), 5) << sipm["h"]["Via"].dump();
        }
    }
}
