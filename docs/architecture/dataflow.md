# Data Flow & Memory Layout

Understanding data flow and memory management in `sip2json`.

---

## Stream Buffer Data Flow

```mermaid
sequenceDiagram
    autonumber
    participant Network as Network Engine (IO)
    participant Buffer as std::string Buffer
    participant Parser as sip2json::parseAsync
    participant App as Application Handler

    Network->>Buffer: Append TCP payload bytes
    Buffer->>Parser: Pass start & end iterators
    Parser->>Parser: CTRE Regex Frame Detection
    alt Frame complete
        Parser->>App: Invoke callback with sipmessage&& (move)
        Parser->>Buffer: Advance start iterator past frame
    else Incomplete frame
        Parser->>Buffer: Stop parsing, retain partial frame
    end
    App->>Buffer: Erase processed bytes (begin() to advanced cursor)
```

---

## Memory & Lifetime Rules

1. **Move Semantics (`std::move`)**: Messages passed to parsing callbacks are rvalue references (`sipmessage&&`). If you need to preserve message state beyond callback completion, make an explicit copy.
2. **Buffer Alignment**: `parseAsync` uses string iterators without forcing buffer realignments or memory reallocations.
3. **Container Allocation**: Headers and multi-value header arrays are stored using standard library vectors and maps for predictable memory layout.
