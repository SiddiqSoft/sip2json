# `sipmessage` Class Reference

The `siddiqsoft::sipmessage` class represents a parsed or constructed SIP request or response message. It extends `nlohmann::json` to provide structured JSON semantics with native C++ accessors.

```cpp
#include "siddiqsoft/sipmessage.hpp"
```

---

## Performance Best Practice

!!! tip "Zero-Allocation Static Constants"
    For maximum throughput and zero-allocation key lookups, use the pre-defined library constants (`siddiqsoft::METHOD_*`, `siddiqsoft::HF_*`, `siddiqsoft::CONTENT_TYPE_*`). Passing static string constants yields a **13.2% performance gain for `setHeader`** and a **4.8% gain for `getHeader`** over raw string literals.

---

## Constructors

### 1. Default Constructor
Initializes an empty `sipmessage` populated with standard metadata (`meta` object with version, timestamp, and TTX parse duration).

```cpp
sipmessage();
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    sipmessage msg;
    std::cout << "Schema Version: " << msg.value("/meta/version"_json_pointer, "") << std::endl;
    std::cout << "Timestamp     : " << msg.value("/meta/time"_json_pointer, "") << std::endl;
    ```

---

### 2. Request Constructor
Instantiates a new SIP request message initialized with method, Request-URI, optional Call-ID, and optional CSeq number.

```cpp
sipmessage(const std::string& method,
           const std::string& uri,
           const std::string& callId = {},
           uint32_t cseq = 0);
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    // Create an INVITE request with Call-ID and CSeq 100
    sipmessage req(METHOD_INVITE, "sip:alice@biloxi.com", "call-id-99812", 100);

    req.setHeader(HF_FROM, "sip:bob@biloxi.com;tag=8831")
       .setHeader(HF_TO, "sip:alice@biloxi.com")
       .setHeader(HF_CONTACT, "<sip:bob@10.0.0.4:5060>");

    std::cout << "Method: " << req.getMethod() << "\nURI: " << req.getUri() << std::endl;
    ```

---

### 3. Response Constructor (from Status Code)
Instantiates a new standalone SIP response message with the specified integer status code and automatically maps standard RFC 3261 reason phrases.

```cpp
explicit sipmessage(uint32_t statusCode);
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    // Create a standalone 200 OK response
    sipmessage resp(200);

    resp.setHeader(HF_CALLID, "call-id-99812")
        .setHeader(HF_CSEQ, "100 INVITE")
        .setUserAgent("MyProxy/2.0");

    std::cout << "Status: " << resp.getStatusCode() << " " << resp.getReason() << std::endl;
    ```

---

### 4. Response Constructor (from Request)
Instantiates a SIP response message copying the routing headers (`Call-ID`, `From`, `To`, `CSeq`, `Via`) from an incoming request message and updating the start-line to a response.

```cpp
sipmessage(uint32_t statusCode, const sipmessage& request);
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void handleIncomingRequest(const sipmessage& incomingReq)
    {
        // Construct a 200 OK response copying header context from incoming request
        sipmessage okResp(200, incomingReq);

        // Add server-specific response headers
        okResp.setHeader(HF_CONTACT, "<sip:server@10.0.0.1:5060>")
              .setUserAgent("sip2json-Proxy/2.6");

        std::string wire = sip2json::serialize(okResp);
        std::cout << "Response Wire:\n" << wire << std::endl;
    }
    ```

---

### 5. Direct JSON Constructors
Instantiates a `sipmessage` directly from an existing `nlohmann::json` document conforming to the `sip2json` schema.

```cpp
explicit sipmessage(const nlohmann::json& src);
explicit sipmessage(nlohmann::json&& src) noexcept;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    // Define message structure via JSON literal
    nlohmann::json doc = {
        {"s", {
            {"type", "request"},
            {"method", "BYE"},
            {"uri", "sip:bob@biloxi.com"},
            {"version", "SIP/2.0"}
        }},
        {"h", {
            {"Call-ID", "call-id-99812"},
            {"CSeq", "101 BYE"},
            {"From", "sip:alice@biloxi.com;tag=8831"},
            {"To", "sip:bob@biloxi.com;tag=9912"}
        }}
    };

    sipmessage byeMsg(std::move(doc));
    std::string wireFormat = sip2json::serialize(byeMsg);
    std::cout << wireFormat << std::endl;
    ```

---

## Header Accessors & Mutators

### `getHeader`
Retrieves a header value by key with an optional fallback default value. Overloaded for `std::string`, `const char*`, and `std::string_view`.

```cpp
template <class T = std::string>
auto getHeader(const std::string& key, std::optional<T> defaultValue = {}) const;

template <class T = std::string>
auto getHeader(const char* key, std::optional<T> defaultValue = {}) const;

template <class T = std::string>
auto getHeader(std::string_view key, std::optional<T> defaultValue = {}) const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void inspectHeaders(const sipmessage& msg)
    {
        // Retrieve string header
        std::string callId = msg.getHeader(HF_CALLID);

        // Retrieve integer header with default fallback
        uint32_t maxForwards = msg.getHeader<uint32_t>(HF_MAX_FORWARDS, 70);

        // Retrieve custom header with default fallback
        std::string route = msg.getHeader("X-Custom-Route", std::string("direct"));

        std::cout << "Call-ID: " << callId << ", Max-Forwards: " << maxForwards << std::endl;
    }
    ```

---

### `setHeader`
Sets or updates a header key-value pair. Returns a reference to `*this` to support method chaining.

