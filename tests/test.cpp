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
TEST(core_parser_tests, Test_UserAgent)
{
    using namespace siddiqsoft;

    auto                   ua = __func__; //NOLINT
    siddiqsoft::sipmessage sipm(METHOD_REGISTER, "sip:hello@world.com");

    try
    {
        sipm.setUserAgent(ua);
        std::cerr << sip2json::serialize(sipm);
        EXPECT_TRUE(sipm.getUserAgent().find(ua) != std::string::npos);
        EXPECT_TRUE(sipm.getUserAgent().find("sip2json") != std::string::npos);
    }
    catch (const std::exception& e)
    {
        EXPECT_TRUE(true) << L"Got exception. " << e.what();
    }
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_meta_element)
{
    siddiqsoft::sipmessage sipm(siddiqsoft::METHOD_REGISTER, "sip:hello@world.com");

    try
    {
        std::clog << siddiqsoft::sip2json::serialize(sipm);
        EXPECT_TRUE(sipm.contains("meta"));
    }
    catch (const std::exception& e)
    {
        EXPECT_TRUE(true) << L"Got exception. " << e.what();
    }
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_sip2jsonErrors)
{
    auto ee = siddiqsoft::sip2jsonErrors::ok;

    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::empty_message;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::incomplete_buffer_for_content;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::incomplete_buffer_for_header;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::incomplete_buffer_for_parse;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_document;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_document_unsupported_content;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_document_unsupported_method;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());

    ee = siddiqsoft::sip2jsonErrors::invalid_startline;
    std::clog << std::format("{} - {} --> {}\n", __func__, uint32_t(ee), nlohmann::json(ee).dump());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_createCallId)
{
    auto ci = siddiqsoft::createCallId();
    EXPECT_TRUE(ci.length() == 44);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC1123)
{
    auto todays_date = siddiqsoft::TimeAsRFC1123();
    EXPECT_TRUE(!todays_date.empty());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC1123_args)
{
    tm knowntm {};
    knowntm.tm_year  = 2010 - 1900;
    knowntm.tm_mon   = 11 - 1; // Nov
    knowntm.tm_mday  = 13;     // 13th
    knowntm.tm_hour  = 23;     // 23h
    knowntm.tm_min   = 29;     // 29m
    knowntm.tm_sec   = 0;      // 0s
    knowntm.tm_wday  = 6;      // Sat
    knowntm.tm_isdst = 0;

// When using mktime it screws up the conversion by always applying the local time to convert to UTC.
// So if we have UTC already, it will adjust this time with the timezone difference on this computer!
// Use _mkgmtime() on windows and timegm() on linux/macos
#ifdef _WINDOWS
    time_t tv1 {};
    tv1 = ::_mkgmtime64(&knowntm);
#else
    auto tv1 = ::timegm(&knowntm);
#endif

    char knowntmRepresentation[128] {'\0'};
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%A %c", &knowntm);
    std::clog << "     %A %c: " << knowntmRepresentation << std::endl;
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%FT%TZ", &knowntm);
    std::clog << "  strftime: " << knowntmRepresentation << std::endl;
    std::clog << "     ctime: " << ctime(&tv1) << std::endl;

    auto todays_date = siddiqsoft::TimeAsRFC1123(std::chrono::system_clock::from_time_t(tv1));
    EXPECT_EQ("Sat, 13 Nov 2010 23:29:00 GMT", todays_date);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsRFC3339_args)
{
    tm knowntm {};
    knowntm.tm_year  = 2010 - 1900;
    knowntm.tm_mon   = 11 - 1; // Nov
    knowntm.tm_mday  = 13;     // 13th
    knowntm.tm_hour  = 23;     // 23h
    knowntm.tm_min   = 29;     // 29m
    knowntm.tm_sec   = 0;      // 0s
    knowntm.tm_wday  = 6;      // Sat
    knowntm.tm_isdst = 0;

// When using mktime it screws up the conversion by always applying the local time to convert to UTC.
// So if we have UTC already, it will adjust this time with the timezone difference on this computer!
// Use _mkgmtime() on windows and timegm() on linux/macos
#ifdef _WINDOWS
    time_t tv1 {};
    tv1 = ::_mkgmtime64(&knowntm);
#else
    auto tv1 = ::timegm(&knowntm);
#endif

    char knowntmRepresentation[128] {'\0'};
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%A %c", &knowntm);
    std::clog << "     %A %c: " << knowntmRepresentation << std::endl;
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%FT%TZ", &knowntm);
    std::clog << "  strftime: " << knowntmRepresentation << std::endl;
    std::clog << "     ctime: " << ctime(&tv1) << std::endl;

    auto todays_date = siddiqsoft::TimeAsRFC3339(std::chrono::system_clock::from_time_t(tv1));
    EXPECT_EQ("2010-11-13T23:29:00.000Z", todays_date);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601)
{
    auto todays_date = siddiqsoft::TimeAsISO8601();
    EXPECT_TRUE(!todays_date.empty());
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601_args)
{
    tm knowntm {};
    knowntm.tm_year  = 2010 - 1900;
    knowntm.tm_mon   = 11 - 1; // Nov
    knowntm.tm_mday  = 13;     // 13th
    knowntm.tm_hour  = 23;     // 23h
    knowntm.tm_min   = 29;     // 29m
    knowntm.tm_sec   = 59;     // 59s
    knowntm.tm_wday  = 6;      // Sat
    knowntm.tm_isdst = 0;

// When using mktime it screws up the conversion by always applying the local time to convert to UTC.
// So if we have UTC already, it will adjust this time with the timezone difference on this computer!
// Use _mkgmtime() on windows and timegm() on linux/macos
#ifdef _WINDOWS
    time_t tv1 {};
    tv1 = ::_mkgmtime64(&knowntm);
#else
    auto tv1 = ::timegm(&knowntm);
#endif

    // When we convert, the resulting epoch should match!
    EXPECT_EQ(tv1, 1289690999L) << "tv0: " << tv1 << std::endl;

    char knowntmRepresentation[128] {'\0'};
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%A %c", &knowntm);
    std::clog << "     %A %c: " << knowntmRepresentation << std::endl;
    std::strftime(knowntmRepresentation, sizeof(knowntmRepresentation), "%FT%TZ", &knowntm);
    std::clog << "  strftime: " << knowntmRepresentation << std::endl;
    std::clog << "     ctime: " << ctime(&tv1) << std::endl;

    auto knownDate = siddiqsoft::TimeAsISO8601(std::chrono::system_clock::from_time_t(tv1));
    EXPECT_EQ("2010-11-13T23:29:59.000Z", knownDate);
}

