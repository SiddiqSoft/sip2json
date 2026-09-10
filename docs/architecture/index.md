# Architecture & Design

`sip2json` is engineered around zero-copy `std::string_view` parsing, stateless execution, 64-bit constexpr FNV-1a hash matching, and modern C++20 type safety.

## Architectural Principles

1. **Stateless Operations**: No connection state or dialog state machine logic.
2. **First-Class JSON**: Serializes to and from `nlohmann::json` objects without transformation layers.
3. **Iterator Stream Parsing**: Processes non-owning string buffers directly, supporting non-blocking stream drains.
4. **Header-Only**: Single `#include <siddiqsoft/sip2json.hpp>`, zero compiled dependencies, zero regex dependencies.

## Subsystem Topology

The system is organized into distinct architectural layers with strict separation of concerns:

### 1. Public API Layer
Provides high-level developer interfaces and message domain models:

- **`siddiqsoft/sip2json.hpp`**: Primary facade providing static entry points:
    - `parse(std::string_view)`: Decodes single datagram frames.
    - `parseAsync(std::string_view&, onMsg, onErr)`: Continuous multi-frame stream scanner with callback dispatch and buffer progression.
    - `parseFromBuffer(std::string_view)`: Fast-path standalone buffer consumption.
    - `serialize(const sipmessage&)`: RFC 3261 compliance wire serializer.
- **`siddiqsoft/sipmessage.hpp`**: Primary message container extending `nlohmann::json`:
    - `/s`: Start line (`type`, `method`, `uri`, `version`, `statusCode`, `reason`).
    - `/h`: Canonical header map with compact alias lookup (`Call-ID`, `From`, `To`, `Via`, `CSeq`, etc.).
    - `/b`: Message payload and parsed SDP structure.
    - `/meta`: Provenance metadata (`version`, `time`, `ttx`).

### 2. Parser & Serializer Engines (`private/`)
Internal high-throughput token scanners and wire formatters:

- **`sip2json_parser.hpp`**: Linear-time non-backtracking scanner for start line, headers, and body boundary detection.
- **`sip2json_serializer.hpp`**: Canonical serializer formatting headers, parameters, and payloads into CRLF-separated byte streams.
- **`sip2json_sdp.hpp`**: RFC 4566 / RFC 8866 Session Description Protocol parser and JSON serializer.

### 3. Protocol Dictionaries & Token Tables (`private/`)
Precomputed mapping tables and constexpr hashing:

- **`sip2json_header_keys.hpp`**: 64-bit constexpr FNV-1a hash dispatch (`hash_header_key`) with case-folding and compact-to-canonical aliases (`i` -> `Call-ID`, `m` -> `Contact`, `c` -> `Content-Type`, `l` -> `Content-Length`).
- **`sip2json_constants.hpp`**: SIP method literals (`INVITE`, `ACK`, `BYE`, `CANCEL`), version strings, delimiters (`CRLF`, `SP`, `COLON`), and JSON field pointers.
- **`sip2json_response_codes.hpp`**: Numeric status codes (100-699) and RFC 3261 reason phrases.

### 4. Diagnostics & Utilities (`private/`)
Core utilities and diagnostic classifications:

- **`sip2json_exception.hpp`**: Exception type `siddiqsoft::sip2json_exception` (derives `std::runtime_error`) and error classifications `siddiqsoft::sip2jsonErrors`.
- **`sip2json_datetime.hpp`**: High-precision ISO 8601 UTC and RFC 1123 datetime generators for message metadata.
- **`sip2json_utils.hpp`**: Zero-copy string view trimming, pointer escaping, and delimiter scanner helpers.

### 5. External Dependencies
- **`nlohmann::json`**: Header-only JSON library serving as the base class for `sipmessage`.
- **`std::runtime_error`**: Standard C++ exception base for `sip2json_exception`.
- **C++20 Standard Library**: `std::string_view`, `std::functional`, `std::format`, `std::chrono`.

## Component Relationships & Data Flow

- **Class Inheritance**:
    - `siddiqsoft::sipmessage` inherits public `nlohmann::json`.
    - `siddiqsoft::sip2json_exception` inherits public `std::runtime_error`.
- **Inclusion & Orchestration**:
    - `siddiqsoft/sip2json.hpp` includes `sipmessage.hpp`, `sip2json_parser.hpp`, `sip2json_serializer.hpp`, and `sip2json_sdp.hpp`.
    - `siddiqsoft::sip2json` produces and consumes `siddiqsoft::sipmessage`.
- **Engine Delegation & Data Flow**:
    - `sip2json_parser.hpp` delegates payload parsing to `sip2json_sdp.hpp`, uses `sip2json_header_keys.hpp` for canonical hashing, uses `sip2json_utils.hpp` for delimiter scanning, and throws `sip2json_exception` on framing errors.
    - `sip2json_serializer.hpp` delegates SDP serialization to `sip2json_sdp.hpp`, references `sip2json_constants.hpp` for delimiters, and formats wire buffers.
    - `sipmessage.hpp` utilizes `sip2json_header_keys.hpp`, `sip2json_constants.hpp`, `sip2json_response_codes.hpp`, and `sip2json_datetime.hpp` for accessor helpers.


