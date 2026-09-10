# siddiqsoft::sipmessage Class Reference

<div class="api-header-block">
  <div class="api-module-name">Namespace siddiqsoft</div>
  <div class="api-header-file">Inherits <strong>public nlohmann::json</strong> &bull; #include &lt;siddiqsoft/sipmessage.hpp&gt;</div>
</div>

Represents a Session Initiation Protocol (SIP) message with native JSON serialization. Provides accessors for start-line / status-line fields, standard RFC 3261 headers, SDP bodies, and metadata tracking.

## Class Hierarchy & Inheritance

The following UML class diagram highlights `siddiqsoft::sipmessage` within the system architecture. Each node links directly to its source header file on GitHub:

<!-- @@uml-diag:sipmessage -->

## Member Functions Summary

### Constructors

<table class="api-summary-table">
  <tr>
    <td class="memtype"></td>
    <td class="memitemleft"><a href="#default-constructor"><strong>sipmessage</strong></a> ()<div class="mdesc">Default constructor initializing an empty SIP message with standard metadata.</div></td>
  </tr>
  <tr>
    <td class="memtype"></td>
    <td class="memitemleft"><a href="#request-constructor"><strong>sipmessage</strong></a> (
      <div class="param-wrap">const std::string&amp; method,</div>
      <div class="param-wrap">const std::string&amp; uri,</div>
      <div class="param-wrap">const std::string&amp; callId = {},</div>
      <div class="param-wrap">uint32_t cseq = 0</div>
    )<div class="mdesc">Constructs a SIP request message with method, URI, Call-ID, and CSeq.</div></td>
  </tr>
  <tr>
    <td class="memtype"></td>
    <td class="memitemleft"><a href="#response-constructor"><strong>sipmessage</strong></a> (uint32_t statusCode)<div class="mdesc">Constructs a SIP response message with status code and standard reason phrase.</div></td>
  </tr>
  <tr>
    <td class="memtype"></td>
    <td class="memitemleft"><a href="#json-constructors"><strong>sipmessage</strong></a> (const nlohmann::json&amp; src)<div class="mdesc">Initializes sipmessage from an existing JSON document.</div></td>
  </tr>
</table>

### Start-Line & Status-Line Accessors

<table class="api-summary-table">
  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getmethod"><strong>getMethod</strong></a> () const<div class="mdesc">Returns the SIP request method string.</div></td></tr>
  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getmethodview"><strong>getMethodView</strong></a> () const<div class="mdesc">Returns zero-copy view of request method string from internal JSON storage.</div></td></tr>
  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#geturi"><strong>getUri</strong></a> () const<div class="mdesc">Returns the SIP request URI string.</div></td></tr>
  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#geturiview"><strong>getUriView</strong></a> () const<div class="mdesc">Returns zero-copy view of request URI string from internal JSON storage.</div></td></tr>
  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getstatuscode"><strong>getStatusCode</strong></a> () const<div class="mdesc">Returns numeric response status code.</div></td></tr>
  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getreason"><strong>getReason</strong></a> () const<div class="mdesc">Returns response reason phrase string.</div></td></tr>
  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getreasonview"><strong>getReasonView</strong></a> () const<div class="mdesc">Returns zero-copy view of response reason phrase.</div></td></tr>
  <tr><td class="memtype"><code>bool</code></td><td class="memitemleft"><a href="#ismessagerequest"><strong>isMessageRequest</strong></a> () const<div class="mdesc">Returns true if message represents a SIP request.</div></td></tr>
  <tr><td class="memtype"><code>bool</code></td><td class="memitemleft"><a href="#ismessageresponse"><strong>isMessageResponse</strong></a> () const<div class="mdesc">Returns true if message represents a SIP response.</div></td></tr>
</table>

### Header Management

