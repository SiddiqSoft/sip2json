# Errors & Exceptions

`sip2json` uses exception-based reporting for synchronous calls and error code callbacks for stream parsing.

---

## `sip2json_exception` Class

Derived from `std::exception`, thrown when standard parsing or serialization operations encounter invalid data.

```cpp
try {
    auto msg = sip2json::parse(invalidBuffer);
} catch (const sip2json_exception& ex) {
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
