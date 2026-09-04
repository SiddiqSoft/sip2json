# Code Examples & Integration Recipes

Ready-to-compile modern C++20 code examples demonstrating common `sip2json` parsing, serialization, and high-throughput streaming patterns.

---

## Core Usage Examples

=== "1. Asynchronous Stream Parsing"

    `sip2json::parseAsync` reads continuous TCP/TLS buffers, processes complete frames via zero-copy callbacks, and leaves partial frames intact for subsequent network reads.

    ```cpp
    #include <iostream>
    #include <string>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void handleNetworkInput(std::string& socketReadBuffer)
    {
        auto cursor = socketReadBuffer.begin();

        // Parse stream buffer in-place; invoke callback for each complete frame
        sip2json::parseAsync(
            cursor,
            socketReadBuffer.end(),
            [](sipmessage&& msg) {
                std::cout << "[SIP Message Received]\n"
                          << "  Type:    " << msg.type << "\n"
                          << "  Method:  " << msg.method << "\n"
                          << "  URI:     " << msg.uri << "\n"
                          << "  Call-ID: " << msg.callid << "\n";
            },
            [](sip2json_exception& ex, std::string::iterator& start, const std::string::iterator& end) {
                std::cerr << "[Parser Error] " << ex.what() << "\n";
            }
        );

        // Erase consumed frames; incomplete frames remain at front of buffer
        socketReadBuffer.erase(socketReadBuffer.begin(), cursor);
    }

    int main()
    {
        std::string buffer =
            "REGISTER sip:example.com SIP/2.0\r\n"
            "Via: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK-123\r\n"
            "From: <sip:user@example.com>;tag=111\r\n"
            "To: <sip:user@example.com>\r\n"
            "Call-ID: reg-101@192.168.1.1\r\n"
            "CSeq: 1 REGISTER\r\n"
            "Content-Length: 0\r\n\r\n";

        handleNetworkInput(buffer);
        return 0;
    }
    ```

=== "2. Message Construction & Serialization"

    Construct standard RFC 3261 messages using fluent chaining, attach structured SDP bodies, and serialize directly to wire format.

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main()
    {
        // 1. Construct INVITE request DTO
        sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:bob@biloxi.example.com", "call-id-99812@atlanta", 1);

        // 2. Set headers using fluent chaining
        msg.setHeader(siddiqsoft::HF_FROM, "sip:alice@atlanta.example.com;tag=a831")
           .setHeader(siddiqsoft::HF_TO, "sip:bob@biloxi.example.com")
           .setHeader(siddiqsoft::HF_CONTACT, "<sip:alice@10.0.0.4:5060>")
           .setHeader(siddiqsoft::HF_CONTENT_TYPE, "application/sdp")
           .setHeader(siddiqsoft::HF_USER_AGENT, "sip2json/3.0");

        // 3. Attach SDP payload directly
        msg.body = {
            {"sdp", {
                {
                    {"v", 0},
                    {"o", {{"user", "alice"}, {"t1", "1000"}, {"t2", "1000"}, {"type", "IN"}, {"subtype", "IP4"}, {"host", "10.0.0.4"}}},
                    {"s", "SIP Session"},
                    {"c", {{"type", "IN"}, {"subtype", "IP4"}, {"dn", "10.0.0.4"}}},
                    {"t", {0, 0}},
                    {"m", "audio 49170 RTP/AVP 0 101"},
                    {"a", {{"rtpmap", {"0 PCMU/8000", "101 telephone-event/8000"}}}}
                }
            }}
        };

        // 4. Serialize to wire format
        try {
            std::string wireText = sip2json::serialize(msg);
            std::cout << "Serialized SIP Wire Output:\n\n" << wireText << std::endl;
        } catch (const sip2json_exception& e) {
            std::cerr << "Serialization error: " << e.what() << std::endl;
        }

        return 0;
    }
    ```

=== "3. Single Message Parsing (`parseFromBuffer`)"

    For UDP datagrams or pre-framed buffers where exactly one SIP message is expected.

    ```cpp
    #include <iostream>
    #include <string_view>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main()
    {
        std::string_view datagram = 
            "SIP/2.0 200 OK\r\n"
            "Via: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK-123\r\n"
            "From: <sip:user@example.com>;tag=111\r\n"
            "To: <sip:user@example.com>;tag=222\r\n"
            "Call-ID: reg-101@192.168.1.1\r\n"
            "CSeq: 1 REGISTER\r\n"
            "Content-Length: 0\r\n\r\n";

        try {
            auto cursor = datagram.begin();
            sipmessage msg = sip2json::parseFromBuffer(cursor, datagram.end());

            std::cout << "Status: " << msg.getResponseCode() << " " << msg.getReason() << "\n"
                      << "Call-ID: " << msg.getCallID() << "\n";
        } catch (const sip2json_exception& e) {
            std::cerr << "Parse failure: " << e.what() << "\n";
        }

        return 0;
    }
    ```

=== "4. JSON DTO & Analytics Integration"

    Because `sipmessage` subclasses `nlohmann::json`, it converts natively to JSON documents for queuing into Kafka, RabbitMQ, DuckDB, or MongoDB.

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void processSipAsJson(const sipmessage& msg)
    {
        // 1. Direct conversion to nlohmann::json
        const nlohmann::json& doc = msg;

        // 2. Query compact metaphor schema: s (startline), h (headers), b (body)
        std::cout << "Method: " << doc["s"]["m"].get<std::string>() << "\n"
                  << "CSeq:   " << doc["s"]["cs"].get<uint64_t>() << "\n"
                  << "From:   " << doc["h"]["from"].get<std::string>() << "\n";

        // 3. Pretty-print or export
        std::cout << doc.dump(2) << "\n";
    }
    ```

---

## Related References

* [`sipmessage` Class Specification](../sipmessage.md)
* [`sip2json` Static Functions](../sip2json.md)
* [Exception & Error Handling](../errors.md)
* [JSON Schema Specification](../json_schema.md)
* [SDP Support](../sdp.md)