<table class="api-summary-table">
  <tr><td class="memtype"><code>auto&amp;</code></td><td class="memitemleft"><a href="#headers"><strong>headers</strong></a> ()<div class="mdesc">Provides direct reference to the /h headers JSON object.</div></td></tr>
  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getheader"><strong>getHeader</strong></a> (
    <div class="param-wrap">const std::string&amp; key,</div>
    <div class="param-wrap">std::optional&lt;T&gt; defaultValue = {}</div>
  ) const<div class="mdesc">Retrieves header value matching key, converting to type T.</div></td></tr>
  <tr><td class="memtype"><code>sipmessage&amp;</code></td><td class="memitemleft"><a href="#setheader"><strong>setHeader</strong></a> (
    <div class="param-wrap">const std::string&amp; key,</div>
    <div class="param-wrap">const T&amp; v</div>
  )<div class="mdesc">Sets or updates header key-value pair. Returns *this for chaining.</div></td></tr>
  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getcallid"><strong>getCallID</strong></a> () const<div class="mdesc">Returns the Call-ID header value.</div></td></tr>
  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getcallidview"><strong>getCallIDView</strong></a> () const<div class="mdesc">Returns zero-copy view of Call-ID header.</div></td></tr>
  <tr><td class="memtype"><code>std::string</code></td><td class="memitemleft"><a href="#getcontenttype"><strong>getContentType</strong></a> () const<div class="mdesc">Returns Content-Type header string.</div></td></tr>
  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getcontenttypeview"><strong>getContentTypeView</strong></a> () const<div class="mdesc">Returns zero-copy view of Content-Type header.</div></td></tr>
  <tr><td class="memtype"><code>uint32_t</code></td><td class="memitemleft"><a href="#getcontentlength"><strong>getContentLength</strong></a> () const<div class="mdesc">Parses and returns Content-Length as integer.</div></td></tr>
  <tr><td class="memtype"><code>uint32_t</code></td><td class="memitemleft"><a href="#getexpires"><strong>getExpires</strong></a> () const<div class="mdesc">Parses and returns Expires header as integer.</div></td></tr>
</table>

### Body & SDP Management

<table class="api-summary-table">
  <tr><td class="memtype"><code>auto&amp;</code></td><td class="memitemleft"><a href="#body"><strong>body</strong></a> ()<div class="mdesc">Provides direct reference to the /b body JSON object.</div></td></tr>
  <tr><td class="memtype"><code>bool</code></td><td class="memitemleft"><a href="#hasbody"><strong>hasBody</strong></a> () const<div class="mdesc">Returns true if message body contains data.</div></td></tr>
  <tr><td class="memtype"><code>T</code></td><td class="memitemleft"><a href="#getbodyelement"><strong>getBodyElement</strong></a> (
    <div class="param-wrap">const nlohmann::json::json_pointer&amp; jp,</div>
    <div class="param-wrap">const T&amp; defaultValue</div>
  ) const<div class="mdesc">Queries property within body JSON tree via RFC 6901 JSON pointer.</div></td></tr>
  <tr><td class="memtype"><code>sipmessage&amp;</code></td><td class="memitemleft"><a href="#setbody"><strong>setBody</strong></a> (
    <div class="param-wrap">const nlohmann::json::json_pointer&amp; jp,</div>
    <div class="param-wrap">const T&amp; v</div>
  )<div class="mdesc">Sets value at specified JSON pointer path within the body object.</div></td></tr>
</table>

## Detailed Description

The `sipmessage` class represents a SIP message as a first-class JSON object by extending `nlohmann::json`. The internal structure partitions the message into standardized components:

* `/s`: Start line / status line container (`method`, `uri`, `version`, `statusCode`, `reasonPhrase`).
* `/h`: Headers dictionary mapping canonical header names to their wire values.
* `/b`: Body dictionary containing unstructured payload strings or structured SDP arrays (`/b/sdp`).
* `/meta`: Diagnostic and provenance metadata (`version`, timestamp, parse duration `ttx`).

## Member Function Documentation

### Constructors

<div class="memitem" id="default-constructor" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">sipmessage()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
siddiqsoft::sipmessage::sipmessage();
```

</div>
<div class="memdoc" markdown="1">

Default constructor initializing an empty SIP message with standard metadata (version, timestamp, TTX counter).

<div class="memdoc-section-title">Returns</div>

<code>sipmessage</code> &mdash; An initialized empty SIP message instance.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L28-L34
siddiqsoft::sipmessage msg;

EXPECT_TRUE(msg.contains("meta"));
EXPECT_FALSE(msg.value("/meta/version"_json_pointer, std::string {}).empty());
EXPECT_FALSE(msg.value("/meta/time"_json_pointer, std::string {}).empty());
EXPECT_EQ(0, msg.value("/meta/ttx"_json_pointer, -1));
```

</div>
</div>

<div class="memitem" id="request-constructor" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">sipmessage(request)</h4>
</div>
<div class="memproto" markdown="1">

