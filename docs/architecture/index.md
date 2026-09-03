# Architecture & Design

`sip2json` is engineered around zero-copy `std::string_view` parsing, stateless execution, 64-bit FNV-1a hash matching, and modern C++23 type safety.

---

## Principles

1. **Stateless Operations**: No connection state or dialog state machine logic.
2. **First-Class JSON**: Serializes to/from `nlohmann::json` objects without transformation layers.
3. **Iterator Stream Parsing**: Processes string iterators directly, supporting non-blocking stream buffer drains.
4. **Header-Only**: Single `#include "siddiqsoft/sip2json.hpp"`, zero compiled dependencies, zero regex dependencies.

---

## Header Layout & Modular Architecture

`sip2json` separates public interfaces from private implementation details under `include/siddiqsoft/`:

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
    └── sip2json_utils.hpp             # Utilities, hashing & date formatters
```

---

## Out-of-Scope Capabilities

To maintain a zero-dependency header-only architecture and maximum runtime efficiency, `sip2json` deliberately excludes:

* **I/O Facilities**: Network sockets, event loops, and buffer management are handled by your host networking engine (ASIO, libuv, epoll, Windows Sockets).
* **Dialog State Machine & CSeq Tracking**: The parser is stateless; call states and dialog transactions are tracked at the application layer.
* **Encryption / TLS**: TLS termination is handled at the transport layer before passing cleartext stream buffers to `sip2json`.

---

## Architecture & Design Topics

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [Design Patterns & Idioms](patterns.md)

Factory methods, strategy patterns, fluent builder chaining, and exception-safe move semantics.

[Explore Patterns :octicons-arrow-right-24:](patterns.md)

</div>

<div class="card" markdown="1">

### [Data Flow & Memory Layout](dataflow.md)

Stream buffer iteration, rvalue move semantics, zero-copy iterator advancement, and memory lifetime rules.

[View Data Flow :octicons-arrow-right-24:](dataflow.md)

</div>

<div class="card" markdown="1">

### [Async Stream Parsing](async.md)

Mechanics of continuous TCP/TLS stream processing, multi-frame batches, and partial frame residual retention.

[Learn Stream Mechanics :octicons-arrow-right-24:](async.md)

</div>

<div class="card" markdown="1">

### [Optimization Choices](optimization_choices.md)

64-bit FNV-1a hash matching, compile-time jump tables, in-register case folding, and zero-copy string_view parsing.

[View Optimizations :octicons-arrow-right-24:](optimization_choices.md)

</div>

<div class="card" markdown="1">

### [Native Struct vs. JSON Model](native_vs_json.md)

Empirical trade-off study comparing standalone native C++ structs against `nlohmann::json` inheritance.

[Read Architectural Study :octicons-arrow-right-24:](native_vs_json.md)

</div>

<div class="card" markdown="1">

### [Performance & Benchmarks](benchmarks.md)

Multi-platform benchmarks (>64k msgs/sec), per-message latency (<16 µs), and thread scaling analysis.

[View Benchmarks :octicons-arrow-right-24:](benchmarks.md)

</div>

<div class="card" markdown="1">

### [Standards Compliance](compliance.md)

Concurrence with RFC 3261, RFC 4475 (SIP Torture 50 test cases), RFC 4566/8866 (SDP), and WebRTC specifications.

[View Standards :octicons-arrow-right-24:](compliance.md)

</div>

<div class="card" markdown="1">

### [Test Suite & RFC Reference](test_suite_sources.md)

Section-by-section mapping of test runner files, sample fixtures, and direct links to official IETF specifications.

[View Test Sources :octicons-arrow-right-24:](test_suite_sources.md)

</div>

</div>
