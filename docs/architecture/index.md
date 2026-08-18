# Architecture & Design Overview

`sip2json` is engineered around zero-copy design principles, stateless execution, and modern C++23 type safety.

---

## Architectural Principles

1. **Stateless Operations**: Neither `sipmessage` nor `sip2json` maintain internal connection state or state machine logic.
2. **First-Class JSON Representation**: Data models serialize cleanly to/from `nlohmann::json` objects.
3. **Iterator-Based Stream Parsing**: Stream functions process iterators directly, supporting non-blocking stream buffer drains.
4. **Header-Only Implementation**: Zero compiled library artifacts; easily integrated into CMake projects.

---

## Section Navigation

- [**Design Patterns**](patterns.md): Factory methods, strategy patterns, and builder chain mechanics.
- [**Data Flow & Memory**](dataflow.md): Stream buffer iteration, move semantics, and zero-allocation parsing paths.
