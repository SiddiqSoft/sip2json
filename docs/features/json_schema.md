# JSON Metaphor & Schema

To streamline integration with modern NoSQL databases (DocumentDB, Azure Cosmos DB, MongoDB, Elasticsearch), `sip2json` represents SIP messages as structured JSON documents.

---

## Schema Overview

The document uses single-character keys (`s`, `h`, `b`, `meta`) to minimize payload size during network transmission and indexing.

```json
{
  "meta": {
    "version": "sip2json/1.17.0/1.0.0",
    "time": "2026-08-18T15:30:00.000Z",
    "ttx": 0
  },
  "s": {
    "type": "request",
    "method": "INVITE",
    "uri": "sip:user@example.com",
    "version": "SIP/2.0"
  },
  "h": {
    "Call-ID": "c30382-990-12@10.0.0.1",
    "CSeq": "1 INVITE",
    "From": "sip:caller@example.com;tag=991a",
    "To": "sip:user@example.com",
    "Via": [
      "SIP/2.0/TCP 10.0.0.1:5060;branch=z9hG4bK776"
    ],
    "Content-Length": 142
  },
  "b": {
    "sdp": [
      {
        "v": 0,
        "o": {"user": "-", "t1": "2890844526", "t2": "2890844526", "type": "IN", "subtype": "IP4", "host": "10.0.0.1"},
        "s": "-",
        "c": {"type": "IN", "subtype": "IP4", "dn": "10.0.0.1"},
        "t": [0, 0],
        "m": "audio 49170 RTP/AVP 0 8 96",
        "a": {
          "rtpmap": ["0 PCMU/8000", "8 PCMA/8000"]
        }
      }
    ]
  }
}
```

---

## Document Fields

| Field | Type | Description |
| :--- | :--- | :--- |
| **`s`** | `object` | Start line container (Request line or Status line) |
| **`meta`** | `object` | Diagnostics: library version, ISO timestamp, parse time (`ttx` in ms) |
| **`h`** | `object` | Header key-value pairs. Arrays represent multi-instance headers (e.g. `Via`) |
| **`b`** | `object` | Optional body container (e.g. SDP descriptors) |

---

## Header Type Conversions

* Numeric fields like `Content-Length` are converted to JSON integers.
* Boolean extensions (e.g., `X-Call-URL: true`) are converted to JSON booleans.
* Headers occurring multiple times in a SIP message (e.g. `Via`, `Record-Route`) are represented as JSON string arrays.
