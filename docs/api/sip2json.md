# `sip2json` Parser & Serializer Functions

The `siddiqsoft::sip2json` class provides static utility functions for parsing continuous streams, individual message buffers, and serializing `sipmessage` objects into standard RFC 3261 wire format.

```cpp
#include "siddiqsoft/sip2json.hpp"
```

---

## 1. `parseAsync` (Stream Parsing)

Processes continuous network stream buffers (TCP/TLS/UDP). Decodes all complete SIP frames and invokes `parseCallback(sipmessage&&)` for each message. Automatically trims parsed bytes from `frameBuffer`, leaving partial frames intact for the next socket read cycle.

```cpp
static std::string& parseAsync(
    std::string& frameBuffer,
    std::function<void(sipmessage&&)> parseCallback,
    std::optional<std::function<void(const sip2json_exception&,
                                     std::string::iterator&,
                                     const std::string::iterator&)>> errorCallback = {}) noexcept;
```

#### Parameters
* **`frameBuffer`**: In-out reference to the incoming stream buffer. Parsed frames are erased; partial frames remain at the front.
* **`parseCallback`**: Callback invoked with an rvalue reference `sipmessage&&` for every successfully decoded SIP message.
* **`errorCallback`**: Optional callback invoked on malformed frames or framing errors, providing the exception and iterator location.

=== "Example"
    ```cpp
    #include <iostream>
    #include <string>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void onSocketData(std::string& socketBuffer)
    {
        // Asynchronously decode all complete SIP messages in the socket buffer
        sip2json::parseAsync(
            socketBuffer,
            [](sipmessage&& msg) {
                std::cout << "[SIP Message Decoded]\n";
                if (msg.isMessageRequest()) {
                    std::cout << "  Request : " << msg.getMethod() << " " << msg.getUri() << "\n";
                } else if (msg.isMessageResponse()) {
                    std::cout << "  Response: " << msg.getStatusCode() << " " << msg.getReason() << "\n";
                }
                std::cout << "  Call-ID : " << msg.getCallID() << "\n";
                std::cout << "  From    : " << msg.getHeader(HF_FROM) << "\n";
            },
            [](const sip2json_exception& ex, std::string::iterator& errStart, const std::string::iterator& errEnd) {
                std::cerr << "Parser Warning: " << ex.what() << "\n";
            }
        );

        // socketBuffer now contains only remaining unparsed partial frame data
        std::cout << "Remaining unparsed bytes in buffer: " << socketBuffer.size() << std::endl;
    }
    ```

---

## 2. `parseFromBuffer` (Single Message Parsing)

Extracts and deserializes a single SIP message from the iterator range `[bufferStart, bufferEnd)` and advances `bufferStart` past the end of the decoded message.

```cpp
static sipmessage parseFromBuffer(
    std::string::iterator& bufferStart,
    const std::string::iterator& bufferEnd) noexcept(false);
```

#### Parameters
* **`bufferStart`**: In-out iterator pointing to the start of the message. Advanced past the parsed message on success.
* **`bufferEnd`**: Iterator pointing to the end of the buffer.

#### Throws
* `invalid_startline_error`: If the start-line is malformed or uses an invalid method.
* `incomplete_buffer_for_parse_error`: If the buffer contains an incomplete header or body.
* `sip2json_exception`: On general syntax or framing errors.

=== "Example"
    ```cpp
    #include <iostream>
    #include <string>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void parseDatagramPacket(std::string packetData)
    {
        auto itStart = packetData.begin();
        auto itEnd   = packetData.end();

        try {
            sipmessage msg = sip2json::parseFromBuffer(itStart, itEnd);

            std::cout << "Parsed Message Type: " << (msg.isMessageRequest() ? "Request" : "Response") << "\n";
            std::cout << "Call-ID            : " << msg.getCallID() << "\n";
            std::cout << "CSeq               : " << msg.getHeader(HF_CSEQ) << "\n";
            std::cout << "Bytes Consumed     : " << std::distance(packetData.begin(), itStart) << " bytes\n";
        }
        catch (const sip2json_exception& ex) {
            std::cerr << "SIP Parse Error: " << ex.what() << std::endl;
        }
    }
    ```

---

## 3. `parse` (Batch Multi-Message Parsing)

Parses as many SIP messages as possible from the iterator range `[bufferStart, bufferEnd)`. Advances `bufferStart` past all parsed messages and returns a `std::vector<sipmessage>`.

```cpp
static std::vector<sipmessage> parse(
    std::string::iterator& bufferStart,
    const std::string::iterator& bufferEnd) noexcept(false);
```

#### Parameters
* **`bufferStart`**: In-out iterator pointing to the beginning of the buffer. Advanced past all successfully parsed messages.
* **`bufferEnd`**: Iterator pointing to the end of the buffer.

#### Throws
* `invalid_document_error`: If no messages could be successfully decoded from the buffer.

=== "Example"
    ```cpp
    #include <iostream>
    #include <vector>
    #include <string>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void processBatchLog(std::string multiMessageLog)
    {
        auto start = multiMessageLog.begin();
        auto end   = multiMessageLog.end();

        try {
            std::vector<sipmessage> messages = sip2json::parse(start, end);
            std::cout << "Successfully batch-parsed " << messages.size() << " SIP messages.\n";

            for (size_t i = 0; i < messages.size(); ++i) {
                std::cout << "  [" << i + 1 << "] " 
                          << (messages[i].isMessageRequest() ? messages[i].getMethod() : std::to_string(messages[i].getStatusCode()))
                          << " - " << messages[i].getCallID() << "\n";
            }
        }
        catch (const sip2json_exception& ex) {
            std::cerr << "Batch Parse Failure: " << ex.what() << std::endl;
        }
    }
    ```

---

## 4. `serialize` (Wire Format Serialization)

Serializes a `sipmessage` object into a standard RFC 3261 wire-formatted SIP string complete with request/status line, formatted headers, and SDP payload body.

```cpp
static std::string serialize(sipmessage& msg) noexcept(false);
```

#### Parameters
* **`msg`**: The `sipmessage` to serialize. Automatically calculates and updates the `Content-Length` header if body content is present.

#### Throws
* `empty_message_error`: If the `sipmessage` is empty.
* `invalid_document_error`: If the start-line is invalid, method is unsupported, or headers contain illegal CRLF characters.

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main()
    {
        // 1. Construct Request Message
        sipmessage req(METHOD_INVITE, "sip:bob@biloxi.com", "call-id-99812", 1);
        req.setHeader(HF_FROM, "sip:alice@biloxi.com;tag=a831")
           .setHeader(HF_TO, "sip:bob@biloxi.com")
           .setHeader(HF_CONTACT, "<sip:alice@10.0.0.4:5060>")
           .setHeader(HF_CONTENT_TYPE, CONTENT_TYPE_APP_SDP);

        // 2. Attach SDP Media Description
        req.setBody("/sdp/0/v"_json_pointer, 0)
           .setBody("/sdp/0/s"_json_pointer, "Call")
           .setBody("/sdp/0/c/dn"_json_pointer, "10.0.0.4")
           .setBody("/sdp/0/m"_json_pointer, "audio 49170 RTP/AVP 0");

        // 3. Serialize to RFC 3261 Wire Format
        try {
            std::string rawWireSip = sip2json::serialize(req);
            std::cout << "--- Wire Formatted SIP Message ---\n";
            std::cout << rawWireSip << std::endl;
        }
        catch (const sip2json_exception& ex) {
            std::cerr << "Serialization Failed: " << ex.what() << std::endl;
        }

        return 0;
    }
    ```