```cpp
template <typename T> sipmessage& setHeader(const std::string& key, const T& value);
template <typename T> sipmessage& setHeader(const char* key, const T& value);
template <typename T> sipmessage& setHeader(std::string_view key, const T& value);
sipmessage& setHeader(const nlohmann::json& jsonPatch);
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    sipmessage msg(METHOD_OPTIONS, "sip:gateway.example.com");

    // Fluent method chaining
    msg.setHeader(HF_FROM, "sip:monitor@example.com")
       .setHeader(HF_TO, "sip:gateway.example.com")
       .setHeader(HF_ACCEPT, "application/sdp")
       .setHeader(HF_MAX_FORWARDS, 70)
       .setHeader("X-Audit-ID", 99812);

    // Merge-patch multiple headers via JSON
    msg.setHeader(nlohmann::json{
        {"X-Cluster-Node", "east-us-node-04"},
        {"X-Trace-Level", "verbose"}
    });
    ```

---

### `headers`
Returns direct reference or const reference to the internal `"h"` JSON headers object.

```cpp
nlohmann::json& headers();
const nlohmann::json& headers() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void listAllHeaders(const sipmessage& msg)
    {
        for (const auto& [key, value] : msg.headers().items()) {
            std::cout << "Header [" << key << "] = " << value << "\n";
        }
    }
    ```

---

### Convenience Header Getters & Setters

```cpp
// User-Agent
auto& setUserAgent(const std::string& ua = {});
auto getUserAgent() const;

// Common Headers
uint32_t getContentLength() const;
uint32_t getExpires() const;
auto getContentType() const;
auto getCallID() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    sipmessage msg(METHOD_REGISTER, "sip:registrar.example.com");
    msg.setUserAgent("MySoftphone/1.2")
       .setHeader(HF_EXPIRES, 3600);

    std::cout << "User-Agent: " << msg.getUserAgent() << "\n";
    std::cout << "Expires   : " << msg.getExpires() << " seconds\n";
    ```

---

## Start-Line Accessors & Inspection

### `getMethod` & `getUri`
Retrieves the SIP method string (e.g., `INVITE`, `BYE`) and Request-URI for request messages.

```cpp
auto getMethod() const;
auto getUri() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void checkRequest(const siddiqsoft::sipmessage& req)
    {
        if (req.getMethod() == siddiqsoft::METHOD_INVITE) {
            std::cout << "Incoming call to target: " << req.getUri() << std::endl;
        }
    }
    ```

---

### `getStatusCode` & `getReason`
Retrieves the integer status code and reason phrase for response messages.

```cpp
auto getStatusCode() const;
auto getReason() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void checkResponse(const siddiqsoft::sipmessage& resp)
    {
        std::cout << "Received Status: " << resp.getStatusCode() 
                  << " (" << resp.getReason() << ")" << std::endl;
    }
    ```

---

### `isMessageRequest` & `isMessageResponse`
Inspects whether the message represents a SIP Request or a SIP Response.

```cpp
bool isMessageRequest() const;
bool isMessageResponse() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void routeMessage(const siddiqsoft::sipmessage& msg)
    {
        if (msg.isMessageRequest()) {
            std::cout << "Processing Request: " << msg.getMethod() << std::endl;
        } else if (msg.isMessageResponse()) {
            std::cout << "Processing Response: " << msg.getStatusCode() << std::endl;
        }
    }
    ```

---

## Zero-Copy String View Accessors

These accessors return `std::string_view` pointing directly into internal JSON string storage with **zero heap allocations**:

```cpp
std::string_view getMethodView() const;
std::string_view getUriView() const;
std::string_view getReasonView() const;
std::string_view getCallIDView() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include <string_view>
    #include "siddiqsoft/sip2json.hpp"

    void fastMatch(const siddiqsoft::sipmessage& msg)
    {
        // Zero-copy string view lookup
        std::string_view method = msg.getMethodView();
        std::string_view uri    = msg.getUriView();
        std::string_view callId = msg.getCallIDView();

        if (method == "INVITE" && uri.starts_with("sip:support")) {
            std::cout << "Priority routing for Call-ID: " << callId << std::endl;
        }
    }
    ```

---

## Body & SDP Accessors

### `hasBody` & `body`
Checks for the presence of body payload and provides direct reference access to the `"b"` JSON body container.

```cpp
bool hasBody() const;
nlohmann::json& body();
const nlohmann::json& body() const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void inspectBody(const siddiqsoft::sipmessage& msg)
    {
        if (msg.hasBody()) {
            std::cout << "Body payload:\n" << msg.body().dump(2) << std::endl;
        }
    }
    ```

---

### `setBody` & `getBodyElement`
Sets or extracts specific nested elements inside the body using `nlohmann::json::json_pointer`.

```cpp
template <typename T>
sipmessage& setBody(const nlohmann::json::json_pointer& key, const T& value);

sipmessage& setBody(const nlohmann::json& jsonBody);

template <typename T>
T getBodyElement(const nlohmann::json::json_pointer& jp, const T& defaultValue) const;
```

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    sipmessage msg(METHOD_INVITE, "sip:bob@example.com");

    // Set SDP attributes via JSON pointers
    msg.setBody("/sdp/0/v"_json_pointer, 0)
       .setBody("/sdp/0/s"_json_pointer, "Live Call")
       .setBody("/sdp/0/c/dn"_json_pointer, "192.0.2.1")
       .setBody("/sdp/0/m"_json_pointer, "audio 49170 RTP/AVP 0");

    // Retrieve SDP elements with default fallbacks
    std::string sessionName = msg.getBodyElement<std::string>("/sdp/0/s"_json_pointer, "Unknown");
    std::string connIp      = msg.getBodyElement<std::string>("/sdp/0/c/dn"_json_pointer, "0.0.0.0");

    std::cout << "Session: " << sessionName << " on IP: " << connIp << std::endl;
    ```
