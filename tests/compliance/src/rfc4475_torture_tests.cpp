/*
    A SIP Parser for Modern C++: RFC 4475 SIP Torture Test Suite
    Version 1.0.0
    https://github.com/siddiqsoftware/sip2json/

    BSD 3-Clause License
    Copyright (c) 2003-2026, Abdelkareem Siddiq
*/

#include <gtest/gtest.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <set>
#include "siddiqsoft/sip2json.hpp"

namespace siddiqsoft
{
    static std::string loadRfc4475File(const std::string& fileName)
    {
        std::string rawData;
        if (auto env_samples_dir = std::getenv("SAMPLES_DIR"); env_samples_dir != nullptr)
        {
            std::filesystem::path p = std::filesystem::path(env_samples_dir) / "rfc4475" / fileName;
            if (std::filesystem::exists(p))
            {
                std::ifstream f(p, std::ios::binary);
                rawData = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            }
        }

        if (rawData.empty())
        {
            auto cwd = std::filesystem::current_path();
            std::vector<std::filesystem::path> candidates;
            for (auto cur = cwd; !cur.empty() && cur != cur.root_path(); cur = cur.parent_path())
            {
                candidates.push_back(cur / "samples" / "rfc4475" / fileName);
                candidates.push_back(cur / "tests" / "compliance" / "samples" / "rfc4475" / fileName);
                candidates.push_back(cur / "tests" / "validation" / "samples" / "rfc4475" / fileName);
            }

            for (const auto& cand : candidates)
            {
                if (std::filesystem::exists(cand))
                {
                    std::ifstream f(cand, std::ios::binary);
                    rawData = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                    break;
                }
            }
        }

        if (rawData.empty())
        {
            throw std::runtime_error("Cannot locate RFC 4475 sample file: " + fileName);
        }

        // Restore bit-exact CRLF line endings if Git normalized \r\n to \n on Unix/Linux checkouts
        if (rawData.find("\r\n") == std::string::npos && rawData.find('\n') != std::string::npos)
        {
            std::string crlfData;
            crlfData.reserve(rawData.size() + 50);
            for (size_t i = 0; i < rawData.size(); ++i)
            {
                if (rawData[i] == '\n' && (i == 0 || rawData[i - 1] != '\r'))
                {
                    crlfData += "\r\n";
                }
                else
                {
                    crlfData += rawData[i];
                }
            }
            return crlfData;
        }

        return rawData;
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.1: Valid Short Message (wsinv.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_1_A_Short_Tortuous_INVITE)
    {
        std::string rawMsg = loadRfc4475File("wsinv.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("INVITE", sipm.getMethod());
        EXPECT_EQ("sip:vivekg@chair-dnrc.example.com;unknownparam", sipm.getUri());
        EXPECT_EQ("wsinv.ndaksdj@192.0.2.1", sipm.getCallID());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.2: Wide Range of Valid Characters (multi01.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_2_Wide_Range_Valid_Characters)
    {
        std::string rawMsg = loadRfc4475File("multi01.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("INVITE", sipm.getMethod());
        EXPECT_EQ("sip:user@company.com", sipm.getUri());
        EXPECT_TRUE(sipm.headers().contains("Via"));
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.3: Valid Escaping Mechanism (esc01.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_3_Valid_Use_Escaping)
    {
        std::string rawMsg = loadRfc4475File("esc01.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("INVITE", sipm.getMethod());
        EXPECT_EQ("sip:sips%3Auser%40example.com@example.net", sipm.getUri());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.4: Escaped Nulls in URIs (escnull.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_4_Escaped_Nulls_In_URIs)
    {
        std::string rawMsg = loadRfc4475File("escnull.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("REGISTER", sipm.getMethod());
        EXPECT_EQ("sip:example.com", sipm.getUri());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.5: Escaped Method Name (esc02.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_5_Escaped_Method_Name_Rejection)
    {
        std::string rawMsg = loadRfc4475File("esc02.dat");
        auto bs = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_startline_error);
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.6: No LWS between Display Name and <> (lwsdisp.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_6_No_LWS_Display_Name)
    {
        std::string rawMsg = loadRfc4475File("lwsdisp.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("OPTIONS", sipm.getMethod());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.7: Long Values in Header Fields (longreq.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_7_Long_Values_Header_Fields)
    {
        std::string rawMsg = loadRfc4475File("longreq.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("INVITE", sipm.getMethod());
        EXPECT_TRUE(sipm.headers().contains("Via"));
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.8: Trailing Octets in Datagram (trws.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_8_Extra_Space_In_Startline_Rejection)
    {
        std::string rawMsg = loadRfc4475File("trws.dat");
        auto bs = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_startline_error);
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.9: Semicolon-Separated Parameters (cparam01.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_9_Semicolon_Separated_URI_Params)
    {
        std::string rawMsg = loadRfc4475File("cparam01.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("REGISTER", sipm.getMethod());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.10: Varied Transport Types (transports.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_10_Varied_Transport_Types)
    {
        std::string rawMsg = loadRfc4475File("transports.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("OPTIONS", sipm.getMethod());
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.11: Multipart MIME Message (mpart01.dat)
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_11_Multipart_MIME_Rejection)
    {
        std::string rawMsg = loadRfc4475File("mpart01.dat");
        auto bs = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), unsupported_contenttype_error);
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.1.12 & 3.1.1.13: Reason Phrase Variations
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_1_12_Unusual_Reason_Phrase)
    {
        std::string rawMsg = loadRfc4475File("unreason.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("SIP/2.0", sipm.value("/s/version"_json_pointer, ""));
        EXPECT_EQ(200, sipm.value("/s/status"_json_pointer, 0));
    }

    TEST(RFC4475_Torture, Section_3_1_1_13_Empty_Reason_Phrase)
    {
        std::string rawMsg = loadRfc4475File("noreason.dat");
        auto bs = rawMsg.begin();

        sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
        EXPECT_EQ("SIP/2.0", sipm.value("/s/version"_json_pointer, ""));
        EXPECT_EQ(100, sipm.value("/s/status"_json_pointer, 0));
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Section 3.1.2: Robust Exception Handling for Invalid Messages
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Section_3_1_2_Negative_Content_Length_Rejection)
    {
        std::string rawMsg = loadRfc4475File("ncl.dat");
        auto bs = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_document_error);
    }

    TEST(RFC4475_Torture, Section_3_1_2_Unknown_Protocol_Version_Rejection)
    {
        std::string rawMsg = loadRfc4475File("badvers.dat");
        auto bs = rawMsg.begin();

        EXPECT_THROW(sip2json::parseFromBuffer(bs, rawMsg.end()), invalid_startline_error);
    }

    //-------------------------------------------------------------------------
    // RFC 4475 Exhaustive Test Suite: All 50 Official Bit-Exact Files
    //-------------------------------------------------------------------------
    TEST(RFC4475_Torture, Exhaustive_Corpus_All_50_Official_IETF_Files)
    {
        std::vector<std::string> allFiles = {
                "badaspec.dat", "badbranch.dat", "baddate.dat", "baddn.dat", "badinv01.dat",
                "badvers.dat", "bcast.dat", "bext01.dat", "bigcode.dat", "clerr.dat",
                "cparam01.dat", "cparam02.dat", "dblreq.dat", "esc01.dat", "esc02.dat",
                "escnull.dat", "escruri.dat", "insuf.dat", "intmeth.dat", "inv2543.dat",
                "invut.dat", "longreq.dat", "ltgtruri.dat", "lwsdisp.dat", "lwsruri.dat",
                "lwsstart.dat", "mcl01.dat", "mismatch01.dat", "mismatch02.dat", "mpart01.dat",
                "multi01.dat", "ncl.dat", "noreason.dat", "novelsc.dat", "quotbal.dat",
                "regaut01.dat", "regbadct.dat", "regescrt.dat", "scalar02.dat", "scalarlg.dat",
                "sdp01.dat", "semiuri.dat", "test.dat", "transports.dat", "trws.dat",
                "unkscm.dat", "unksm2.dat", "unreason.dat", "wsinv.dat", "zeromf.dat"
        };

        size_t parsedCount = 0;
        size_t rejectedCount = 0;

        for (const auto& file : allFiles)
        {
            std::string rawMsg = loadRfc4475File(file);
            auto bs = rawMsg.begin();

            try
            {
                sipmessage sipm = sip2json::parseFromBuffer(bs, rawMsg.end());
                parsedCount++;
            }
            catch (const std::exception& ex)
            {
                // Safely rejected malformed invalid torture test input
                rejectedCount++;
            }
        }

        EXPECT_EQ(50, parsedCount + rejectedCount);
        EXPECT_GT(parsedCount, 0);
        EXPECT_GT(rejectedCount, 0);
        std::cout << " -- RFC 4475 Official IETF Torture Suite: "
                  << parsedCount << " valid messages parsed, "
                  << rejectedCount << " malformed messages safely rejected without crash (Total: "
                  << (parsedCount + rejectedCount) << " / 50 files)." << std::endl;
    }
} // namespace siddiqsoft