### Subsystem Topology & Component Structure

<!-- @@uml-diag:structure -->

### Stream Parsing & Control Flow

<!-- @@uml-diag:control-flow -->

### Namespace Architecture & Package Boundaries

<!-- @@uml-diagram:namespace -->

## UML Class Diagram

<!-- @@uml-diag:complete -->

### Source Code Mapping

<!-- @@uml-diag:source-table -->

## Component Breakdown & File Mapping

| Component | File Path | Class / Responsibility |
| :--- | :--- | :--- |
| **Parser &amp; Serializer Facade** | [`siddiqsoft/sip2json.hpp`](../api/sip2json.md) | Static entry-point class [`siddiqsoft::sip2json`](../api/sip2json.md) for streaming, buffer consumption, and wire serialization. |
| **Message DTO** | [`siddiqsoft/sipmessage.hpp`](../api/sipmessage.md) | Document model [`siddiqsoft::sipmessage`](../api/sipmessage.md) extending `nlohmann::json` with typed accessors. |
| **Stream Parser Engine** | `siddiqsoft/private/sip2json_parser.hpp` | High-throughput linear token scanner for start line, headers, and payload dispatch. |
| **Wire Serializer Engine** | `siddiqsoft/private/sip2json_serializer.hpp` | RFC 3261 compliance serializer formatting headers and body into CRLF-separated byte streams. |
| **SDP Processor** | [`siddiqsoft/private/sip2json_sdp.hpp`](../api/json_schema.md) | RFC 4566 / RFC 8866 Session Description Protocol parser and JSON serializer. |
| **Diagnostic Errors** | [`siddiqsoft/private/sip2json_exception.hpp`](../api/errors.md) | Exception [`siddiqsoft::sip2json_exception`](../api/errors.md) (derives `std::runtime_error`) and [`sip2jsonErrors`](../api/errors.md). |
| **Header Key Dispatch** | `siddiqsoft/private/sip2json_header_keys.hpp` | Case-insensitive mapping and compact single-character alias expansion. |
| **Protocol Constants** | `siddiqsoft/private/sip2json_constants.hpp` | RFC 3261 standard method identifiers, version literals, and syntax delimiters. |
| **Status Codes** | `siddiqsoft/private/sip2json_response_codes.hpp` | Numeric status code definitions (100-699) and reason phrase lookup tables. |
| **Timestamp Utilities** | `siddiqsoft/private/sip2json_datetime.hpp` | High-precision ISO 8601 and RFC 1123 datetime generators for message metadata. |
| **Buffer Utilities** | `siddiqsoft/private/sip2json_utils.hpp` | Zero-copy string view trimming, token escaping, and delimiter searching helpers. |

## Modular Header Layout

`sip2json` separates public interfaces from private implementation details under `include/siddiqsoft/`:

```
include/siddiqsoft/
|-- sip2json.hpp                       # Public entry-point header
|-- sipmessage.hpp                     # Primary SIP message DTO class
\-- private/                           # Internal implementation headers
    |-- sip2json_exception.hpp         # Error code enums & exception classes
    |-- sip2json_parser.hpp            # Start-line, header & buffer parsing
    |-- sip2json_response_codes.hpp    # SIP status code to reason phrase mapping
    |-- sip2json_sdp.hpp               # SDP body parsing & serialization helpers
    |-- sip2json_serializer.hpp        # SIP message wire-format serialization
    \-- sip2json_utils.hpp             # Utilities, hashing & date formatters
```

## Out-of-Scope Capabilities

To maintain a zero-dependency header-only architecture and maximum runtime efficiency, `sip2json` deliberately excludes:

* **I/O Facilities**: Network sockets, event loops, and buffer management are handled by your host networking engine (ASIO, libuv, epoll, Windows Sockets).
* **Dialog State Machine & CSeq Tracking**: The parser is stateless; call states and dialog transactions are tracked at the application layer.
* **Encryption / TLS**: TLS termination is handled at the transport layer before passing cleartext stream buffers to `sip2json`.

## Architecture & Design Topics

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [Performance & Benchmarks](benchmarks.md)

Multi-platform pipeline build benchmarks, per-message processing latency, and cross-platform comparative metrics.

[View Benchmarks :octicons-arrow-right-24:](benchmarks.md)

</div>

<div class="card" markdown="1">

### [Stream Parsing & Buffer Management](async.md)

Continuous TCP/TLS stream mechanics, iterator buffer progression, move semantics (`sipmessage&&`), and memory lifetime rules.

[Learn Stream Mechanics :octicons-arrow-right-24:](async.md)

</div>

<div class="card" markdown="1">

### [Optimization Choices](optimization_choices.md)

Clean JavaScript-like C++20 API design, FNV-1 header matching, and protocol-specific optimization choices.

[Read Design Choices :octicons-arrow-right-24:](optimization_choices.md)

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
