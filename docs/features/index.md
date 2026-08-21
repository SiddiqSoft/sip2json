# Features Overview

`sip2json` provides a modern, lightweight set of tools for working with Session Initiation Protocol (SIP) messages in C++23.

---

## Core Capabilities

<div class="grid" markdown="1">

<div class="card" markdown="1">

### Non-Blocking Stream Parsing

Parse multiple SIP frames from continuous TCP byte streams using zero-copy iterator advancement and asynchronous callbacks.

[Learn more :octicons-arrow-right-24:](async.md)

</div>

<div class="card" markdown="1">

### Performance & Benchmarks

High-throughput benchmarks (~269k msgs/sec), noisy stream resilience, and single-stream vs multi-thread architectural analysis.

[Learn more :octicons-arrow-right-24:](benchmarks.md)

</div>

<div class="card" markdown="1">

### First-Class JSON Metaphor

Compact schema mapping SIP start lines, headers, and body attributes directly into `nlohmann::json` objects.

[Learn more :octicons-arrow-right-24:](json_schema.md)

</div>

<div class="card" markdown="1">

### Native SDP Encoding & Decoding

Full Session Description Protocol (`application/sdp`) parsing and formatting integrated directly into message objects.

[Learn more :octicons-arrow-right-24:](sdp.md)

</div>

<div class="card" markdown="1">

### Standards Compliance & Certification

Automated 226-test suite validating RFC 3261, RFC 4475 (SIP Torture), RFC 4566/8866 (SDP), RFC 3264, and extension RFCs.

[Learn more :octicons-arrow-right-24:](compliance.md)

</div>

</div>

---

## Out-of-Scope Capabilities

To maintain a zero-dependency header-only architecture and maximum runtime efficiency, `sip2json` deliberately excludes:

* **I/O Facilities**: Network sockets and buffer allocation are managed by your host engine (ASIO, libuv, WinSockets).
* **State Machine & CSeq Tracking**: The `sipmessage` and `sip2json` components are completely stateless.
* **Encryption / TLS**: Handled at transport layer before passing cleartext buffers to `sip2json`.
