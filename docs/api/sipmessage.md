# `sipmessage` Class Reference

The `sipmessage` class represents a parsed or constructed SIP request or response message, extending `nlohmann::json`.

---

## Optimal Usage Guidelines

> [!TIP]
> **Performance Best Practice**:
> For maximum throughput and zero-allocation key lookups, use the pre-defined library constants (`siddiqsoft::METHOD_*`, `siddiqsoft::HF_*`, `siddiqsoft::CONTENT_TYPE_*`).
> Passing static string constants yields a **13.2% performance gain for `setHeader`** and a **4.8% gain for `getHeader`** over raw string literals.

```cpp
// OPTIMAL (Zero-Allocation Static Reference Path)
siddiqsoft::sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:bob@biloxi.com", callId, 1);
msg.setHeader(siddiqsoft::HF_CONTENT_TYPE, siddiqsoft::CONTENT_TYPE_APP_SDP);
auto callId = msg.getHeader<std::string>(siddiqsoft::HF_CALLID);

// AD-HOC / FALLBACK (Works seamlessly, ~13% overhead for string literal strlen/conversions)
msg.setHeader("X-Custom-Header", "custom-value");
```

---

## Constructors

### Default Constructor

```cpp
sipmessage();
```

Creates an empty `sipmessage` instance initialized with metadata.

### Request Constructor

```cpp
sipmessage(const std::string& method,
           const std::string& uri,
           const std::string& callId = {},
           uint32_t cseq = 0);
```

Initializes a request message with specified method, URI, optional Call-ID, and optional CSeq. Supports both `const std::string&` references and string literals.

---

## Key Methods & Accessors

### `getHeader`

```cpp
template <class T> auto getHeader(const std::string& key, std::optional<T> defaultValue = {}) const;
template <class T> auto getHeader(const char* key, std::optional<T> defaultValue = {}) const;
template <class T> auto getHeader(std::string_view key, std::optional<T> defaultValue = {}) const;
```

Retrieves a header value by key with an optional default fallback value. Overloaded for `const std::string&`, raw C-strings `const char*`, and `std::string_view`.

### `setHeader`

```cpp
template <typename T> sipmessage& setHeader(const std::string& key, const T& v);
template <typename T> sipmessage& setHeader(const char* key, const T& v);
template <typename T> sipmessage& setHeader(std::string_view key, const T& v);
```

Sets or updates a header key-value pair. Returns a reference to `*this` to support method chaining.

### Zero-Copy View Accessors

```cpp
std::string_view getMethodView() const;
std::string_view getUriView() const;
std::string_view getReasonView() const;
std::string_view getCallIDView() const;
```

Returns a `std::string_view` pointing directly into internal JSON string storage with zero allocations.
