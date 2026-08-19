# Asynchronous Stream Parsing Example

This example demonstrates handling incoming network data streams containing multiple SIP frames.

```cpp
#include <iostream>
#include <string>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

void handleNetworkInput(std::string& socketReadBuffer)
{
    auto cursor = socketReadBuffer.begin();

    sip2json::parseAsync(
        cursor,
        socketReadBuffer.end(),
        [](sipmessage&& msg) {
            std::cout << "[SIP Message Received]\n";
            std::cout << "  Type: " << msg.type << "\n";
            if (msg.type == "request") {
                std::cout << "  Method: " << msg.method << " " << msg.uri << "\n";
            } else {
                std::cout << "  Status: " << msg.responseCode << " " << msg.reason << "\n";
            }
            std::cout << "  Call-ID: " << msg.callid << "\n";
        },
        [](sip2jsonErrors& err, const std::string& info) {
            std::cerr << "[Parser Notification] " << info << "\n";
        }
    );

    // Erase consumed frames from beginning of buffer
    socketReadBuffer.erase(socketReadBuffer.begin(), cursor);
}

int main()
{
    std::string sampleBuffer = 
        "REGISTER sip:example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK-123\r\n"
        "From: <sip:user@example.com>;tag=111\r\n"
        "To: <sip:user@example.com>\r\n"
        "Call-ID: reg-call-101@192.168.1.1\r\n"
        "CSeq: 1 REGISTER\r\n"
        "Content-Length: 0\r\n\r\n";

    handleNetworkInput(sampleBuffer);
    return 0;
}
```
