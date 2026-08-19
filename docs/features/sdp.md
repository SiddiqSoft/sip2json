# SDP (Session Description Protocol) Support

`sip2json` includes built-in parsing and serialization for `application/sdp` payload bodies.

---

## SDP Attribute Mapping

SDP lines (`v=`, `o=`, `s=`, `c=`, `t=`, `m=`, `a=`) are parsed into structured JSON objects inside `b.sdp`.

### Media Lines (`m=`) and Attributes (`a=`)

```json
{
  "v": 0,
  "o": {
    "user": "Alice",
    "t1": "2890844526",
    "t2": "2890844526",
    "type": "IN",
    "subtype": "IP4",
    "host": "10.0.0.5"
  },
  "s": "SIP Call",
  "c": {
    "type": "IN",
    "subtype": "IP4",
    "dn": "10.0.0.5"
  },
  "t": [0, 0],
  "m": "audio 49170 RTP/AVP 0 101",
  "a": {
    "rtpmap": [
      "0 PCMU/8000",
      "101 telephone-event/8000"
    ],
    "fmtp": "101 0-16",
    "sendrecv": true
  }
}
```

---

## Handling Multi-Line Attributes

* Flag attributes like `a=sendrecv` are stored as boolean `true`.
* Repeated attribute keys like `a=rtpmap:...` are accumulated into array values.
* Key-value attributes like `a=fmtp:101 0-16` are separated into string mappings.
