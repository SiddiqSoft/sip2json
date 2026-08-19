# Message Serialization Example

This example demonstrates constructing a SIP message using the builder syntax and serializing it into standard SIP wire format.

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

int main()
{
    // 1. Construct INVITE Request
    sipmessage msg("INVITE", "sip:bob@example.com", "call-id-99812", 42);

    // 2. Set Headers using fluent method chaining
    msg.setHeader("From", "sip:alice@example.com;tag=a831")
       .setHeader("To", "sip:bob@example.com")
       .setHeader("Contact", "<sip:alice@10.0.0.4:5060>")
       .setHeader("Content-Type", "application/sdp")
       .setHeader("User-Agent", "sip2json/2.0");

    // 3. Attach SDP Body
    msg.body = {
        {"sdp", {
            {
                {"v", 0},
                {"o", {{"user", "alice"}, {"t1", "1000"}, {"t2", "1000"}, {"type", "IN"}, {"subtype", "IP4"}, {"host", "10.0.0.4"}}},
                {"s", "Talk"},
                {"c", {{"type", "IN"}, {"subtype", "IP4"}, {"dn", "10.0.0.4"}}},
                {"t", {0, 0}},
                {"m", "audio 49170 RTP/AVP 0 101"},
                {"a", {{"rtpmap", {"0 PCMU/8000", "101 telephone-event/8000"}}}}
            }
        }}
    };

    // 4. Serialize to wire format
    try {
        std::string sipText = sip2json::serialize(msg);
        std::cout << "Serialized SIP Output:\n\n" << sipText << std::endl;
    } catch (const sip2json_exception& e) {
        std::cerr << "Serialization error: " << e.what() << std::endl;
    }

    return 0;
}
```
