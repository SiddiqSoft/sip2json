# Features Overview

`sip2json` provides a modern, lightweight set of tools for working with Session Initiation Protocol (SIP) messages in C++23.

---

## Core Capabilities

<div class="grid">
  <div class="card">
    <h3>Non-Blocking Stream Parsing</h3>
    <p>Parse multiple SIP frames from continuous TCP byte streams using zero-copy iterator advancement and asynchronous callbacks.</p>
    <a href="async.md">Learn more :octicons-arrow-right-24:</a>
  </div>
  <div class="card">
    <h3>First-Class JSON Metaphor</h3>
    <p>Compact schema mapping SIP start lines, headers, and body attributes directly into <code>nlohmann::json</code> objects.</p>
    <a href="json_schema.md">Learn more :octicons-arrow-right-24:</a>
  </div>
  <div class="card">
    <h3>Native SDP Encoding & Decoding</h3>
    <p>Full Session Description Protocol (<code>application/sdp</code>) parsing and formatting integrated directly into message objects.</p>
    <a href="sdp.md">Learn more :octicons-arrow-right-24:</a>
  </div>
</div>

---

## Out-of-Scope Capabilities

To maintain a zero-dependency header-only architecture and maximum runtime efficiency, `sip2json` deliberately excludes:

* **I/O Facilities**: Network sockets and buffer allocation are managed by your host engine (ASIO, libuv, WinSockets).
* **State Machine & CSeq Tracking**: The `sipmessage` and `sip2json` components are completely stateless.
* **Encryption / TLS**: Handled at transport layer before passing cleartext buffers to `sip2json`.
