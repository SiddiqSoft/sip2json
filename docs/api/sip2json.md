# `sip2json` Namespace Functions

The `siddiqsoft::sip2json` namespace provides stateless utility functions for parsing and serializing SIP protocol payloads.

---

## Functions

### `parse`

```cpp
static sipmessage parse(const std::string& rawSipMessage);
```

Parses a single full SIP message string into a `sipmessage` instance. Throws `sip2json_exception` on syntax or format errors.

---

### `parseAsync`

```cpp
template <typename InputIterator, typename CallbackFn, typename ErrorFn>
static void parseAsync(InputIterator& start,
                       InputIterator end,
                       CallbackFn&& onMessageParsed,
                       ErrorFn&& onErrorEncountered);
```

Parses multiple SIP messages from an iterator range `[start, end)`.

#### Parameters

* `start`: Input iterator reference pointing to the start of the read buffer. Advanced past parsed frames on success.
* `end`: Input iterator representing the end of the read buffer.
* `onMessageParsed`: Callable with signature `void(sipmessage&& msg)`.
* `onErrorEncountered`: Callable with signature `void(sip2jsonErrors& errCode, const std::string& errMessage)`.

---

### `serialize`

```cpp
static std::string serialize(const sipmessage& msg);
```

Serializes a `sipmessage` object back into a standard RFC-3261 formatted SIP protocol string complete with headers and SDP body.