```cpp
siddiqsoft::sipmessage::sipmessage(
    const std::string& method,
    const std::string& uri,
    const std::string& callId = {},
    uint32_t cseq = 0
);
```

</div>
<div class="memdoc" markdown="1">

Constructs a SIP request message with method, request URI, Call-ID, and CSeq.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>const std::string&amp;</code></td>
    <td class="paramname">method</td>
    <td class="paramdesc">SIP method string (e.g. `INVITE`, `REGISTER`).</td>
  </tr>
  <tr>
    <td class="paramtype"><code>const std::string&amp;</code></td>
    <td class="paramname">uri</td>
    <td class="paramdesc">Target SIP request URI.</td>
  </tr>
  <tr>
    <td class="paramtype"><code>const std::string&amp;</code></td>
    <td class="paramname">callId</td>
    <td class="paramdesc">Optional Call-ID identifier string.</td>
  </tr>
  <tr>
    <td class="paramtype"><code>uint32_t</code></td>
    <td class="paramname">cseq</td>
    <td class="paramdesc">Optional initial sequence number.</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>sipmessage</code> &mdash; A populated SIP request message instance.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L40-L49
siddiqsoft::sipmessage original(siddiqsoft::METHOD_INVITE, "sip:test@example.com", "call-id-123", 1);
original.setHeader("X-Custom", "original-value");

EXPECT_EQ(original.getCallID(), "call-id-123");
EXPECT_EQ(original.getMethod(), siddiqsoft::METHOD_INVITE);
EXPECT_TRUE(original.isMessageRequest());
```

</div>
</div>

<div class="memitem" id="response-constructor" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">sipmessage(response)</h4>
</div>
<div class="memproto" markdown="1">

```cpp
siddiqsoft::sipmessage::sipmessage(
    uint32_t statusCode
);
```

</div>
<div class="memdoc" markdown="1">

Constructs a SIP response message with numeric status code and standard reason phrase.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>uint32_t</code></td>
    <td class="paramname">statusCode</td>
    <td class="paramdesc">Standard SIP status code (e.g. `200`, `404`, `503`).</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>sipmessage</code> &mdash; A populated SIP response message instance.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L99-L100
siddiqsoft::sipmessage response(200);

EXPECT_EQ(200, response.getStatusCode());
EXPECT_EQ("OK", response.getReason());
EXPECT_TRUE(response.isMessageResponse());
```

</div>
</div>

<div class="memitem" id="json-constructors" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">sipmessage(json)</h4>
</div>
<div class="memproto" markdown="1">

```cpp
siddiqsoft::sipmessage::sipmessage(
    const nlohmann::json& src
);
```

</div>
<div class="memdoc" markdown="1">

Initializes `sipmessage` from an existing JSON document conforming to sip2json schema.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>const nlohmann::json&amp;</code></td>
    <td class="paramname">src</td>
    <td class="paramdesc">Source JSON object conforming to sip2json schema.</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>sipmessage</code> &mdash; A deserialized message instance wrapping the JSON payload.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L60-L70
