# Errors & Exceptions

`sip2json` uses exception-based reporting for synchronous calls and error code callbacks for stream parsing.

---

## `sip2json_exception` Class

Derived from `std::exception`, base exception type thrown when standard parsing or serialization operations encounter invalid data.

### Derived Exception Types

* `invalid_startline_error`: Thrown when the start line is malformed or uses an unsupported/custom method token.
* `invalid_document_error`: Thrown during serialization when a document is missing required fields, uses an unsupported method, or contains invalid CRLF characters.
* `incomplete_buffer_for_parse_error`: Thrown when buffer contains a partial message framing.
* `unsupported_contenttype_error`: Thrown when payload content-type is unsupported.
* `empty_message_error`: Thrown when attempting to serialize an empty message.

```cpp
try {
    auto msg = sip2json::parseFromBuffer(startIt, endIt);
} catch (const siddiqsoft::invalid_startline_error& ex) {
    std::cerr << "Invalid SIP Startline: " << ex.what() << std::endl;
} catch (const siddiqsoft::sip2json_exception& ex) {
    std::cerr << "SIP Parse Exception: " << ex.what() << std::endl;
}
```

---

## `sip2jsonErrors` Enum

Enum class defining specific diagnostic error codes returned to stream callbacks:

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
