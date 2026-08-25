# Exceptions & Diagnostic Error Codes

`sip2json` provides structured exception classes for synchronous operations (`parseFromBuffer`, `parse`, `serialize`) and enumeration error codes for stream callbacks (`parseAsync`).

```cpp
#include "siddiqsoft/sip2json.hpp"
```

---

## 1. Exception Hierarchy (`sip2json_exception`)

All parser and serializer exceptions inherit from `siddiqsoft::sip2json_exception`, which inherits from `std::exception`:

```
std::exception
 └── siddiqsoft::sip2json_exception
      ├── invalid_startline_error
      ├── invalid_document_error
      ├── incomplete_buffer_for_parse_error
      ├── unsupported_contenttype_error
      └── empty_message_error
```

---

### Exception Types & Usage Examples

#### `invalid_startline_error`
Thrown when a SIP start-line is malformed, missing the protocol version, or contains an unknown method token.

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void parseMalformedStartline(std::string badData)
    {
        auto start = badData.begin();
        auto end   = badData.end();

        try {
            auto msg = siddiqsoft::sip2json::parseFromBuffer(start, end);
        }
        catch (const siddiqsoft::invalid_startline_error& ex) {
            std::cerr << "Invalid Start-Line: " << ex.what() << std::endl;
        }
    }
    ```

---

#### `invalid_document_error`
Thrown during serialization if a document lacks required fields, uses an unsupported method, or contains CRLF injection characters in header keys/values or URIs.

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void serializeInvalidMessage(siddiqsoft::sipmessage& msg)
    {
        try {
            std::string wire = siddiqsoft::sip2json::serialize(msg);
        }
        catch (const siddiqsoft::invalid_document_error& ex) {
            std::cerr << "Validation Error before Wire Emit: " << ex.what() << std::endl;
        }
    }
    ```

---

#### `incomplete_buffer_for_parse_error`
Thrown when the input buffer contains a partial message (e.g., headers are cut off or fewer bytes than `Content-Length` are present in the buffer).

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void handlePartialRead(std::string partialPacket)
    {
        auto start = partialPacket.begin();
        auto end   = partialPacket.end();

        try {
            auto msg = siddiqsoft::sip2json::parseFromBuffer(start, end);
        }
        catch (const siddiqsoft::incomplete_buffer_for_parse_error& ex) {
            std::cout << "Partial frame received. Awaiting more socket bytes...\n";
        }
    }
    ```

---

#### `empty_message_error`
Thrown when attempting to serialize a `sipmessage` that contains only default metadata and no start-line or headers.

=== "Example"
    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    void testEmptySerialization()
    {
        siddiqsoft::sipmessage emptyMsg;

        try {
            std::string wire = siddiqsoft::sip2json::serialize(emptyMsg);
        }
        catch (const siddiqsoft::empty_message_error& ex) {
            std::cerr << "Cannot serialize uninitialized message: " << ex.what() << std::endl;
        }
    }
    ```

---

## 2. Diagnostic Error Enum (`sip2jsonErrors`)

The `siddiqsoft::sip2jsonErrors` enumeration is used in diagnostic reporting and stream error callbacks:

```cpp
enum class sip2jsonErrors {
    ok = 0,
    incomplete_buffer_for_parse,
    incomplete_buffer_for_content,
    incomplete_buffer_for_header,
    invalid_startline,
    unsupported_contenttype,
    missing_required_element,
    invalid_document,
    invalid_document_unsupported_method,
    invalid_document_unsupported_content,
    empty_message
};
```

=== "Example"
    ```cpp
    #include <iostream>
    #include <format>
    #include "siddiqsoft/sip2json.hpp"

    void logDiagnostic(siddiqsoft::sip2jsonErrors err)
    {
        // sip2jsonErrors supports streaming output and std::format
        std::cout << "Diagnostic Status: " << err << std::endl;
    }
    ```