nlohmann::json json_obj = {
    {"s", {{"type", "request"}, {"method", "INVITE"}, {"uri", "sip:test@example.com"}, {"version", "SIP/2.0"}}},
    {"h", {{"Call-ID", "test-call-id"}, {"User-Agent", "test-agent"}}},
    {"b", nullptr},
    {"meta", {{"version", "sip2json/3.2.0/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
};

siddiqsoft::sipmessage msg(json_obj);
EXPECT_EQ(siddiqsoft::METHOD_INVITE, msg.getMethod());
EXPECT_EQ("test-call-id", msg.getCallID());
```

</div>
</div>

### Start-Line Accessors

<div class="memitem" id="getmethod" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getMethod()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto siddiqsoft::sipmessage::getMethod() const;
```

</div>
<div class="memdoc" markdown="1">

Returns a copy of the SIP request method string.

<div class="memdoc-section-title">Returns</div>

<code>auto</code> (<code>std::string</code>) &mdash; Method string (e.g., `"INVITE"`, `"REGISTER"`). Empty string if response.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L48
siddiqsoft::sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:test@example.com");
EXPECT_EQ(siddiqsoft::METHOD_INVITE, msg.getMethod());
```

</div>
</div>

<div class="memitem" id="getmethodview" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getMethodView()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
std::string_view siddiqsoft::sipmessage::getMethodView() const;
```

</div>
<div class="memdoc" markdown="1">

Returns zero-copy view of request method string from internal JSON storage.

<div class="memdoc-section-title">Returns</div>

<code>std::string_view</code> &mdash; Non-allocating view into internal JSON. Lifetime tied to parent `sipmessage`.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L97
std::string_view methodView = sipm.getMethodView();
EXPECT_EQ("INVITE", methodView);
```

</div>
</div>

<div class="memitem" id="geturi" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getUri()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto siddiqsoft::sipmessage::getUri() const;
```

</div>
<div class="memdoc" markdown="1">

Returns a copy of the SIP request URI string.

<div class="memdoc-section-title">Returns</div>

<code>auto</code> (<code>std::string</code>) &mdash; URI string (e.g. `"sip:user@host.com"`). Empty if missing.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L175
std::string uri = sipm.getUri();
EXPECT_EQ("sip:test@test.com", uri);
```

</div>
</div>

<div class="memitem" id="geturiview" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getUriView()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
std::string_view siddiqsoft::sipmessage::getUriView() const;
```

</div>
<div class="memdoc" markdown="1">

Returns zero-copy view of request URI string from internal JSON storage.

<div class="memdoc-section-title">Returns</div>

<code>std::string_view</code> &mdash; Non-allocating view of the request URI string.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L175
std::string_view uriView = sipm.getUriView();
EXPECT_EQ("sip:test@test.com", uriView);
```

</div>
</div>

<div class="memitem" id="getstatuscode" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getStatusCode()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto siddiqsoft::sipmessage::getStatusCode() const;
```

</div>
<div class="memdoc" markdown="1">

Returns numeric response status code.

<div class="memdoc-section-title">Returns</div>

<code>auto</code> (<code>uint32_t</code>) &mdash; Status code (e.g. `200`, `404`). Returns `0` if message is a request.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L115
uint32_t code = sipm.getStatusCode();
EXPECT_EQ(200u, code);
```

</div>
</div>

<div class="memitem" id="getreason" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getReason()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto siddiqsoft::sipmessage::getReason() const;
```

</div>
<div class="memdoc" markdown="1">

Returns response reason phrase string.

<div class="memdoc-section-title">Returns</div>

<code>auto</code> (<code>std::string</code>) &mdash; Reason phrase string (e.g. `"OK"`, `"Not Found"`).

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L74
siddiqsoft::sipmessage resp(200);
EXPECT_EQ("OK", resp.getReason());
```

</div>
</div>

<div class="memitem" id="getreasonview" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getReasonView()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
std::string_view siddiqsoft::sipmessage::getReasonView() const;
```

</div>
<div class="memdoc" markdown="1">

Returns zero-copy view of response reason phrase.

<div class="memdoc-section-title">Returns</div>

<code>std::string_view</code> &mdash; Non-allocating view of response reason phrase.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L74
std::string_view reason = resp.getReasonView();
EXPECT_EQ("OK", reason);
```

</div>
</div>

<div class="memitem" id="ismessagerequest" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">isMessageRequest()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
bool siddiqsoft::sipmessage::isMessageRequest() const;
```

</div>
<div class="memdoc" markdown="1">

Returns true if message represents a SIP request.

<div class="memdoc-section-title">Returns</div>

<code>bool</code> &mdash; `true` if message represents a request; `false` otherwise.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L85
EXPECT_TRUE(moved.isMessageRequest());
```

</div>
</div>

<div class="memitem" id="ismessageresponse" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">isMessageResponse()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
bool siddiqsoft::sipmessage::isMessageResponse() const;
```

</div>
<div class="memdoc" markdown="1">

Returns true if message represents a SIP response.

<div class="memdoc-section-title">Returns</div>

<code>bool</code> &mdash; `true` if message represents a response; `false` otherwise.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L100
EXPECT_TRUE(response.isMessageResponse());
```

</div>
</div>

### Header Operations

<div class="memitem" id="headers" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">headers()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto& siddiqsoft::sipmessage::headers();
```

</div>
<div class="memdoc" markdown="1">

Provides direct reference to the `/h` headers JSON object.

<div class="memdoc-section-title">Returns</div>

<code>auto&amp;</code> (<code>nlohmann::json&amp;</code>) &mdash; Mutable reference to the internal headers dictionary.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/synthetics.cpp:L84-L89
EXPECT_TRUE(sipm.contains("/h/Via"_json_pointer));
auto via = sipm["h"]["Via"];
EXPECT_TRUE(via.is_array());
```

</div>
</div>

<div class="memitem" id="getheader" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getHeader()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
template <class T> auto siddiqsoft::sipmessage::getHeader(
    const std::string& key,
    std::optional<T> defaultValue = {}
) const;
```

</div>
<div class="memdoc" markdown="1">

Retrieves header value matching key, converting to type `T`.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>const std::string&amp;</code></td>
    <td class="paramname">key</td>
    <td class="paramdesc">Header name (case-insensitive via canonical mapping).</td>
  </tr>
  <tr>
    <td class="paramtype"><code>std::optional&lt;T&gt;</code></td>
    <td class="paramname">defaultValue</td>
    <td class="paramdesc">Optional fallback value returned if key is absent.</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>auto</code> (<code>T</code>) &mdash; Header value converted to type `T`. Returns `defaultValue` if header is not present.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L47
std::string customVal = copy.getHeader<std::string>("X-Custom");
EXPECT_EQ("original-value", customVal);
```

</div>
</div>

<div class="memitem" id="setheader" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">setHeader()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
template <typename T> sipmessage& siddiqsoft::sipmessage::setHeader(
    const std::string& key,
    const T& v
);
```

</div>
<div class="memdoc" markdown="1">

Sets or updates header key-value pair. Returns `*this` for method chaining.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>const std::string&amp;</code></td>
    <td class="paramname">key</td>
    <td class="paramdesc">Header name string or static constant (e.g. `siddiqsoft::HF_FROM`).</td>
  </tr>
  <tr>
    <td class="paramtype"><code>const T&amp;</code></td>
    <td class="paramname">v</td>
    <td class="paramdesc">Header value (string, integer, or convertible type).</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>sipmessage&amp;</code> &mdash; Reference to `*this` enabling method chaining.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L137-L138
sipm.setHeader(siddiqsoft::HF_TO, "sip:test@test.com")
    .setHeader(siddiqsoft::HF_FROM, "sip:sender@sender.com");
```

</div>
</div>

<div class="memitem" id="getcallid" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getCallID()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto siddiqsoft::sipmessage::getCallID() const;
```

</div>
<div class="memdoc" markdown="1">

Returns the Call-ID header value.

<div class="memdoc-section-title">Returns</div>

<code>auto</code> (<code>std::string</code>) &mdash; Call-ID header string.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L46
EXPECT_EQ(original.getCallID(), copy.getCallID());
```

</div>
</div>

<div class="memitem" id="getcallidview" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getCallIDView()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
std::string_view siddiqsoft::sipmessage::getCallIDView() const;
```

</div>
<div class="memdoc" markdown="1">

Returns zero-copy view of Call-ID header.

<div class="memdoc-section-title">Returns</div>

<code>std::string_view</code> &mdash; Non-allocating view into internal JSON for the Call-ID header.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L98
ASSERT_EQ(callId, sipm.getCallIDView());
```

</div>
</div>

<div class="memitem" id="getcontenttype" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getContentType()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
std::string siddiqsoft::sipmessage::getContentType() const;
```

</div>
<div class="memdoc" markdown="1">

Returns Content-Type header string.

<div class="memdoc-section-title">Returns</div>

<code>std::string</code> &mdash; Content-Type string (e.g. `"application/sdp"`). Empty if absent.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L50
sipm.setHeader(siddiqsoft::HF_CONTENT_TYPE, "application/sdp");
EXPECT_EQ("application/sdp", sipm.getContentType());
```

</div>
</div>

<div class="memitem" id="getcontenttypeview" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getContentTypeView()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
std::string_view siddiqsoft::sipmessage::getContentTypeView() const;
```

</div>
<div class="memdoc" markdown="1">

Returns zero-copy view of Content-Type header.

<div class="memdoc-section-title">Returns</div>

<code>std::string_view</code> &mdash; Non-allocating view of Content-Type header.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L50
std::string_view ct = sipm.getContentTypeView();
EXPECT_EQ("application/sdp", ct);
```

</div>
</div>

<div class="memitem" id="getcontentlength" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getContentLength()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
uint32_t siddiqsoft::sipmessage::getContentLength() const;
```

</div>
<div class="memdoc" markdown="1">

Parses and returns Content-Length as integer.

<div class="memdoc-section-title">Returns</div>

<code>uint32_t</code> &mdash; Content length in bytes. Defaults to `0` if absent.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L51
EXPECT_EQ(0u, sipm.getContentLength());
```

</div>
</div>

<div class="memitem" id="getexpires" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getExpires()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
uint32_t siddiqsoft::sipmessage::getExpires() const;
```

</div>
<div class="memdoc" markdown="1">

Parses and returns Expires header as integer.

<div class="memdoc-section-title">Returns</div>

<code>uint32_t</code> &mdash; Expiration period in seconds.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/test.cpp
sipm.setHeader(siddiqsoft::HF_EXPIRES, 3600);
EXPECT_EQ(3600u, sipm.getExpires());
```

</div>
</div>

### Body & SDP Operations

<div class="memitem" id="body" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">body()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
auto& siddiqsoft::sipmessage::body();
```

</div>
<div class="memdoc" markdown="1">

Provides direct reference to the `/b` body JSON object.

<div class="memdoc-section-title">Returns</div>

<code>auto&amp;</code> (<code>nlohmann::json&amp;</code>) &mdash; Mutable reference to body container.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L208
ASSERT_TRUE(sipm.contains("/b/sdp"_json_pointer));
auto& b = sipm.body();
EXPECT_TRUE(b.contains("sdp"));
```

</div>
</div>

<div class="memitem" id="hasbody" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">hasBody()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
bool siddiqsoft::sipmessage::hasBody() const;
```

</div>
<div class="memdoc" markdown="1">

Returns true if message body contains data.

<div class="memdoc-section-title">Returns</div>

<code>bool</code> &mdash; `true` if `/b` exists and is non-empty; `false` otherwise.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/stress_tests.cpp:L208
EXPECT_TRUE(sipm.hasBody());
```

</div>
</div>

<div class="memitem" id="getbodyelement" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">getBodyElement()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
template <typename T> T siddiqsoft::sipmessage::getBodyElement(
    const nlohmann::json::json_pointer& jp,
    const T& defaultValue
) const;
```

</div>
<div class="memdoc" markdown="1">

Queries property within body JSON tree via RFC 6901 JSON pointer.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>const nlohmann::json::json_pointer&amp;</code></td>
    <td class="paramname">jp</td>
    <td class="paramdesc">JSON Pointer path (e.g. `"/sdp/0/c/dn"_json_pointer`).</td>
  </tr>
  <tr>
    <td class="paramtype"><code>const T&amp;</code></td>
    <td class="paramname">defaultValue</td>
    <td class="paramdesc">Fallback value returned if pointer is unresolved.</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>T</code> &mdash; Extracted value converted to type `T`, or `defaultValue` if path does not exist.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/synthetics.cpp:L84
auto elem = sipm.getBodyElement<std::string>("/sdp/0/c/dn"_json_pointer, "0.0.0.0");
EXPECT_EQ("10.0.0.1", elem);
```

</div>
</div>

<div class="memitem" id="setbody" markdown="1">
<div class="memitem-header">
  <span class="memitem-diamond">&#9670;</span>
  <h4 class="memitem-title">setBody()</h4>
</div>
<div class="memproto" markdown="1">

```cpp
template <typename T> sipmessage& siddiqsoft::sipmessage::setBody(
    const nlohmann::json::json_pointer& jp,
    const T& v
);
```

</div>
<div class="memdoc" markdown="1">

Sets value at specified JSON pointer path within the body object.

<div class="memdoc-section-title">Parameters</div>

<table class="params" markdown="0">
  <tr>
    <td class="paramtype"><code>const nlohmann::json::json_pointer&amp;</code></td>
    <td class="paramname">jp</td>
    <td class="paramdesc">JSON Pointer path in body.</td>
  </tr>
  <tr>
    <td class="paramtype"><code>const T&amp;</code></td>
    <td class="paramname">v</td>
    <td class="paramdesc">Value to assign.</td>
  </tr>
</table>

<div class="memdoc-section-title">Returns</div>

<code>sipmessage&amp;</code> &mdash; Reference to `*this` enabling method chaining.

<div class="memdoc-section-title">Example</div>

```cpp
// Source: tests/regression/src/synthetics.cpp:L95
sipm.setBody("/sdp/0/c/dn"_json_pointer, "10.0.0.1");
EXPECT_EQ("10.0.0.1", sipm.getBodyElement<std::string>("/sdp/0/c/dn"_json_pointer, ""));
```

</div>
</div>
