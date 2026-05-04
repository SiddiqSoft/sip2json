# sip2json API Documentation

## Table of Contents

1. [Overview](#overview)
2. [Core Classes](#core-classes)
3. [sipmessage Class](#sipmessage-class)
4. [sip2json Class](#sip2json-class)
5. [Enumerations](#enumerations)
6. [Constants](#constants)
7. [Examples](#examples)
8. [Best Practices](#best-practices)

---

## Overview

The sip2json library provides a modern C++20 API for parsing and serializing SIP (Session Initiation Protocol) messages to/from JSON format. The library is header-only and built on top of the nlohmann/json library.

### Key Features

- **Header-only library** - No compilation required
- **Modern C++20** - Leverages latest language features
- **Bidirectional conversion** - Parse SIP to JSON and serialize JSON to SIP
- **Streaming support** - Parse multiple messages asynchronously
- **SDP support** - Full encoding/decoding of Session Description Protocol
- **Rule of Five compliant** - Proper copy/move semantics
- **Const-correct** - All getters are const-qualified
- **Exception safe** - Consistent exception specifications

---

## Core Classes

### sipmessage

The `sipmessage` class represents a single SIP message (request or response). It inherits from `nlohmann::json` and provides SIP-specific methods for accessing and manipulating message components.

**Namespace**: `siddiqsoft`

**Inheritance**: `public nlohmann::json`

**Special Members**:
- Default constructor
- Copy constructor (deep copy)
- Move constructor (noexcept)
- Copy assignment operator
- Move assignment operator (noexcept)
- Destructor (defaulted)

### sip2json

The `sip2json` class is a factory/utility class providing static methods for parsing and serializing SIP messages.

**Namespace**: `siddiqsoft`

**Type**: Static utility class (no instances)

---

## sipmessage Class

### Constructors

#### Default Constructor
```cpp
sipmessage();
```
Creates an empty SIP message with metadata (version, timestamp, TTX counter).

**Example**:
```cpp
siddiqsoft::sipmessage msg;
```

#### Request Constructor
```cpp
sipmessage(const std::string& method, 
           const std::string& uri,
           const std::string& callId = "",
           uint32_t cseq = 0);
```
Creates a SIP request message.

**Parameters**:
- `method` - SIP method (e.g., "INVITE", "BYE", "REGISTER")
- `uri` - Request URI (e.g., "sip:user@example.com")
- `callId` - Call-ID header value (optional, auto-generated if empty)
- `cseq` - CSeq number (optional)

**Example**:
```cpp
siddiqsoft::sipmessage invite("INVITE", "sip:user@example.com", "call-123", 1);
```

#### Response Constructor
```cpp
sipmessage(uint16_t statusCode, 
           const sipmessage& request);
```
Creates a SIP response message from a request.

**Parameters**:
- `statusCode` - HTTP-like status code (e.g., 200, 404, 500)
- `request` - The request message to respond to

**Example**:
```cpp
siddiqsoft::sipmessage response(200, invite);
```

#### Copy Constructor
```cpp
sipmessage(const sipmessage& src);
```
Creates a deep copy of another sipmessage.

**Example**:
```cpp
siddiqsoft::sipmessage copy = original;
```

#### Move Constructor
```cpp
sipmessage(sipmessage&& src) noexcept;
```
Moves resources from a temporary sipmessage.

**Example**:
```cpp
siddiqsoft::sipmessage msg = std::move(temporary);
```

#### JSON Constructor (Explicit)
```cpp
explicit sipmessage(const nlohmann::json& src);
explicit sipmessage(nlohmann::json&& src) noexcept;
```
Creates a sipmessage from JSON objects.

**Example**:
```cpp
nlohmann::json json_obj = {...};
siddiqsoft::sipmessage msg(json_obj);
```

---

### Assignment Operators

#### Copy Assignment
```cpp
sipmessage& operator=(const sipmessage& src);
sipmessage& operator=(const nlohmann::json& src);
```
Assigns a copy of another sipmessage or JSON object.

**Example**:
```cpp
msg1 = msg2;  // Copy assignment
msg1 = json_obj;  // Assign from JSON
```

#### Move Assignment
```cpp
sipmessage& operator=(sipmessage&& src) noexcept;
sipmessage& operator=(nlohmann::json&& src) noexcept;
```
Moves resources from a temporary sipmessage or JSON object.

**Example**:
```cpp
msg1 = std::move(msg2);  // Move assignment
msg1 = std::move(json_obj);  // Move from JSON
```

---

### Header Methods

#### setHeader
```cpp
sipmessage& setHeader(const std::string& key, 
                      const std::string& value);
sipmessage& setHeader(const std::string& key, 
                      uint32_t value);
sipmessage& setHeader(const std::string& key, 
                      bool value);
```
Sets a SIP header value. Returns reference for method chaining.

**Parameters**:
- `key` - Header name (e.g., "User-Agent", "Content-Type")
- `value` - Header value (string, uint32_t, or bool)

**Example**:
```cpp
msg.setHeader("User-Agent", "MyApp/1.0")
   .setHeader("Content-Length", 0)
   .setHeader("X-Custom", "value");
```

#### getHeader
```cpp
template <class T> 
auto getHeader(const std::string& key, 
               std::optional<T> defaultValue = {}) const;
```
Gets a SIP header value with optional default.

**Template Parameters**:
- `T` - Return type (std::string, uint32_t, bool, etc.)

**Parameters**:
- `key` - Header name
- `defaultValue` - Value to return if header not found

**Returns**: Header value or default value

**Example**:
```cpp
auto userAgent = msg.getHeader<std::string>("User-Agent", "Unknown");
auto contentLength = msg.getHeader<uint32_t>("Content-Length", 0);
```

#### headers
```cpp
auto& headers();
const auto& headers() const;
```
Gets direct access to the headers object for advanced manipulation.

**Returns**: Reference to headers JSON object

**Example**:
```cpp
msg.headers()["Via"].push_back("SIP/2.0/TCP example.com");
const auto& hdrs = const_msg.headers();
```

---

### Getter Methods (Const-Correct)

All getter methods are const-qualified and can be called on const objects.

#### getMethod
```cpp
auto getMethod() const;
```
Gets the SIP method for request messages.

**Returns**: Method string (e.g., "INVITE", "BYE")

**Example**:
```cpp
auto method = msg.getMethod();  // "INVITE"
```

#### getUri
```cpp
auto getUri() const;
```
Gets the Request-URI for request messages.

**Returns**: URI string

**Example**:
```cpp
auto uri = msg.getUri();  // "sip:user@example.com"
```

#### getStatusCode
```cpp
auto getStatusCode() const;
```
Gets the status code for response messages.

**Returns**: Status code (uint16_t)

**Example**:
```cpp
auto status = msg.getStatusCode();  // 200
```

#### getReason
```cpp
auto getReason() const;
```
Gets the reason phrase for response messages.

**Returns**: Reason string (e.g., "OK", "Not Found")

**Example**:
```cpp
auto reason = msg.getReason();  // "OK"
```

#### getCallID
```cpp
auto getCallID() const;
```
Gets the Call-ID header value.

**Returns**: Call-ID string

**Example**:
```cpp
auto callId = msg.getCallID();
```

#### getUserAgent
```cpp
auto getUserAgent() const;
```
Gets the User-Agent header value.

**Returns**: User-Agent string

**Example**:
```cpp
auto ua = msg.getUserAgent();
```

#### getContentLength
```cpp
uint32_t getContentLength() const;
```
Gets the Content-Length header value.

**Returns**: Content length (uint32_t)

**Example**:
```cpp
auto len = msg.getContentLength();
```

#### getContentType
```cpp
auto getContentType() const;
```
Gets the Content-Type header value.

**Returns**: Content-Type string

**Example**:
```cpp
auto ct = msg.getContentType();  // "application/sdp"
```

#### getExpires
```cpp
uint32_t getExpires() const;
```
Gets the Expires header value.

**Returns**: Expiration time in seconds

**Example**:
```cpp
auto expires = msg.getExpires();
```

#### isMessageRequest
```cpp
bool isMessageRequest() const;
```
Checks if the message is a SIP request.

**Returns**: true if request, false otherwise

**Example**:
```cpp
if (msg.isMessageRequest()) { ... }
```

#### isMessageResponse
```cpp
bool isMessageResponse() const;
```
Checks if the message is a SIP response.

**Returns**: true if response, false otherwise

**Example**:
```cpp
if (msg.isMessageResponse()) { ... }
```

#### hasBody
```cpp
bool hasBody() const;
```
Checks if the message has a body.

**Returns**: true if body exists, false otherwise

**Example**:
```cpp
if (msg.hasBody()) { ... }
```

---

### Body Methods

#### body
```cpp
auto& body();
const auto& body() const;
```
Gets direct access to the message body for advanced manipulation.

**Returns**: Reference to body JSON object

**Example**:
```cpp
msg.body("/sdp/0/v"_json_pointer, 0);
const auto& b = const_msg.body();
```

#### getBodyElement
```cpp
template <typename T> 
T getBodyElement(const std::string& path, 
                 std::optional<T> defaultValue = {}) const;
```
Gets a specific element from the message body using JSON pointer path.

**Template Parameters**:
- `T` - Return type

**Parameters**:
- `path` - JSON pointer path (e.g., "/sdp/0/v")
- `defaultValue` - Value to return if element not found

**Returns**: Body element value or default

**Example**:
```cpp
auto version = msg.getBodyElement<int>("/sdp/0/v", 0);
```

---

## sip2json Class

### Static Methods

#### parseAsync
```cpp
static std::string& parseAsync(
    std::string& frameBuffer,
    std::function<void(sipmessage&&)> parseCallback,
    std::optional<std::function<void(const sip2json_exception&, 
                                     std::string::iterator&, 
                                     const std::string::iterator&)>> 
        errorCallback = {}) noexcept;
```
Parses multiple SIP messages from a buffer asynchronously using callbacks.

**Parameters**:
- `frameBuffer` - Buffer containing SIP messages (modified to remove parsed content)
- `parseCallback` - Callback invoked for each successfully parsed message
- `errorCallback` - Optional callback for error handling

**Returns**: Reference to modified frameBuffer

**Example**:
```cpp
std::string buffer = "INVITE sip:test@example.com SIP/2.0\r\n...";

sip2json::parseAsync(buffer,
    [](sipmessage&& msg) {
        // Process parsed message
        std::cout << "Method: " << msg.getMethod() << std::endl;
    },
    [](const sip2json_exception& err, auto& start, auto& end) {
        // Handle error
        std::cerr << "Parse error: " << err.what() << std::endl;
    });
```

#### parse
```cpp
static std::vector<sipmessage> parse(
    std::string::iterator& bufferStart,
    const std::string::iterator& bufferEnd) noexcept(false);
```
Parses multiple SIP messages from a buffer and returns them as a vector.

**Parameters**:
- `bufferStart` - Iterator to start of buffer (modified to point past parsed content)
- `bufferEnd` - Iterator to end of buffer

**Returns**: Vector of parsed sipmessage objects

**Throws**: `sip2json_exception` if parsing fails

**Example**:
```cpp
std::string buffer = "INVITE sip:test@example.com SIP/2.0\r\n...";
auto start = buffer.begin();
auto end = buffer.end();

try {
    auto messages = sip2json::parse(start, end);
    for (auto& msg : messages) {
        std::cout << "Method: " << msg.getMethod() << std::endl;
    }
} catch (const sip2json_exception& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

#### parseFromBuffer
```cpp
static sipmessage parseFromBuffer(
    std::string::iterator& bufferStart,
    const std::string::iterator& bufferEnd) noexcept(false);
```
Parses a single SIP message from a buffer.

**Parameters**:
- `bufferStart` - Iterator to start of buffer (modified to point past parsed message)
- `bufferEnd` - Iterator to end of buffer

**Returns**: Parsed sipmessage object

**Throws**: `sip2json_exception` if parsing fails

**Example**:
```cpp
std::string buffer = "INVITE sip:test@example.com SIP/2.0\r\n...";
auto start = buffer.begin();
auto end = buffer.end();

try {
    auto msg = sip2json::parseFromBuffer(start, end);
    std::cout << "Method: " << msg.getMethod() << std::endl;
} catch (const sip2json_exception& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

#### serialize
```cpp
static std::string serialize(sipmessage& sipm) noexcept(false);
```
Serializes a sipmessage object to a SIP message string.

**Parameters**:
- `sipm` - sipmessage object to serialize

**Returns**: Serialized SIP message string (CRLF-terminated)

**Throws**: `sip2json_exception` if serialization fails

**Note**: This method modifies the input to set Content-Length header.

**Example**:
```cpp
siddiqsoft::sipmessage msg("INVITE", "sip:test@example.com", "call-id", 1);
msg.setHeader("User-Agent", "MyApp/1.0");

try {
    auto sip_string = sip2json::serialize(msg);
    std::cout << sip_string << std::endl;
} catch (const sip2json_exception& e) {
    std::cerr << "Serialize error: " << e.what() << std::endl;
}
```

---

## Enumerations

### SIPMessageType

```cpp
enum class SIPMessageType
{
    request,   // SIP request message
    response   // SIP response message
};
```

---

## Constants

### SIP Methods

```cpp
constexpr std::string_view METHOD_INVITE = "INVITE";
constexpr std::string_view METHOD_ACK = "ACK";
constexpr std::string_view METHOD_BYE = "BYE";
constexpr std::string_view METHOD_CANCEL = "CANCEL";
constexpr std::string_view METHOD_REGISTER = "REGISTER";
constexpr std::string_view METHOD_OPTIONS = "OPTIONS";
constexpr std::string_view METHOD_INFO = "INFO";
constexpr std::string_view METHOD_NOTIFY = "NOTIFY";
constexpr std::string_view METHOD_SUBSCRIBE = "SUBSCRIBE";
constexpr std::string_view METHOD_MESSAGE = "MESSAGE";
```

### Content Types

```cpp
constexpr std::string_view CONTENT_TYPE_APPLICATION_SDP = "application/sdp";
constexpr std::string_view CONTENT_TYPE_TEXT_PLAIN = "text/plain";
```

### Header Field Names

```cpp
constexpr std::string_view HF_VIA = "Via";
constexpr std::string_view HF_AUTHORIZATION = "Authorization";
constexpr std::string_view HF_CONTENT_TYPE = "Content-Type";
constexpr std::string_view HF_CONTENT_LENGTH = "Content-Length";
```

---

## Examples

### Example 1: Creating and Serializing a Request

```cpp
#include "siddiqsoft/sip2json.hpp"
#include <iostream>

using namespace siddiqsoft;

int main()
{
    // Create an INVITE request
    sipmessage invite("INVITE", "sip:user@example.com", "call-123", 1);
    
    // Set headers
    invite.setHeader("From", "sip:caller@example.com")
          .setHeader("To", "sip:user@example.com")
          .setHeader("Contact", "sip:caller@example.com")
          .setHeader("User-Agent", "MyApp/1.0")
          .setHeader("Content-Type", "application/sdp")
          .setHeader("Content-Length", 0);
    
    // Add Via header (can have multiple)
    invite.headers()["Via"].push_back("SIP/2.0/TCP example.com");
    
    // Serialize to SIP message
    try {
        auto sip_message = sip2json::serialize(invite);
        std::cout << sip_message << std::endl;
    } catch (const sip2json_exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### Example 2: Parsing SIP Messages

```cpp
#include "siddiqsoft/sip2json.hpp"
#include <iostream>

using namespace siddiqsoft;

int main()
{
    std::string buffer = "INVITE sip:test@example.com SIP/2.0\r\n"
                        "Via: SIP/2.0/TCP example.com\r\n"
                        "Call-ID: call-123\r\n"
                        "CSeq: 1 INVITE\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n";
    
    auto start = buffer.begin();
    auto end = buffer.end();
    
    try {
        auto msg = sip2json::parseFromBuffer(start, end);
        
        std::cout << "Method: " << msg.getMethod() << std::endl;
        std::cout << "URI: " << msg.getUri() << std::endl;
        std::cout << "Call-ID: " << msg.getCallID() << std::endl;
        
    } catch (const sip2json_exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### Example 3: Creating a Response

```cpp
#include "siddiqsoft/sip2json.hpp"
#include <iostream>

using namespace siddiqsoft;

int main()
{
    // Parse incoming request
    std::string buffer = "INVITE sip:test@example.com SIP/2.0\r\n...";
    auto start = buffer.begin();
    auto end = buffer.end();
    
    auto request = sip2json::parseFromBuffer(start, end);
    
    // Create response
    sipmessage response(200, request);
    response.setHeader("User-Agent", "MyApp/1.0");
    
    // Serialize response
    try {
        auto sip_message = sip2json::serialize(response);
        std::cout << sip_message << std::endl;
    } catch (const sip2json_exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### Example 4: Async Parsing with Callbacks

```cpp
#include "siddiqsoft/sip2json.hpp"
#include <iostream>

using namespace siddiqsoft;

int main()
{
    std::string buffer = "INVITE sip:test@example.com SIP/2.0\r\n..."
                        "BYE sip:test@example.com SIP/2.0\r\n...";
    
    sip2json::parseAsync(buffer,
        [](sipmessage&& msg) {
            // Process each parsed message
            std::cout << "Parsed: " << msg.getMethod() << std::endl;
        },
        [](const sip2json_exception& err, auto& start, auto& end) {
            // Handle errors
            std::cerr << "Parse error: " << err.what() << std::endl;
        });
    
    return 0;
}
```

### Example 5: Working with SDP Body

```cpp
#include "siddiqsoft/sip2json.hpp"
#include <iostream>

using namespace siddiqsoft;

int main()
{
    sipmessage msg("INVITE", "sip:test@example.com", "call-id", 1);
    
    // Set SDP body
    msg.setHeader("Content-Type", "application/sdp");
    msg.body("/sdp/0/v"_json_pointer, 0);
    msg.body("/sdp/0/s"_json_pointer, "Session");
    msg.body("/sdp/0/o"_json_pointer, nlohmann::json{
        {"user", "user"},
        {"t1", "123456"},
        {"t2", "654321"},
        {"type", "IN"},
        {"subtype", "IP4"},
        {"host", "example.com"}
    });
    
    // Get SDP elements
    auto version = msg.getBodyElement<int>("/sdp/0/v", 0);
    auto session = msg.getBodyElement<std::string>("/sdp/0/s", "");
    
    std::cout << "SDP Version: " << version << std::endl;
    std::cout << "Session: " << session << std::endl;
    
    return 0;
}
```

---

## Best Practices

### 1. Use Const-Correct Code

```cpp
// ✅ Good: Use const for read-only access
const sipmessage& msg = getMessageFromNetwork();
auto method = msg.getMethod();
const auto& headers = msg.headers();

// ❌ Avoid: Non-const access when not needed
sipmessage& msg = getMessageFromNetwork();
auto method = msg.getMethod();
```

### 2. Use Move Semantics

```cpp
// ✅ Good: Use move for efficiency
std::vector<sipmessage> messages;
messages.push_back(std::move(msg));

// ❌ Avoid: Unnecessary copies
std::vector<sipmessage> messages;
messages.push_back(msg);  // Copies instead of moves
```

### 3. Handle Exceptions

```cpp
// ✅ Good: Catch and handle exceptions
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}

// ❌ Avoid: Ignoring exceptions
auto msg = sip2json::parseFromBuffer(start, end);
```

### 4. Use Method Chaining

```cpp
// ✅ Good: Chain setter methods
msg.setHeader("From", "sip:test@example.com")
   .setHeader("To", "sip:user@example.com")
   .setHeader("User-Agent", "MyApp/1.0");

// ❌ Avoid: Separate calls
msg.setHeader("From", "sip:test@example.com");
msg.setHeader("To", "sip:user@example.com");
msg.setHeader("User-Agent", "MyApp/1.0");
```

### 5. Use Async Parsing for Multiple Messages

```cpp
// ✅ Good: Use parseAsync for streaming
sip2json::parseAsync(buffer,
    [](sipmessage&& msg) {
        // Process message
    },
    [](const sip2json_exception& err, auto& start, auto& end) {
        // Handle error
    });

// ⚠️ Alternative: Use parse() for batch processing
auto messages = sip2json::parse(start, end);
```

### 6. Use JSON Pointers for Body Access

```cpp
// ✅ Good: Use JSON pointers for clarity
auto version = msg.getBodyElement<int>("/sdp/0/v", 0);
msg.body("/sdp/0/s"_json_pointer, "Session");

// ⚠️ Alternative: Direct JSON access
auto version = msg["b"]["sdp"][0]["v"].get<int>();
```

---

## Exception Handling

The library throws `sip2json_exception` for parsing and serialization errors:

```cpp
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    // Handle SIP-specific errors
    std::cerr << "SIP Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    // Handle other errors
    std::cerr << "Error: " << e.what() << std::endl;
}
```

---

## Performance Considerations

1. **Use Move Semantics**: Always use `std::move()` when passing temporary sipmessage objects
2. **Const References**: Use `const sipmessage&` for read-only access to avoid copies
3. **Async Parsing**: Use `parseAsync()` for streaming scenarios to avoid buffering entire messages
4. **JSON Pointers**: Use JSON pointers for efficient body element access

---

## Thread Safety

The library is **not thread-safe**. If using in a multi-threaded environment:
- Each thread should have its own sipmessage instances
- Use synchronization primitives (mutex, etc.) when sharing messages between threads
- The underlying nlohmann::json is not thread-safe for concurrent modifications

---

## Backward Compatibility

All improvements maintain 100% backward compatibility:
- Existing code continues to work without modification
- New const-correct methods are additions only
- Copy/move semantics improvements are transparent to users

---

**Last Updated**: 2024  
**Version**: 2.0+  
**Status**: Production Ready ✅
