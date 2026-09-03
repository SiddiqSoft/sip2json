# `sip2json` Class Functions

The `siddiqsoft::sip2json` class provides static utility functions for parsing and serializing SIP protocol payloads.

---

## Method Summary

| Method | Return Type | Description |
| :--- | :--- | :--- |
| [`parse`](#parse) | `std::vector<`[`sipmessage`](sipmessage.md)`>` | Parses all available messages from iterator buffer range. |
| [`parseFromBuffer`](#parsefrombuffer) | [`sipmessage`](sipmessage.md) | Extracts single message from iterator buffer range. |
| [`parseAsync`](#parseasync) | `std::string&` | Asynchronously decodes continuous stream via zero-copy callback. |
| [`serialize`](#serialize) | `std::string` | Serializes [`sipmessage`](sipmessage.md) into RFC 3261 wire format string. |

---

## Functions

### `parse`

```cpp
static std::vector<sipmessage> parse(std::string::iterator& bufferStart,
                                   const std::string::iterator& bufferEnd) noexcept(false);
```

Parses as many SIP messages as possible from the buffer range `[bufferStart, bufferEnd)`. Advances `bufferStart` past successfully parsed frames. Returns a vector of [`sipmessage`](sipmessage.md) objects. Throws `std::invalid_argument` if no messages could be decoded.

---

### `parseFromBuffer`

```cpp
static sipmessage parseFromBuffer(std::string::iterator& bufferStart,
                                  const std::string::iterator& bufferEnd) noexcept(false);
```

Extracts and deserializes the first SIP message from the buffer range `[bufferStart, bufferEnd)`. Advances `bufferStart` past the parsed message. Validates the start line against standard RFC SIP methods (`INVITE`, `ACK`, `OPTIONS`, `BYE`, `CANCEL`, `REGISTER`, `SUBSCRIBE`, `NOTIFY`, `MESSAGE`, `INFO`, `REFER`, `PUBLISH`, `UPDATE`, `PRACK`). Throws a derived [`sip2json_exception`](errors.md#sip2json_exception-class) (such as [`invalid_startline_error`](errors.md#derived-exception-types) for custom/unknown method tokens, [`incomplete_buffer_for_parse_error`](errors.md#derived-exception-types), etc.) on syntax or framing errors.

---

### `parseAsync`

```cpp
static std::string& parseAsync(
        std::string& frameBuffer,
        std::function<void(sipmessage&&)> parseCallback,
        std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>> errorCallback = {}) noexcept;
```

Asynchronously parses multiple SIP messages from `frameBuffer`. Decoded messages are moved to `parseCallback(`[`sipmessage`](sipmessage.md)`&&)`. Automatically erases decoded bytes from the front of `frameBuffer`, retaining partial frames for subsequent read cycles.

#### Parameters

* `frameBuffer`: Reference to input string buffer.
* `parseCallback`: Callable with signature `void(`[`sipmessage`](sipmessage.md)`&& msg)`.
* `errorCallback`: Optional error callback with signature `void(const `[`sip2json_exception`](errors.md#sip2json_exception-class)`& e, std::string::iterator& start, const std::string::iterator& end)`.

---

### `serialize`

```cpp
static std::string serialize(sipmessage& msg) noexcept(false);
```

Serializes a [`sipmessage`](sipmessage.md) object back into a standard RFC 3261 formatted SIP protocol string complete with headers and SDP body.

Validates that request messages use a supported standard SIP method (`INVITE`, `ACK`, `OPTIONS`, `BYE`, `CANCEL`, `REGISTER`, `SUBSCRIBE`, `NOTIFY`, `MESSAGE`, `INFO`, `REFER`, `PUBLISH`, `UPDATE`, `PRACK`). Throws [`invalid_document_error`](errors.md#derived-exception-types) if the method is unsupported (e.g. custom token) or if headers/URI contain CRLF injection characters.
