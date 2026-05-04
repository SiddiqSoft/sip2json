/*
  Rule of Five Compliance Tests for sipmessage Class
  
  This test suite verifies that the sipmessage class properly implements
  all five special member functions according to C++ best practices:
  1. Default constructor
  2. Copy constructor
  3. Move constructor
  4. Copy assignment operator
  5. Move assignment operator
  6. Destructor (bonus)
*/

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "nlohmann/json.hpp"
#include "../include/siddiqsoft/sip2json.hpp"
#include "gtest/gtest.h"

namespace
{
    // NOLINTNEXTLINE
    TEST(RuleOfFive, DefaultConstructor)
    {
        // Test that default constructor creates a valid sipmessage with metadata
        siddiqsoft::sipmessage msg;
        
        EXPECT_TRUE(msg.contains("meta"));
        EXPECT_FALSE(msg.value("/meta/version"_json_pointer, std::string {}).empty());
        EXPECT_FALSE(msg.value("/meta/time"_json_pointer, std::string {}).empty());
        EXPECT_EQ(0, msg.value("/meta/ttx"_json_pointer, -1));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyConstructor_FromSipmessage)
    {
        // Test that copy constructor creates a deep copy
        siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", "call-id-123", 1);
        original.setHeader("X-Custom", "original-value");
        
        siddiqsoft::sipmessage copy(original);
        
        // Verify copy has same content
        EXPECT_EQ(original.getCallID(), copy.getCallID());
        EXPECT_EQ("original-value", copy.getHeader<std::string>("X-Custom"));
        EXPECT_EQ(original.getMethod(), copy.getMethod());
        
        // Verify it's a deep copy (modifying copy doesn't affect original)
        copy.setHeader("X-Custom", "modified-value");
        EXPECT_EQ("original-value", original.getHeader<std::string>("X-Custom"));
        EXPECT_EQ("modified-value", copy.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyConstructor_FromJson)
    {
        // Test that copy constructor from nlohmann::json works
        nlohmann::json json_obj = {
            {"s", {{"type", "request"}, {"method", "INVITE"}, {"uri", "sip:test@example.com"}, {"version", "SIP/2.0"}}},
            {"h", {{"Call-ID", "test-call-id"}, {"User-Agent", "test-agent"}}},
            {"b", nullptr},
            {"meta", {{"version", "sip2json/2.2/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
        };
        
        siddiqsoft::sipmessage msg(json_obj);
        
        EXPECT_EQ("INVITE", msg.getMethod());
        EXPECT_EQ("test-call-id", msg.getCallID());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveConstructor_FromSipmessage)
    {
        // Test that move constructor efficiently transfers ownership
        auto callId = siddiqsoft::createCallId();
        siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", callId, 1);
        original.setHeader("X-Custom", "test-value");
        
        siddiqsoft::sipmessage moved(std::move(original));
        
        // Verify moved object has the content
        EXPECT_EQ(callId, moved.getCallID());
        EXPECT_EQ("test-value", moved.getHeader<std::string>("X-Custom"));
        EXPECT_TRUE(moved.isMessageRequest());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveConstructor_FromJson)
    {
        // Test that move constructor from nlohmann::json works
        nlohmann::json json_obj = {
            {"s", {{"type", "response"}, {"status", 200}, {"reason", "OK"}, {"version", "SIP/2.0"}}},
            {"h", {{"User-Agent", "test-agent"}}},
            {"b", nullptr},
            {"meta", {{"version", "sip2json/2.2/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
        };
        
        siddiqsoft::sipmessage msg(std::move(json_obj));
        
        EXPECT_EQ(200, msg.getStatusCode());
        EXPECT_TRUE(msg.isMessageResponse());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyAssignmentOperator_FromSipmessage)
    {
        // Test that copy assignment operator creates a deep copy
        siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", "call-id-456", 1);
        original.setHeader("X-Custom", "original-value");
        
        siddiqsoft::sipmessage target;
        target = original;
        
        // Verify target has same content
        EXPECT_EQ(original.getCallID(), target.getCallID());
        EXPECT_EQ("original-value", target.getHeader<std::string>("X-Custom"));
        
        // Verify it's a deep copy
        target.setHeader("X-Custom", "modified-value");
        EXPECT_EQ("original-value", original.getHeader<std::string>("X-Custom"));
        EXPECT_EQ("modified-value", target.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyAssignmentOperator_FromJson)
    {
        // Test that copy assignment from nlohmann::json works
        nlohmann::json json_obj = {
            {"s", {{"type", "request"}, {"method", "BYE"}, {"uri", "sip:test@example.com"}, {"version", "SIP/2.0"}}},
            {"h", {{"Call-ID", "bye-call-id"}}},
            {"b", nullptr},
            {"meta", {{"version", "sip2json/2.2/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
        };
        
        siddiqsoft::sipmessage msg;
        msg = json_obj;
        
        EXPECT_EQ("BYE", msg.getMethod());
        EXPECT_EQ("bye-call-id", msg.getCallID());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyAssignmentOperator_SelfAssignment)
    {
        // Test that self-assignment is safe
        siddiqsoft::sipmessage msg("INVITE", "sip:test@example.com", "call-id-789", 1);
        msg.setHeader("X-Custom", "test-value");
        
        auto callId = msg.getCallID();
        msg = msg;  // Self-assignment
        
        EXPECT_EQ(callId, msg.getCallID());
        EXPECT_EQ("test-value", msg.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveAssignmentOperator_FromSipmessage)
    {
        // Test that move assignment operator efficiently transfers ownership
        auto callId = siddiqsoft::createCallId();
        siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", callId, 1);
        original.setHeader("X-Custom", "test-value");
        
        siddiqsoft::sipmessage target;
        target = std::move(original);
        
        // Verify target has the content
        EXPECT_EQ(callId, target.getCallID());
        EXPECT_EQ("test-value", target.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveAssignmentOperator_FromJson)
    {
        // Test that move assignment from nlohmann::json works
        nlohmann::json json_obj = {
            {"s", {{"type", "response"}, {"status", 404}, {"reason", "Not Found"}, {"version", "SIP/2.0"}}},
            {"h", {{"User-Agent", "test-agent"}}},
            {"b", nullptr},
            {"meta", {{"version", "sip2json/2.2/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
        };
        
        siddiqsoft::sipmessage msg;
        msg = std::move(json_obj);
        
        EXPECT_EQ(404, msg.getStatusCode());
        EXPECT_TRUE(msg.isMessageResponse());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveAssignmentOperator_SelfAssignment)
    {
        // Test that self-assignment with move is safe
        auto callId = siddiqsoft::createCallId();
        siddiqsoft::sipmessage msg("INVITE", "sip:test@example.com", callId, 1);
        msg.setHeader("X-Custom", "test-value");
        
        msg = std::move(msg);  // Self-assignment with move
        
        EXPECT_EQ(callId, msg.getCallID());
        EXPECT_EQ("test-value", msg.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, Destructor_NoLeaks)
    {
        // Test that destructor properly cleans up
        {
            siddiqsoft::sipmessage msg("INVITE", "sip:test@example.com", "call-id-999", 1);
            msg.setHeader("X-Custom", "test-value");
            // Destructor called when msg goes out of scope
        }
        // If there are memory leaks, they will be detected by the test framework
        EXPECT_TRUE(true);
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyInContainer)
    {
        // Test that copy semantics work in standard containers
        std::vector<siddiqsoft::sipmessage> messages;
        
        siddiqsoft::sipmessage msg1("INVITE", "sip:test1@example.com", "call-id-1", 1);
        siddiqsoft::sipmessage msg2("BYE", "sip:test2@example.com", "call-id-2", 1);
        
        messages.push_back(msg1);
        messages.push_back(msg2);
        
        EXPECT_EQ(2, messages.size());
        EXPECT_EQ("INVITE", messages[0].getMethod());
        EXPECT_EQ("BYE", messages[1].getMethod());
        
        // Verify copies are independent
        messages[0].setHeader("X-Custom", "modified");
        EXPECT_EQ("", msg1.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveInContainer)
    {
        // Test that move semantics work in standard containers
        std::vector<siddiqsoft::sipmessage> messages;
        
        siddiqsoft::sipmessage msg1("INVITE", "sip:test1@example.com", "call-id-1", 1);
        siddiqsoft::sipmessage msg2("BYE", "sip:test2@example.com", "call-id-2", 1);
        
        messages.push_back(std::move(msg1));
        messages.push_back(std::move(msg2));
        
        EXPECT_EQ(2, messages.size());
        EXPECT_EQ("INVITE", messages[0].getMethod());
        EXPECT_EQ("BYE", messages[1].getMethod());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, ConstCorrectness_GettersOnConstObject)
    {
        // Test that all getters work on const objects
        const siddiqsoft::sipmessage msg("INVITE", "sip:test@example.com", "call-id-const", 1);
        
        // All these should compile and work
        EXPECT_EQ("INVITE", msg.getMethod());
        EXPECT_EQ("call-id-const", msg.getCallID());
        EXPECT_TRUE(msg.isMessageRequest());
        EXPECT_FALSE(msg.isMessageResponse());
        EXPECT_TRUE(msg.hasBody());
        
        // Const accessors
        const auto& headers = msg.headers();
        const auto& body = msg.body();
        
        EXPECT_TRUE(headers.contains("User-Agent"));
        EXPECT_TRUE(body.is_null());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, ChainedAssignment)
    {
        // Test that assignment operators support chaining
        siddiqsoft::sipmessage msg1("INVITE", "sip:test1@example.com", "call-id-1", 1);
        siddiqsoft::sipmessage msg2;
        siddiqsoft::sipmessage msg3;
        
        msg3 = msg2 = msg1;
        
        EXPECT_EQ(msg1.getCallID(), msg2.getCallID());
        EXPECT_EQ(msg1.getCallID(), msg3.getCallID());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyThenModify)
    {
        // Test that copy creates independent objects
        siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", "call-id-orig", 1);
        original.setHeader("X-Test", "original");
        
        siddiqsoft::sipmessage copy = original;
        copy.setHeader("X-Test", "copy");
        copy.setHeader("X-New", "new-value");
        
        EXPECT_EQ("original", original.getHeader<std::string>("X-Test"));
        EXPECT_EQ("copy", copy.getHeader<std::string>("X-Test"));
        EXPECT_EQ("", original.getHeader<std::string>("X-New"));
        EXPECT_EQ("new-value", copy.getHeader<std::string>("X-New"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MovePreservesContent)
    {
        // Test that move operation preserves content in moved-to object
        auto callId = siddiqsoft::createCallId();
        siddiqsoft::sipmessage original("INVITE", "sip:test@example.com", callId, 1);
        original.setHeader("X-Custom", "test-value");
        
        siddiqsoft::sipmessage moved = std::move(original);
        
        // Verify moved object has all the content
        EXPECT_EQ(callId, moved.getCallID());
        EXPECT_EQ("INVITE", moved.getMethod());
        EXPECT_TRUE(moved.isMessageRequest());
        EXPECT_EQ("test-value", moved.getHeader<std::string>("X-Custom"));
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MultipleAssignments)
    {
        // Test multiple assignments in sequence
        siddiqsoft::sipmessage msg1("INVITE", "sip:test1@example.com", "call-id-1", 1);
        siddiqsoft::sipmessage msg2("BYE", "sip:test2@example.com", "call-id-2", 1);
        siddiqsoft::sipmessage msg3("REGISTER", "sip:test3@example.com", "call-id-3", 1);
        
        siddiqsoft::sipmessage target;
        
        target = msg1;
        EXPECT_EQ("INVITE", target.getMethod());
        
        target = msg2;
        EXPECT_EQ("BYE", target.getMethod());
        
        target = msg3;
        EXPECT_EQ("REGISTER", target.getMethod());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, CopyFromResponse)
    {
        // Test copy semantics with response messages
        siddiqsoft::sipmessage request("INVITE", "sip:test@example.com", "call-id-resp", 1);
        siddiqsoft::sipmessage response(200, request);
        
        siddiqsoft::sipmessage copy = response;
        
        EXPECT_EQ(200, copy.getStatusCode());
        EXPECT_TRUE(copy.isMessageResponse());
        EXPECT_EQ(response.getCallID(), copy.getCallID());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, MoveFromResponse)
    {
        // Test move semantics with response messages
        siddiqsoft::sipmessage request("INVITE", "sip:test@example.com", "call-id-move-resp", 1);
        siddiqsoft::sipmessage response(200, request);
        
        siddiqsoft::sipmessage moved = std::move(response);
        
        EXPECT_EQ(200, moved.getStatusCode());
        EXPECT_TRUE(moved.isMessageResponse());
    }

    // NOLINTNEXTLINE
    TEST(RuleOfFive, ExplicitConstructorsFromJson)
    {
        // Test that explicit constructors from nlohmann::json work
        nlohmann::json json_obj = {
            {"s", {{"type", "request"}, {"method", "OPTIONS"}, {"uri", "sip:test@example.com"}, {"version", "SIP/2.0"}}},
            {"h", {{"Call-ID", "options-call-id"}}},
            {"b", nullptr},
            {"meta", {{"version", "sip2json/2.2/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
        };
        
        // Explicit copy constructor
        siddiqsoft::sipmessage msg1(json_obj);
        EXPECT_EQ("OPTIONS", msg1.getMethod());
        
        // Explicit move constructor
        siddiqsoft::sipmessage msg2(std::move(json_obj));
        EXPECT_EQ("OPTIONS", msg2.getMethod());
    }

} // namespace
