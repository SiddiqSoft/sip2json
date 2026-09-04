# Errors & Exceptions

`sip2json` uses exception-based reporting for synchronous calls ([`sip2json::parseFromBuffer`](sip2json.md#parsefrombuffer), [`sip2json::serialize`](sip2json.md#serialize)) and error code callbacks for stream parsing ([`sip2json::parseAsync`](sip2json.md#parseasync)).

---

## `sip2json_exception` Class

Derived from `std::exception`, root base exception type thrown when parsing or serialization operations encounter invalid data.

### Derived Exception Types

* **`invalid_startline_error`**: Thrown by [`sip2json::parseFromBuffer`](sip2json.md#parsefrombuffer) when the start line is malformed or uses an unsupported/custom method token.
* **`invalid_document_error`**: Thrown by [`sip2json::serialize`](sip2json.md#serialize) when a document is missing required fields, uses an unsupported method, or contains CRLF injection characters.
* **`incomplete_buffer_for_parse_error`**: Thrown by [`sip2json::parseFromBuffer`](sip2json.md#parsefrombuffer) when the input buffer contains a partial message framing.
* **`unsupported_contenttype_error`**: Thrown when message body has an unsupported payload Content-Type.
* **`empty_message_error`**: Thrown by [`sip2json::serialize`](sip2json.md#serialize) when attempting to serialize an empty [`sipmessage`](sipmessage.md).

```cpp
try {
    auto msg = siddiqsoft::sip2json::parseFromBuffer(startIt, endIt);
} catch (const siddiqsoft::invalid_startline_error& ex) {
    std::cerr << "Invalid SIP Startline: " << ex.what() << std::endl;
} catch (const siddiqsoft::sip2json_exception& ex) {
    std::cerr << "SIP Parse Exception: " << ex.what() << std::endl;
}
```

---

## `sip2jsonErrors` Enum

Enum class defining specific diagnostic error codes passed to [`sip2json::parseAsync`](sip2json.md#parseasync) error callbacks:

```cpp
enum class sip2jsonErrors {
    Success = 0,
    InvalidStartLine,
    InvalidHeaderFormat,
    InvalidContentLength,
    IncompleteMessage,
    InvalidSdpBody
};
```

---

## Related References

* [`sipmessage` Class Reference](sipmessage.md)
* [`sip2json` Parsing & Serialization Functions](sip2json.md)
* [JSON Schema Specification](json_schema.md)
* [Code Examples & Stream Parsing](examples/index.md)
