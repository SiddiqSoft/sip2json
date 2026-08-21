# SIP & SDP Standards Compliance

`sip2json` is designed and validated against official IETF Session Initiation Protocol (SIP) and Session Description Protocol (SDP) standards.

---

## Automated Compliance & Torture Test Suite

The library includes an automated 226-test suite featuring a dedicated compliance module located in `tests/compliance/`:

| Test Suite File | Standards Covered | Key Verifications |
| :--- | :--- | :--- |
| [`tests/compliance/rfc3261_compliance_tests.cpp`](https://github.com/SiddiqSoft/sip2json/blob/master/tests/compliance/rfc3261_compliance_tests.cpp) | **RFC 3261** | Request line parsing for 14 RFC methods (`INVITE`, `ACK`, `OPTIONS`, `BYE`, `CANCEL`, `REGISTER`, `SUBSCRIBE`, `NOTIFY`, `REFER`, `PUBLISH`, `UPDATE`, `PRACK`, `INFO`, `MESSAGE`), status line classes (1xx-6xx), case-insensitive header key canonicalization, compact header field abbreviations (`v`, `f`, `t`, `i`, `c`, `l`, `m`, `s`, `k`, `e`), and body framing. |
| [`tests/compliance/rfc4475_torture_tests.cpp`](https://github.com/SiddiqSoft/sip2json/blob/master/tests/compliance/rfc4475_torture_tests.cpp) | **RFC 4475** | Official IETF torture test cases: `wshort` (minimal valid request), `clnfrn` (compact headers and mixed casing), `foldhdr` (multiline header folding with LWSP `\r\n\t` and `\r\n `), `unkhdr` (preservation of unknown/extension headers), `multvia` (array formatting for multiple `Via` headers), `badact` (rejection of unsupported method tokens), `badlen` (negative `Content-Length` rejection), and `shortbuf` (incomplete header section buffer handling). |
| [`tests/compliance/sip_certification_suite.cpp`](https://github.com/SiddiqSoft/sip2json/blob/master/tests/compliance/sip_certification_suite.cpp) | **RFC 3261, 3262, 6665, 3515, 3903** | End-to-end certification for SIP Core (§7.1, §7.2, §7.3), PRACK reliability (`RAck`/`RSeq`), Event Notifications (`Event`/`Subscription-State`), Call Transfer (`Refer-To`), and Event Publication (`SIP-ETag`). |
| [`tests/compliance/sdp_compliance_tests.cpp`](https://github.com/SiddiqSoft/sip2json/blob/master/tests/compliance/sdp_compliance_tests.cpp) | **RFC 4566, RFC 8866, RFC 3264** | Complete Session Description Protocol parsing: session-level lines (`v=0`, `o=`, `s=`, `i=`, `u=`, `e=`, `p=`, `c=`, `t=`), Offer/Answer direction attributes (`sendrecv`, `sendonly`, `recvonly`, `inactive`), WebRTC ICE/DTLS attributes (`a=candidate`, `a=ice-ufrag`, `a=fingerprint:sha-256 ...`), multiple SDP sessions (`v=0` demarcation), and UNIX `\n` line endings. |

---

## Detailed Standard Verification Matrix

### 1. SIP Start Line Verification
- **Supported Methods**: `INVITE`, `ACK`, `OPTIONS`, `BYE`, `CANCEL`, `REGISTER`, `SUBSCRIBE`, `NOTIFY`, `REFER`, `PUBLISH`, `UPDATE`, `PRACK`, `INFO`, `MESSAGE`.
- **Method Rejection**: Non-standard or custom method tokens (e.g. `BENCHMARK`, `CUSTOM`) are rejected during startline matching and serialization validation.
- **Protocol Version**: Rejects unsupported protocol versions (`SIP/1.0`, `SIP/3.0`, `HTTP/1.1`), enforcing strict `SIP/2.0` compliance.

### 2. Header Field Processing
- **Case-Insensitive Normalization**: Normalizes incoming header field names (`vIa`, `fRoM`, `cALL-id`, `cONTENT-lENGTH`) to canonical Pascal-Kebab-Case keys (`Via`, `From`, `Call-ID`, `Content-Length`).
- **Compact Form Expansion**: Automatically maps single-letter abbreviations (`v`, `f`, `t`, `i`, `c`, `l`, `m`, `s`, `k`, `e`) to their canonical header field equivalents.
- **Multiline Header Folding**: Unfolds multiline header values continuation lines beginning with LWSP (`\r\n ` and `\r\n\t`) per RFC 3261 §7.3.1.
- **Multiple Headers**: Preserves multiple header occurrences (`Via`, `Record-Route`, `Route`, `Accept`) as JSON arrays.

### 3. SDP Body Processing
- **Format Compliance**: Parses SDP session descriptions into structured `/b/sdp` JSON objects per RFC 4566 and RFC 8866.
- **WebRTC Extensions**: Full support for ICE credentials (`ice-ufrag`, `ice-pwd`), candidate lines, and DTLS fingerprint attributes.
- **Line Ending Flexibility**: Seamlessly parses SDP payloads formatted with internet CRLF (`\r\n`) or UNIX LF (`\n`).
