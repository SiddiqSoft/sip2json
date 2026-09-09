# Error Codes & Exceptions Reference

<div class="api-header-block">
  <div class="api-module-name">Namespace siddiqsoft</div>
  <div class="api-header-file">#include &lt;siddiqsoft/private/sip2json_exception.hpp&gt;</div>
</div>

Diagnostic errors and exception reporting model for `sip2json`. Error conditions are communicated either via exceptions (`sip2json_exception`) or error callback invocations in asynchronous streaming (`sip2jsonErrors`).

## sip2jsonErrors Enumeration

Defined in namespace `siddiqsoft` with underlying type `uint32_t`:

```cpp
enum class sip2jsonErrors : uint32_t {
    ok = 0,
    /* parse errors */
    incomplete_buffer_for_parse,
    incomplete_buffer_for_content,
    incomplete_buffer_for_header,
    invalid_startline,
    unsupported_contenttype,
    missing_required_element,
    /* serialization errors */
    invalid_document,
    invalid_document_unsupported_method,
    invalid_document_unsupported_content,
    empty_message
};
```

### Error Code Specifications

| Enumerator | Numeric | Category | Description | Recommended Action |
| :--- | :--- | :--- | :--- | :--- |
| `ok` | `0` | Success | Operation completed successfully with no errors. | Proceed with next operation. |
| `incomplete_buffer_for_parse` | `1` | Framing | Buffer does not yet contain a complete SIP frame. | Retain buffer; wait for additional socket read data. |
| `incomplete_buffer_for_content` | `2` | Framing | Body content size is less than specified `Content-Length`. | Await more payload bytes from stream. |
| `incomplete_buffer_for_header` | `3` | Framing | Header section is incomplete (missing terminating `\r\n\r\n`). | Await complete header delimiter. |
| `invalid_startline` | `4` | Syntax | Request line or status line is malformed or unparseable. | Discard malformed frame or log protocol error. |
| `unsupported_contenttype` | `5` | Payload | Payload Content-Type is unrecognized for structured parsing. | Fall back to raw string body extraction. |
| `missing_required_element` | `6` | Schema | Expected required element was not found in message. | Verify incoming packet complies with RFC 3261. |
| `invalid_document` | `7` | Serialization | Provided JSON document violates SIP message structure. | Validate JSON schema before serialization. |
| `invalid_document_unsupported_method` | `8` | Serialization | Specified SIP request method is not supported by RFC 3261. | Use standard SIP method token. |
| `invalid_document_unsupported_content` | `9` | Serialization | Message body format cannot be serialized to wire format. | Provide valid string or SDP structure. |
| `empty_message` | `10` | Serialization | Attempted to serialize an empty `sipmessage` object. | Populate start line and headers prior to serialize. |

## Exception Class Hierarchy

All exceptions derive from `std::exception` via `siddiqsoft::sip2json_exception`:

```text
std::exception
 \-- siddiqsoft::sip2json_exception
      |-- invalid_startline_error
      |-- invalid_document_error
      |-- incomplete_buffer_for_parse_error
      |-- incomplete_buffer_for_header_error
      |-- incomplete_buffer_for_content_error
      |-- unsupported_contenttype_error
      |-- missing_required_element
      \-- empty_message_error
```

### Exception Handling Example

```cpp
// Source: tests/regression/src/test.cpp:L45-L52
#include <iostream>
#include <siddiqsoft/sip2json.hpp>

void parseSafely(std::string_view buffer)
{
    try {
        auto msg = siddiqsoft::sip2json::parseFromBuffer(buffer);
        std::cout << "Parsed " << msg.getMethodView() << "\n";
    } catch (const siddiqsoft::invalid_startline_error& ex) {
        std::cerr << "Malformed start line: " << ex.what() << "\n";
    } catch (const siddiqsoft::incomplete_buffer_for_parse_error& ex) {
        std::cerr << "Need more data: " << ex.what() << "\n";
    } catch (const siddiqsoft::sip2json_exception& ex) {
        std::cerr << "SIP error (code " << static_cast<uint32_t>(ex.errCode) << "): " << ex.what() << "\n";
    }
}
```
