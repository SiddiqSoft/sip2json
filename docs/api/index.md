# API Reference Overview

<div class="api-header-block">
  <div class="api-module-name">sip2json C++20 Reference</div>
  <div class="api-header-file">Generated from intermediate Doxygen XML</div>
</div>

The `siddiqsoft` namespace provides data structures, stream parsing utilities, diagnostic error definitions, and serialization functions for SIP and SDP protocols.

## Namespaces

<table class="api-summary-table">
  <tr>
    <td class="memitemleft"><a href="#namespace-siddiqsoft"><strong>siddiqsoft</strong></a><div class="mdesc">Core namespace containing the parser, message DTO, and protocol constants.</div></td>
  </tr>
</table>

## Classes & Structures

<table class="api-summary-table">
  <tr>
    <td class="memtype"><code>class</code></td>
    <td class="memitemleft"><a href="sip2json.md"><strong>siddiqsoft::sip2json</strong></a><div class="mdesc">Static utility class for streaming, parsing, and serializing SIP and SDP messages.</div></td>
  </tr>
  <tr>
    <td class="memtype"><code>class</code></td>
    <td class="memitemleft"><a href="sipmessage.md"><strong>siddiqsoft::sipmessage</strong></a><div class="mdesc">Primary message DTO inheriting <code>nlohmann::json</code>. Represents request and response packets.</div></td>
  </tr>
  <tr>
    <td class="memtype"><code>class</code></td>
    <td class="memitemleft"><a href="errors.md#exception-class-hierarchy"><strong>siddiqsoft::sip2json_exception</strong></a><div class="mdesc">Base exception class for parse syntax, framing, and serialization failures.</div></td>
  </tr>
  <tr>
    <td class="memtype"><code>class</code></td>
    <td class="memitemleft"><a href="constants.md#header-key-sets--compact-aliases-hfs_"><strong>siddiqsoft::HeaderKeySet</strong></a><div class="mdesc">Canonical header metadata and compact alias mapping descriptor.</div></td>
  </tr>
</table>

## Enumerations & Constants

<table class="api-summary-table">
  <tr>
    <td class="memtype"><code>constants</code></td>
    <td class="memitemleft"><a href="constants.md"><strong>Protocol &amp; Header Constants</strong></a><div class="mdesc">Complete dictionary of methods, JSON section keys, delimiters, and canonical headers.</div></td>
  </tr>
  <tr>
    <td class="memtype"><code>enum class</code></td>
    <td class="memitemleft"><a href="errors.md#sip2jsonerrors-enumeration"><strong>siddiqsoft::sip2jsonErrors</strong></a><div class="mdesc">Diagnostic error classification passed to <code>parseAsync</code> error callbacks.</div></td>
  </tr>
  <tr>
    <td class="memtype"><code>enum class</code></td>
    <td class="memitemleft"><a href="sipmessage.md"><strong>siddiqsoft::SIPMessageType</strong></a><div class="mdesc">Discriminator distinguishing request (1) from response (2) messages.</div></td>
  </tr>
</table>

## Header Files

| Header File | Include Path | Description |
| :--- | :--- | :--- |
| **`sip2json.hpp`** | `#include <siddiqsoft/sip2json.hpp>` | Primary parser and serializer engine entry point. |
| **`sipmessage.hpp`** | `#include <siddiqsoft/sipmessage.hpp>` | Complete `sipmessage` container and field accessors. |
| **`sip2json_constants.hpp`** | `#include <siddiqsoft/private/sip2json_constants.hpp>` | Method names, URI schemes, and delimiters. [View Constants](constants.md) |
| **`sip2json_header_keys.hpp`** | `#include <siddiqsoft/private/sip2json_header_keys.hpp>` | Hash table dispatch and canonical header representations. [View Constants](constants.md) |
| **`sip2json_exception.hpp`** | `#include <siddiqsoft/private/sip2json_exception.hpp>` | `sip2jsonErrors` enumeration and derived exception types. |
| **`sip2json_sdp.hpp`** | `#include <siddiqsoft/private/sip2json_sdp.hpp>` | SDP line parser and serialization engine. |

## System UML Class Diagram

The following UML class diagram illustrates the primary classes, relationships, and exception hierarchy in `sip2json`. Each node links directly to its source header file on GitHub:

<!-- @@uml-diag:complete -->

<!-- @@uml-diag:source-table -->

## Detailed Topics

<div class="grid" markdown="1">
<div class="card" markdown="1">

### [Constants Reference](constants.md)

Full documentation of methods (`METHOD_INVITE`), JSON keys, delimiters, and canonical header names.

[View Constants Reference :octicons-arrow-right-24:](constants.md)

</div>
<div class="card" markdown="1">

### [JSON Schema & SDP](json_schema.md)

JSON document structure, field types, header conversions, and SDP attribute mapping.

[View Schema Reference :octicons-arrow-right-24:](json_schema.md)

</div>
<div class="card" markdown="1">

### [Code Examples](examples/index.md)

Practical integration snippets: asynchronous stream parsing, message creation, and error handling.

[View Code Examples :octicons-arrow-right-24:](examples/index.md)

</div>
</div>
