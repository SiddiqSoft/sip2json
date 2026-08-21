# Architecture & Design Overview

`sip2json` is engineered around zero-copy design principles, stateless execution, and modern C++23 type safety.

---

## Architectural Principles

1. **Stateless Operations**: Neither `sipmessage` nor `sip2json` maintain internal connection state or state machine logic.
2. **First-Class JSON Representation**: Data models serialize cleanly to/from `nlohmann::json` objects.
3. **Iterator-Based Stream Parsing**: Stream functions process iterators directly, supporting non-blocking stream buffer drains.
4. **Header-Only Implementation**: Zero compiled library artifacts; easily integrated into CMake projects.

---

## Header Layout & Modular Architecture

`sip2json` uses a clean header layout separating public interfaces from private implementation details under `include/siddiqsoft/`:

```
include/siddiqsoft/
├── sip2json.hpp                       # Public entry-point header
├── sipmessage.hpp                     # Primary SIP message DTO class
└── private/                           # Internal implementation headers
    ├── sip2json_exception.hpp         # Error code enums & exception classes
    ├── sip2json_parser.hpp            # Start-line, header & buffer parsing
    ├── sip2json_response_codes.hpp    # SIP status code to reason phrase mapping
    ├── sip2json_sdp.hpp               # SDP body parsing & serialization helpers
    ├── sip2json_serializer.hpp        # SIP message wire-format serialization
    └── sip2json_utils.hpp             # Utilities, CTRE regexes & date formatters
```

* **Single Include Entry**: End-user applications include `#include "siddiqsoft/sip2json.hpp"`.
* **Decoupled Private Implementations**: Core parsing, SDP processing, serialization, and exception handling are split into focused private headers within `private/`.
* **Header-Only Library**: Entirely inline implementation requiring no compiled library binaries.

---

## Section Navigation

- [**Design Patterns**](patterns.md): Factory methods, strategy patterns, and builder chain mechanics.
- [**Data Flow & Memory**](dataflow.md): Stream buffer iteration, move semantics, and zero-allocation parsing paths.
- [**Native vs JSON Study**](native_vs_json.md): Empirical trade-off study comparing standalone native C++ structs against `nlohmann::json` inheritance.
