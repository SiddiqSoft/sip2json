# Practical Usage Examples

This guide provides end-to-end code examples demonstrating common tasks with `sip2json`: stream parsing, message construction, SDP payloads, and JSON transformations.

---

## 1. Asynchronous Stream Parsing from Network Buffers

When receiving continuous TCP/TLS streams, multiple SIP messages can arrive in a single packet, or a single message may be fragmented across multiple read cycles. `sip2json::parseAsync` processes all complete frames and leaves partial frames in the buffer for the next read:

```cpp
#include <iostream>
#include <string>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

void onSocketDataReceived(std::string& socketReadBuffer)
{
    auto cursor = socketReadBuffer.begin();

    // Parse all complete SIP messages in the buffer
    sip2json::parseAsync(
        cursor,
        socketReadBuffer.end(),
        [](sipmessage&& msg) {
            std::cout << "[SIP Message Decoded]\n";
            if (msg.isMessageRequest()) {
                std::cout << "  Method : " << msg.getMethod() << "\n";
                std::cout << "  URI    : " << msg.getUri() << "\n";
            } else if (msg.isMessageResponse()) {
                std::cout << "  Status : " << msg.value("/s/status"_json_pointer, 0) << "\n";
                std::cout << "  Reason : " << msg.getReason() << "\n";
            }
            std::cout << "  Call-ID: " << msg.getCallID() << "\n";
            std::cout << "  From   : " << msg.getHeader(HF_FROM) << "\n";
            std::cout << "  To     : " << msg.getHeader(HF_TO) << "\n";
        },
        [](const sip2json_exception& ex, std::string::iterator&, const std::string::iterator&) {
            std::cerr << "Parser Warning: " << ex.what() << "\n";
        }
    );

    // Erase consumed frames; unparsed partial frames remain at the front
    socketReadBuffer.erase(socketReadBuffer.begin(), cursor);
}
```

---

## 2. SIP Message Construction & Serialization (Fluent API)

Constructing SIP requests and responses using method chaining and serializing them into standard RFC 3261 wire format:

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

int main()
{
    // 1. Construct INVITE Request with Start-Line parameters
    sipmessage msg(METHOD_INVITE, "sip:bob@example.com", "call-id-99812", 42);

    // 2. Set Headers using fluent chaining
    msg.setHeader(HF_FROM, "sip:alice@example.com;tag=a831")
       .setHeader(HF_TO, "sip:bob@example.com")
       .setHeader(HF_CONTACT, "<sip:alice@10.0.0.4:5060>")
       .setHeader(HF_CONTENT_TYPE, CONTENT_TYPE_APP_SDP)
       .setUserAgent("CustomApp/1.0");

    // 3. Attach SDP Session Description Body
    msg.setBody("/sdp/0/v"_json_pointer, 0)
       .setBody("/sdp/0/s"_json_pointer, "Talk")
       .setBody("/sdp/0/o/user"_json_pointer, "alice")
       .setBody("/sdp/0/o/host"_json_pointer, "10.0.0.4")
       .setBody("/sdp/0/c/dn"_json_pointer, "10.0.0.4")
       .setBody("/sdp/0/m"_json_pointer, "audio 49170 RTP/AVP 0 101");

    // 4. Serialize to RFC 3261 Wire Format
    try {
        std::string rawSipWire = sip2json::serialize(msg);
        std::cout << "Serialized Wire SIP:\n\n" << rawSipWire << std::endl;
    } catch (const sip2json_exception& e) {
        std::cerr << "Serialization error: " << e.what() << std::endl;
    }

    return 0;
}
```

---

## 3. Direct Construction from JSON (Zero-Setter Metaphor)

Because `sipmessage` inherits directly from `nlohmann::json`, you can construct and serialize SIP messages directly from a native JSON structure without calling `.setHeader()` or `.setBody()` methods.

This pattern is ideal for microservices and cloud workers receiving structured JSON payloads from message queues (Kafka, RabbitMQ, Azure Service Bus) or HTTP REST endpoints that need to emit SIP messages directly onto the wire:

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

int main()
{
    // 1. Directly initialize sipmessage from the JSON document
    // Define the full SIP message and SDP body directly via JSON structure
    siddiqsoft::sipmessage msg(nlohmann::json {
        {"s", { // start-line
            {"type", "request"},
            {"method", "INVITE"},
            {"uri", "sip:bob@example.com"},
            {"version", "SIP/2.0"}
        }},
        {"h", { // headers
            {"Call-ID", siddiqsoft::createCallId()},
            {"CSeq", "1 INVITE"},
            {"From", "sip:alice@example.com;tag=a831"},
            {"To", "sip:bob@example.com"},
            {"Via", {"SIP/2.0/TCP 10.0.0.4:5060;branch=z9hG4bK776"}},
            {"Contact", "<sip:alice@10.0.0.4:5060>"},
            {"Content-Type", "application/sdp"},
            {"User-Agent", "sip2json/2.6"}
        }},
        {"b", { //body
            {"sdp", { // must match what you set in Content-Type header
                {
                    {"v", 0}, // first sdp element
                    {"o", {   // origin info
                        {"user", "alice"},
                        {"t1", "2890844526"},
                        {"t2", "2890844526"},
                        {"type", "IN"},
                        {"subtype", "IP4"},
                        {"host", "10.0.0.4"}
                    }},
                    {"s", "Talk"},
                    {"c", { // contact
                        {"type", "IN"},
                        {"subtype", "IP4"},
                        {"dn", "10.0.0.4"}
                    }},
                    {"t", {0, 0}}, // times are epoch values start/stop
                    {"m", "audio 49170 RTP/AVP 0 101"}, // codecs
                    {"a", { // extended attributes
                        {"rtpmap", {"0 PCMU/8000", "101 telephone-event/8000"}},
                        {"fmtp", "101 0-16"},
                        {"sendrecv", true}
                    }}
                }
            }}
        }}
    };

    // 2. Serialize directly to standard RFC 3261 wire format
    try {
        std::string rawSipWire = siddiqsoft::sip2json::serialize(msg);
        std::cout << "Directly Serialized Wire SIP:\n\n" << rawSipWire << std::endl;
    } catch (const sip2json_exception& e) {
        std::cerr << "Serialization error: " << e.what() << std::endl;
    }

    return 0;
}
```

!!! tip "JSON Schema Specifications"
    - Learn more about the core message schema, status line layout, and header formatting in the [**JSON Schema Specification**](../features/json_schema.md).
    - Learn more about media lines (`m=`), connection addresses (`c=`), and multi-attribute mapping in the [**SDP Media Schema & Attributes**](../features/sdp.md).

---

## 4. First-Class JSON Interoperability & NoSQL Export

Because `sipmessage` is an `nlohmann::json` object, you can directly query and export parsed SIP messages to NoSQL databases (MongoDB, Azure Cosmos DB, PostgreSQL `jsonb`) or log analytics pipelines with zero translation cost:

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

void exportToJsonStream(const std::string& rawSipString)
{
    // Parse raw SIP buffer
    auto bs = rawSipString.begin();
    sipmessage msg = sip2json::parseFromBuffer(bs, rawSipString.end());

    // Access native JSON document
    nlohmann::json jsonDoc = msg;

    // Pretty-printed JSON output
    std::cout << jsonDoc.dump(2) << std::endl;

    // Direct JSON pointer queries
    if (jsonDoc.contains("/h/Via"_json_pointer)) {
        std::cout << "Via: " << jsonDoc["/h/Via"_json_pointer] << std::endl;
    }
}
```