// NOLINTNEXTLINE
TEST(core_parser_tests, Test_TimeAsISO8601_epoch)
{
    auto timeAsEpoch {1289690999L}; // Corresponds to Saturday, November 13, 2010 11:29:59 PM

    auto knownDate = siddiqsoft::TimeAsISO8601(std::chrono::system_clock::from_time_t(timeAsEpoch));
    EXPECT_EQ("2010-11-13T23:29:59.000Z", knownDate);
}


// NOLINTNEXTLINE
TEST(ImplementationChecks, Test_stream_serializer_1)
{
    std::stringstream sstr {};

    // This should compile without issue.
    sstr << __func__ << " format " << siddiqsoft::SIPMessageType::request << " and " << siddiqsoft::SIPMessageType::response
         << " checks out." << std::endl;

    auto msg = sstr.str();

    EXPECT_TRUE(msg.find("request") != std::string::npos);
    EXPECT_TRUE(msg.find("response") != std::string::npos);

    std::cerr << msg << std::endl;
}

// NOLINTNEXTLINE
TEST(ImplementationChecks, Test_formatters_1)
{
    // This should compile without issue.
    auto msg0 = std::format("{} format ", __func__);
    auto msg1 = std::format("{} and ", siddiqsoft::SIPMessageType::request);
    auto msg2 = std::format("{} checks out.", siddiqsoft::SIPMessageType::response);
    auto msg  = msg0.append(msg1).append(msg2);

    EXPECT_TRUE(msg.find("request") != std::string::npos) << " .. expected `request` in the string : " << msg;
    EXPECT_TRUE(msg.find("response") != std::string::npos) << " .. expected `response` in the string: " << msg;

    std::cerr << "msg0 : " << msg0 << std::endl;
    std::cerr << "msg1 : " << msg1 << std::endl;
    std::cerr << "msg2 : " << msg2 << std::endl;
    std::cerr << "msg  : " << msg << std::endl;
}
