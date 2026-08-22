# Design Patterns

`sip2json` applies clean C++ design patterns to provide a fluent and intuitive API.

---

## 1. Builder Pattern (Method Chaining)

The `sipmessage` class supports method chaining for configuring headers, start lines, and content:

```cpp
sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:alice@example.com", "call-id-100", 1);

msg.setHeader(siddiqsoft::HF_FROM, "sip:bob@example.com")
   .setHeader(siddiqsoft::HF_TO, "sip:alice@example.com")
   .setHeader(siddiqsoft::HF_USER_AGENT, "sip2json/2.0");
```

---

## 2. Strategy / Callback Pattern

Asynchronous stream parsing uses non-blocking callbacks for decoupled event handling:

```cpp
sip2json::parseAsync(
    startIt,
    endIt,
    [](sipmessage&& msg) {
        // Message handler strategy
    },
    [](sip2jsonErrors& err, const std::string& info) {
        // Error handler strategy
    }
);
```

---

## 3. Data Transfer Object (DTO) & Type Conversions

`sipmessage` integrates natively with `nlohmann::json` via C++ standard `to_json` and `from_json` serializer functions, allowing implicit conversions:

```cpp
sipmessage msg = sip2json::parse(rawSipText);

// Direct implicit conversion to nlohmann::json DTO
nlohmann::json j = msg;
```
