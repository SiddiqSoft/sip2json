# API Reference Overview

The `siddiqsoft::sip2json` namespace provides data structures, stream parsing utilities, and serialization functions for SIP processing.

---

## Core Classes & Structures

| Structure / Class | Description | Reference |
| :--- | :--- | :--- |
| `sipmessage` | Primary data model for SIP requests and responses | [`sipmessage`](sipmessage.md) |
| `sip2json` | Namespace containing static `parse`, `parseAsync`, and `serialize` functions | [`sip2json`](sip2json.md) |

---

## Exceptions & Error Codes

| Entity | Description | Reference |
| :--- | :--- | :--- |
| `sip2json_exception` | Custom exception type thrown on parse or validation failure | [`Errors & Exceptions`](errors.md) |
| `sip2jsonErrors` | Enum class defining error codes returned during stream parsing | [`Errors & Exceptions`](errors.md) |
