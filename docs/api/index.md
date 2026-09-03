# API Reference

The `siddiqsoft::sip2json` namespace provides data structures, stream parsing utilities, error definitions, and serialization functions for SIP message processing in Modern C++23.

---

## Core API Components

| Component | Category | Description | Reference |
| :--- | :--- | :--- | :--- |
| **`sipmessage`** | Primary DTO Class | High-level DTO representing SIP requests and responses. Inherits `nlohmann::json` for zero-overhead JSON serialization. | [`sipmessage`](sipmessage.md) |
| **`sip2json`** | Core Functions | Static methods: `parse` (single-message), `parseAsync` (zero-copy iterator stream callback), and `serialize` (wire format). | [`sip2json`](sip2json.md) |
| **`sip2json_exception`** | Exceptions | Custom exception type thrown on parse syntax errors or wire-format validation failures. | [`Errors & Exceptions`](errors.md) |
| **`sip2jsonErrors`** | Error Codes | Strongly-typed enum class defining non-fatal and fatal parser status codes. | [`Errors & Exceptions`](errors.md) |
| **JSON Metaphor & Schema** | Wire Model | Specification for the compact single-character key schema (`s`, `h`, `b`, `meta`) mapping SIP frames to JSON. | [JSON Schema](json_schema.md) |
| **SDP Support** | Media Descriptors | Native Session Description Protocol parsing and serialization into structured `/b/sdp` objects. | [SDP Support](sdp.md) |

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
