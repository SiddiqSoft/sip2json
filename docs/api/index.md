# API Reference

The `siddiqsoft::sip2json` namespace provides data structures, stream parsing utilities, error definitions, and serialization functions for SIP message processing in Modern C++23.

---

## Classes & Structs

| Type / Symbol | Kind | Description | Reference |
| :--- | :--- | :--- | :--- |
| [`siddiqsoft::sipmessage`](sipmessage.md) | Class | Primary DTO representing SIP requests and responses. Extends [`nlohmann::json`](json_schema.md). | [`sipmessage.md`](sipmessage.md) |
| [`siddiqsoft::sip2json`](sip2json.md) | Static Class | Parsing & serialization engine: [`parse`](sip2json.md#parse), [`parseFromBuffer`](sip2json.md#parsefrombuffer), [`parseAsync`](sip2json.md#parseasync), and [`serialize`](sip2json.md#serialize). | [`sip2json.md`](sip2json.md) |
| [`siddiqsoft::sip2json_exception`](errors.md#sip2json_exception-class) | Exception Base | Root exception class thrown on parse syntax errors or serialization validation failures. | [`errors.md`](errors.md#sip2json_exception-class) |
| [`siddiqsoft::invalid_startline_error`](errors.md#derived-exception-types) | Exception | Thrown when the startline is malformed or uses an unsupported/custom method token. | [`errors.md`](errors.md#derived-exception-types) |
| [`siddiqsoft::invalid_document_error`](errors.md#derived-exception-types) | Exception | Thrown during serialization when missing required fields or containing CRLF injections. | [`errors.md`](errors.md#derived-exception-types) |
| [`siddiqsoft::incomplete_buffer_for_parse_error`](errors.md#derived-exception-types) | Exception | Thrown when an input buffer contains partial framing. | [`errors.md`](errors.md#derived-exception-types) |
| [`siddiqsoft::unsupported_contenttype_error`](errors.md#derived-exception-types) | Exception | Thrown when message body has an unsupported payload Content-Type. | [`errors.md`](errors.md#derived-exception-types) |
| [`siddiqsoft::empty_message_error`](errors.md#derived-exception-types) | Exception | Thrown when attempting to serialize an empty message object. | [`errors.md`](errors.md#derived-exception-types) |
| [`siddiqsoft::sip2jsonErrors`](errors.md#sip2jsonerrors-enum) | Enum Class | Diagnostic error codes returned to stream callbacks (`Success`, `InvalidStartLine`, etc.). | [`errors.md`](errors.md#sip2jsonerrors-enum) |
| [JSON Metaphor Schema](json_schema.md) | Data Model | Compact single-character key schema (`s`, `h`, `b`, `meta`) mapping SIP frames to JSON. | [`json_schema.md`](json_schema.md) |
| [SDP Support](sdp.md) | Descriptors | Native Session Description Protocol parsing and serialization for structured `/b/sdp` bodies. | [`sdp.md`](sdp.md) |

---

## Member Functions & Methods

| Function / Method | Defined In | Description |
| :--- | :--- | :--- |
| [`sipmessage::sipmessage()`](sipmessage.md#default-constructor) | [`sipmessage`](sipmessage.md) | Default constructor; initializes empty message with metadata. |
| [`sipmessage::sipmessage(method, uri, callId, cseq)`](sipmessage.md#request-constructor) | [`sipmessage`](sipmessage.md) | Constructs request with method, URI, optional Call-ID, and CSeq. |
| [`sipmessage::getHeader<T>(key, default)`](sipmessage.md#getheader) | [`sipmessage`](sipmessage.md) | Retrieves header value by key with optional default fallback. |
| [`sipmessage::setHeader(key, value)`](sipmessage.md#setheader) | [`sipmessage`](sipmessage.md) | Sets or updates header key-value pair (fluent chaining). |
| [`sipmessage::getMethodView()`](sipmessage.md#zero-copy-view-accessors) | [`sipmessage`](sipmessage.md) | Zero-copy `std::string_view` of request method. |
| [`sipmessage::getUriView()`](sipmessage.md#zero-copy-view-accessors) | [`sipmessage`](sipmessage.md) | Zero-copy `std::string_view` of request URI. |
| [`sipmessage::getReasonView()`](sipmessage.md#zero-copy-view-accessors) | [`sipmessage`](sipmessage.md) | Zero-copy `std::string_view` of response reason phrase. |
| [`sipmessage::getCallIDView()`](sipmessage.md#zero-copy-view-accessors) | [`sipmessage`](sipmessage.md) | Zero-copy `std::string_view` of Call-ID. |
| [`sip2json::parse`](sip2json.md#parse) | [`sip2json`](sip2json.md) | Parses all SIP messages from an iterator range into `std::vector<sipmessage>`. |
| [`sip2json::parseFromBuffer`](sip2json.md#parsefrombuffer) | [`sip2json`](sip2json.md) | Extracts single SIP message from iterator buffer range. |
| [`sip2json::parseAsync`](sip2json.md#parseasync) | [`sip2json`](sip2json.md) | High-throughput callback-driven stream parsing with partial frame retention. |
| [`sip2json::serialize`](sip2json.md#serialize) | [`sip2json`](sip2json.md) | Serializes a `sipmessage` back into an RFC 3261 wire format string. |

---

## Code Examples

Practical, ready-to-compile snippets demonstrating primary usage patterns:

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [Asynchronous Stream Parsing](examples/async_parsing.md)

Process continuous TCP/TLS stream buffers with multi-frame batches and partial frame preservation using zero-copy callbacks.

[View Stream Example :octicons-arrow-right-24:](examples/async_parsing.md)

</div>

<div class="card" markdown="1">

### [Message Serialization](examples/serialization.md)

Construct SIP INVITE requests with headers and SDP media descriptions using fluent builder methods, and serialize to RFC 3261 wire format.

[View Serialization Example :octicons-arrow-right-24:](examples/serialization.md)

</div>

<div class="card" markdown="1">

### [Examples Overview](examples/index.md)

Full directory of code examples, common usage patterns, and integration recipes.

[View All Examples :octicons-arrow-right-24:](examples/index.md)

</div>

</div>
