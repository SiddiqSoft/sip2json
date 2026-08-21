# `sipmessage` Class Reference

The `sipmessage` class represents a parsed or constructed SIP request or response message.

---

## Member Variables

```cpp
struct sipmessage {
    std::string type;       // "request" or "response"
    std::string method;     // Standard SIP method: "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER", "SUBSCRIBE", "NOTIFY", "MESSAGE", "INFO", "REFER", "PUBLISH", "UPDATE", "PRACK"
    std::string uri;        // Request URI (e.g. "sip:user@example.com")
    int responseCode{0};    // Response status code (e.g. 200, 404, 180)
    std::string reason;     // Response reason phrase (e.g. "OK", "Not Found")
    std::string version{"SIP/2.0"};
    
    std::string callid;     // Call-ID header value
    uint32_t cseq{0};       // CSeq sequence number
    
    nlohmann::json headers; // JSON object storing all message headers (RFC 3261 case-insensitive matching)
    nlohmann::json body;    // JSON object storing payload body (e.g. SDP)
};
```

---

## Constructors

### Default Constructor

```cpp
sipmessage();
```

Creates an empty `sipmessage` instance.

### Request Constructor

```cpp
sipmessage(const std::string& reqMethod,
           const std::string& reqUri,
           const std::string& callId = "",
           uint32_t cseqNumber = 0);
```

Initializes a request message with specified method, URI, Call-ID, and CSeq.

---

## Methods

### `setHeader`

```cpp
sipmessage& setHeader(const std::string& name, const std::string& value);
```

Sets or updates a header key-value pair. Returns a reference to `*this` to support method chaining.

### `empty`

```cpp
bool empty() const noexcept;
```

Returns `true` if the message is uninitialized or empty.

### Implicit JSON Conversion

```cpp
operator nlohmann::json() const;
```

Converts the message to its compact `nlohmann::json` schema representation.
