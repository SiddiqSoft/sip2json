# `sip2json` Class Functions

The `siddiqsoft::sip2json` class provides static utility functions for parsing and serializing SIP protocol payloads.

---

## Functions

### `parse`

```cpp
static std::vector<sipmessage> parse(std::string::iterator& bufferStart,
                                   const std::string::iterator& bufferEnd) noexcept(false);
```

Parses as many SIP messages as possible from the buffer range `[bufferStart, bufferEnd)`. Advanced `bufferStart` past successfully parsed frames. Returns a vector of `sipmessage` objects. Throws `std::invalid_argument` if no messages could be decoded.

---

### `parseFromBuffer`

```cpp
static sipmessage parseFromBuffer(std::string::iterator& bufferStart,
                                  const std::string::iterator& bufferEnd) noexcept(false);
```

Extracts and deserializes the first SIP message from the buffer range `[bufferStart, bufferEnd)`. Advances `bufferStart` past the parsed message. Validates the start line against standard RFC SIP methods (`INVITE`, `ACK`, `OPTIONS`, `BYE`, `CANCEL`, `REGISTER`, `SUBSCRIBE`, `NOTIFY`, `MESSAGE`, `INFO`, `REFER`, `PUBLISH`, `UPDATE`, `PRACK`). Throws a derived `sip2json_exception` (such as `invalid_startline_error` for custom/unknown method tokens, `incomplete_buffer_for_parse_error`, etc.) on syntax or framing errors.

---

### `parseAsync`

```cpp
static std::string& parseAsync(
        std::string& frameBuffer,
        std::function<void(sipmessage&&)> parseCallback,
        std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>> errorCallback = {}) noexcept;
```

Asynchronously parses multiple SIP messages from `frameBuffer`. Decoded messages are moved to `parseCallback(sipmessage&&)`. Automatically erases decoded bytes from the front of `frameBuffer`, retaining partial frames for subsequent read cycles.

#### Parameters

* `frameBuffer`: Reference to input string buffer.
* `parseCallback`: Callable with signature `void(sipmessage&& msg)`.
* `errorCallback`: Optional error callback with signature `void(const sip2json_exception& e, std::string::iterator& start, const std::string::iterator& end)`.

---

### `serialize`

```cpp
static std::string serialize(sipmessage& msg) noexcept(false);
```

Serializes a `sipmessage` object back into a standard RFC 3261 formatted SIP protocol string complete with headers and SDP body.

Validates that request messages use a supported standard SIP method (`INVITE`, `ACK`, `OPTIONS`, `BYE`, `CANCEL`, `REGISTER`, `SUBSCRIBE`, `NOTIFY`, `MESSAGE`, `INFO`, `REFER`, `PUBLISH`, `UPDATE`, `PRACK`). Throws `invalid_document_error` if the method is unsupported (e.g. custom token) or if headers/URI contain CRLF injection characters.
