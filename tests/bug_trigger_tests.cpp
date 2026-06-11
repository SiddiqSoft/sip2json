/*
  Bug Trigger Tests for sip2json
  Tests designed to expose the 18 reported bugs and undefined behavior issues
  
  Compile with: g++ -std=c++20 -I../include bug_trigger_tests.cpp -o bug_tests
*/

#include <string>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <set>
#include <sstream>
#include <format>

#include "nlohmann/json.hpp"
#include "../include/siddiqsoft/sip2json.hpp"
#include "gtest/gtest.h"

using namespace siddiqsoft;

// ============================================================================
// CRITICAL BUG TESTS
// ============================================================================

/// BUG #1: Iterator Arithmetic Overflow Risk
/// Tests unsafe pointer arithmetic in header folding logic
TEST(BugTrigger_Critical, Bug_1_IteratorArithmeticOverflow)
{
    // Craft a SIP message with header folding at buffer boundary
    // The folding indicator check does: *(hend + lineEndSize)
    // without verifying hend + lineEndSize < headerEnd
    
    std::string malformedSIP = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        // Craft header that ends exactly at buffer boundary with folding indicator
        "Subject: Test\r\n"
        " \r\n"  // Folding indicator at end - triggers bounds check
        "Content-Length: 0\r\n"
        "\r\n";
    
    auto bufferStart = malformedSIP.begin();
    auto bufferEnd = malformedSIP.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        // If we get here without crash, the bug may not be triggered
        std::cout << "Bug #1: Parsing completed (may not have triggered bounds check)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #1: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #2: String Iterator Invalidation
/// Tests iterator invalidation after buffer erase in parseAsync
TEST(BugTrigger_Critical, Bug_2_StringIteratorInvalidation)
{
    std::string buffer = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Length: 0\r\n"
        "\r\n"
        "INVITE sip:user2@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user2@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    int parseCount = 0;
    try {
        sip2json::parseAsync(buffer, [&](sipmessage&& msg) {
            parseCount++;
            std::cout << "Bug #2: Parsed message " << parseCount << std::endl;
        });
        std::cout << "Bug #2: Parsed " << parseCount << " messages" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #2: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #3: Weak Random Number Generation & Thread Safety
/// Tests predictability and thread safety of createCallId
TEST(BugTrigger_Critical, Bug_3_WeakRandomAndThreadSafety)
{
    // Test 1: Check if Call-IDs are predictable (weak entropy)
    std::set<std::string> callIds;
    for (int i = 0; i < 100; ++i) {
        callIds.insert(createCallId());
    }
    
    if (callIds.size() < 100) {
        std::cout << "Bug #3: WEAK ENTROPY - Only " << callIds.size() << " unique Call-IDs out of 100" << std::endl;
    } else {
        std::cout << "Bug #3: Generated " << callIds.size() << " unique Call-IDs" << std::endl;
    }
    
    // Test 2: Thread safety - race condition
    std::vector<std::string> threadCallIds;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 100; ++j) {
                threadCallIds.push_back(createCallId());
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::set<std::string> uniqueThreadCallIds(threadCallIds.begin(), threadCallIds.end());
    std::cout << "Bug #3: Thread safety - Generated " << uniqueThreadCallIds.size() 
              << " unique Call-IDs from 10 threads (expected 1000)" << std::endl;
    
    if (uniqueThreadCallIds.size() < 1000) {
        std::cout << "Bug #3: RACE CONDITION DETECTED - Duplicate Call-IDs generated" << std::endl;
    }
}

// ============================================================================
// HIGH PRIORITY BUG TESTS
// ============================================================================

/// BUG #4: Unsafe stoi() Without Exception Handling
/// Tests crash on invalid Content-Length header
TEST(BugTrigger_High, Bug_4_UnsafeStoiNoExceptionHandling)
{
    std::string malformedSIP = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Length: NOT_A_NUMBER\r\n"  // Invalid integer
        "\r\n";
    
    auto bufferStart = malformedSIP.begin();
    auto bufferEnd = malformedSIP.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #4: Parsing completed (exception not thrown)" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Bug #4: CAUGHT - std::invalid_argument: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #4: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #4b: Unsafe stoi() - Out of Range
TEST(BugTrigger_High, Bug_4b_UnsafeStoiOutOfRange)
{
    std::string malformedSIP = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Expires: 999999999999999999999999999999\r\n"  // Out of range
        "Content-Length: 0\r\n"
        "\r\n";
    
    auto bufferStart = malformedSIP.begin();
    auto bufferEnd = malformedSIP.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #4b: Parsing completed (exception not thrown)" << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Bug #4b: CAUGHT - std::out_of_range: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #4b: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #6: Unsafe Header Folding Logic
/// Tests buffer overflow in header continuation
TEST(BugTrigger_High, Bug_6_UnsafeHeaderFolding)
{
    // Create a message with header folding that could overflow
    std::string malformedSIP = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Subject: This is a very long subject that will be folded\r\n"
        " and continued on the next line\r\n"
        " and another continuation\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    auto bufferStart = malformedSIP.begin();
    auto bufferEnd = malformedSIP.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #6: Header folding parsed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #6: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #7: Incomplete SDP Timing Validation
/// Tests malformed SDP timing elements
TEST(BugTrigger_High, Bug_7_IncompleteSdpTimingValidation)
{
    std::string sdpWithBadTiming = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 100\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 128.3.4.5\r\n"
        "s=Session SDP\r\n"
        "c=IN IP4 128.3.4.5\r\n"
        "t=0\r\n"  // Only one value instead of two
        "m=audio 6000 RTP/AVP 0\r\n";
    
    auto bufferStart = sdpWithBadTiming.begin();
    auto bufferEnd = sdpWithBadTiming.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #7: Parsed SDP with incomplete timing" << std::endl;
        
        // Try to serialize - this should fail if timing is incomplete
        try {
            auto serialized = sip2json::serialize(result);
            std::cout << "Bug #7: Serialization succeeded (may have incomplete timing)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Bug #7: Serialization failed: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Bug #7: Parsing failed: " << e.what() << std::endl;
    }
}

/// BUG #8: Missing Bounds Check in Serialization
/// Tests serialization of malformed timing array
TEST(BugTrigger_High, Bug_8_MissingBoundsCheckSerialization)
{
    sipmessage msg(200);
    msg.setHeader("Content-Type", "application/sdp");
    
    // Manually create malformed SDP with incomplete timing array
    nlohmann::json sdp = nlohmann::json::array();
    nlohmann::json block;
    block["v"] = 0;
    block["o"] = "user1 53655765 2353687637 IN IP4 128.3.4.5";
    block["s"] = "Session";
    block["c"] = nlohmann::json{{"type", "IN"}, {"subtype", "IP4"}, {"dn", "128.3.4.5"}};
    block["t"] = nlohmann::json::array();
    block["t"].push_back(0);  // Only one value - incomplete!
    block["m"] = "audio 6000 RTP/AVP 0";
    block["a"] = nlohmann::json::object();
    
    sdp.push_back(block);
    msg.setBody(nlohmann::json::json_pointer("/sdp"), sdp);
    
    try {
        auto serialized = sip2json::serialize(msg);
        std::cout << "Bug #8: Serialization succeeded with incomplete timing" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #8: CAUGHT - Serialization failed: " << e.what() << std::endl;
    }
}

// ============================================================================
// MEDIUM PRIORITY BUG TESTS
// ============================================================================

/// BUG #9: Unsafe String Comparison for Header Names
/// Tests case-sensitive header matching
TEST(BugTrigger_Medium, Bug_9_UnsafeStringComparison)
{
    // Test with different case variations of "uthorization"
    std::string malformedSIP = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Uthorization: Digest username=\"user\"\r\n"  // Capital U - won't match
        "Content-Length: 0\r\n"
        "\r\n";
    
    auto bufferStart = malformedSIP.begin();
    auto bufferEnd = malformedSIP.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        
        // Check if Authorization header was properly handled
        if (result["h"].contains("Authorization")) {
            std::cout << "Bug #9: Authorization header found" << std::endl;
        } else if (result["h"].contains("Uthorization")) {
            std::cout << "Bug #9: CASE SENSITIVITY - Uthorization not normalized to Authorization" << std::endl;
        } else {
            std::cout << "Bug #9: Neither Authorization nor Uthorization found" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Bug #9: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #10: Incomplete Regex Pattern for SDP Attributes
/// Tests SDP attribute parsing with edge cases
TEST(BugTrigger_Medium, Bug_10_IncompleteRegexPattern)
{
    std::string sdpWithEdgeAttributes = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 150\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 128.3.4.5\r\n"
        "s=Session SDP\r\n"
        "c=IN IP4 128.3.4.5\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=:\r\n"  // Empty attribute name
        "a=rtpmap:0 PCMU/8000\r\n";
    
    auto bufferStart = sdpWithEdgeAttributes.begin();
    auto bufferEnd = sdpWithEdgeAttributes.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #10: Parsed SDP with edge case attributes" << std::endl;
        
        // Check if empty attribute was accepted
        if (result["b"]["sdp"][0]["a"].contains("")) {
            std::cout << "Bug #10: REGEX ISSUE - Empty attribute name accepted" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Bug #10: Exception caught: " << e.what() << std::endl;
    }
}

/// BUG #11: Inconsistent Error Handling
/// Tests different error handling between parse() and parseAsync()
TEST(BugTrigger_Medium, Bug_11_InconsistentErrorHandling)
{
    std::string buffer = 
        "INVALID_START_LINE\r\n"
        "Via: SIP/2.0/UDP pc33.example.com\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    // Test parse() method
    auto bufferStart = buffer.begin();
    auto bufferEnd = buffer.end();
    
    try {
        auto results = sip2json::parse(bufferStart, bufferEnd);
        std::cout << "Bug #11: parse() succeeded with " << results.size() << " messages" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #11: parse() threw: " << e.what() << std::endl;
    }
    
    // Test parseAsync() method
    std::string buffer2 = buffer;
    int parseCount = 0;
    try {
        sip2json::parseAsync(buffer2, [&](sipmessage&& msg) {
            parseCount++;
        });
        std::cout << "Bug #11: parseAsync() parsed " << parseCount << " messages" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #11: parseAsync() threw: " << e.what() << std::endl;
    }
}

/// BUG #13: Unsafe JSON Pointer Construction
/// Tests JSON pointer injection vulnerability
TEST(BugTrigger_Medium, Bug_13_UnsafeJsonPointer)
{
    std::string sdpWithSpecialChars = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 150\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 128.3.4.5\r\n"
        "s=Session SDP\r\n"
        "c=IN IP4 128.3.4.5\r\n"
        "t=0 0\r\n"
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap/0:PCMU/8000\r\n";  // Forward slash in attribute - JSON pointer injection
    
    auto bufferStart = sdpWithSpecialChars.begin();
    auto bufferEnd = sdpWithSpecialChars.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #13: Parsed SDP with special characters in attributes" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #13: Exception caught: " << e.what() << std::endl;
    }
}

// ============================================================================
// LOW PRIORITY BUG TESTS
// ============================================================================

/// BUG #14: Missing Input Validation in Constructor
/// Tests sipmessage constructor with invalid inputs
TEST(BugTrigger_Low, Bug_14_MissingInputValidation)
{
    try {
        // Empty method
        sipmessage msg1("", "sip:user@example.com");
        std::cout << "Bug #14: Created message with empty method (should validate)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #14: Exception for empty method: " << e.what() << std::endl;
    }
    
    try {
        // Empty URI
        sipmessage msg2("INVITE", "");
        std::cout << "Bug #14: Created message with empty URI (should validate)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #14: Exception for empty URI: " << e.what() << std::endl;
    }
    
    try {
        // Invalid method
        sipmessage msg3("INVALID_METHOD", "sip:user@example.com");
        std::cout << "Bug #14: Created message with invalid method (should validate)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #14: Exception for invalid method: " << e.what() << std::endl;
    }
}

/// BUG #18: Integer Overflow Risk
/// Tests Content-Length with large values
TEST(BugTrigger_Low, Bug_18_IntegerOverflowRisk)
{
    std::string largeContentLength = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Content-Length: 4294967295\r\n"  // Max uint32_t
        "\r\n";
    
    auto bufferStart = largeContentLength.begin();
    auto bufferEnd = largeContentLength.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Bug #18: Parsed message with large Content-Length: " 
                  << result.getContentLength() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bug #18: Exception caught: " << e.what() << std::endl;
    }
}

// ============================================================================
// STRESS TESTS
// ============================================================================

/// Stress test combining multiple bugs
TEST(BugTrigger_Stress, CombinedBugStress)
{
    std::string complexMessage = 
        "INVITE sip:user@example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: <sip:user@example.com>\r\n"
        "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.example.com>\r\n"
        "Subject: Test\r\n"
        " continuation\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: 200\r\n"
        "\r\n"
        "v=0\r\n"
        "o=user1 53655765 2353687637 IN IP4 128.3.4.5\r\n"
        "s=Session SDP\r\n"
        "c=IN IP4 128.3.4.5\r\n"
        "t=0\r\n"  // Incomplete timing
        "m=audio 6000 RTP/AVP 0\r\n"
        "a=rtpmap:0 PCMU/8000\r\n";
    
    auto bufferStart = complexMessage.begin();
    auto bufferEnd = complexMessage.end();
    
    try {
        auto result = sip2json::parseFromBuffer(bufferStart, bufferEnd);
        std::cout << "Stress Test: Parsing succeeded" << std::endl;
        
        try {
            auto serialized = sip2json::serialize(result);
            std::cout << "Stress Test: Serialization succeeded" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Stress Test: Serialization failed: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Stress Test: Parsing failed: " << e.what() << std::endl;
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
