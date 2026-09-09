# Protocol, Header, & Framing Constants Reference

<div class="api-header-block">
  <div class="api-module-name">Namespace siddiqsoft</div>
  <div class="api-header-file">#include &lt;siddiqsoft/private/sip2json_constants.hpp&gt; &bull; &lt;siddiqsoft/private/sip2json_header_keys.hpp&gt;</div>
</div>

The `siddiqsoft` namespace provides compile-time string constants, enumeration sets, and fast hash normalization dispatch tables for SIP RFC 3261 protocols, JSON serialization schema keys, framing delimiters, and canonical header names.

## Architectural Rationale

To maximize throughput and achieve zero dynamic heap allocations in high-performance message processing:

1. **`static inline const std::string` definitions**: Defined once per compilation unit. When searching or inserting into `nlohmann::json` objects, passing these constants prevents the creation of temporary `std::string` instances that would otherwise occur when passing raw C-string literals (`const char*`).
2. **Compile-Time FNV-1a Hashing**: Wire header strings are mapped to canonical representations (`HF_*`) via a 64-bit constexpr Fowler-Noll-Vo-1a hash (`hash_header_key`) with inline case-folding, avoiding `tolower()` string copies during packet parsing.
3. **RFC 3261 Compact Aliases**: Compact single-character headers (e.g. `c` for `Content-Type`, `i` for `Call-ID`, `m` for `Contact`) are automatically mapped to their canonical forms via `siddiqsoft::canonicalizeHeaderKey`.

## Top-Level Message JSON Section Keys

Partition keys defining the root structure of the `sipmessage` JSON object:

| Constant Name | Type | Value | Target JSON Path | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| `JSON_KEY_STARTLINE` | `std::string` | `"s"` | `/s` | Container for request-line or status-line fields. |
| `JSON_KEY_HEADERS` | `std::string` | `"h"` | `/h` | Key-value dictionary containing all normalized message headers. |
| `JSON_KEY_BODY` | `std::string` | `"b"` | `/b` | Container for raw message body or structured SDP array. |
| `JSON_KEY_META` | `std::string` | `"meta"` | `/meta` | Diagnostic and provenance tracking metadata. |
| `JSON_KEY_SDP` | `std::string` | `"sdp"` | `/b/sdp` | Structured array container for parsed SDP blocks. |

## Start-Line JSON Field Keys

Keys for start-line / status-line fields under the `/s` object:

| Constant Name | Type | Value | Target Path | Description |
| :--- | :--- | :--- | :--- | :--- |
| `JSON_KEY_TYPE` | `std::string` | `"type"` | `/s/type` | Discriminator: `"request"` (1) or `"response"` (2). |
| `JSON_KEY_METHOD` | `std::string` | `"method"` | `/s/method` | SIP request method name (e.g. `INVITE`). |
| `JSON_KEY_URI` | `std::string` | `"uri"` | `/s/uri` | Target SIP request URI string. |
| `JSON_KEY_VERSION` | `std::string` | `"version"` | `/s/version` | SIP protocol version token (always `SIP/2.0`). |
| `JSON_KEY_STATUS` | `std::string` | `"status"` | `/s/status` | Numeric response status code (e.g. `200`, `404`). |
| `JSON_KEY_REASON` | `std::string` | `"reason"` | `/s/reason` | Response reason phrase string (e.g. `"OK"`). |

## Meta Section Field Keys

Diagnostic telemetry keys under the `/meta` object:

| Constant Name | Type | Value | Target Path | Description |
| :--- | :--- | :--- | :--- | :--- |
| `JSON_KEY_ID` | `std::string` | `"id"` | `/meta/id` | Unique UUID string assigned to message instance. |
| `JSON_KEY_TIME` | `std::string` | `"time"` | `/meta/time` | ISO 8601 UTC creation/receipt timestamp string. |
| `JSON_KEY_TTX` | `std::string` | `"ttx"` | `/meta/ttx` | Parse latency counter / tick accumulator. |

## Standard SIP Methods

Standard RFC 3261 / 3262 / 3265 / 3515 / 3903 request method constants:

| Constant Name | Type | Value | RFC Specification | Description |
| :--- | :--- | :--- | :--- | :--- |
| `METHOD_INVITE` | `std::string` | `"INVITE"` | RFC 3261 | Initiates a session or dialog between endpoints. |
| `METHOD_ACK` | `std::string` | `"ACK"` | RFC 3261 | Confirms final response to an INVITE request. |
| `METHOD_OPTIONS` | `std::string` | `"OPTIONS"` | RFC 3261 | Queries capabilities and supported extensions. |
| `METHOD_BYE` | `std::string` | `"BYE"` | RFC 3261 | Terminates an active media session or call. |
| `METHOD_CANCEL` | `std::string` | `"CANCEL"` | RFC 3261 | Cancels a pending uncompleted request. |
| `METHOD_REGISTER` | `std::string` | `"REGISTER"` | RFC 3261 | Binds a SIP address to current network contact. |
| `METHOD_SUBSCRIBE` | `std::string` | `"SUBSCRIBE"` | RFC 3265 | Requests event notifications from remote agent. |
| `METHOD_NOTIFY` | `std::string` | `"NOTIFY"` | RFC 3265 | Transports state notifications to subscriber. |
| `METHOD_MESSAGE` | `std::string` | `"MESSAGE"` | RFC 3428 | Transports instant message text payloads. |
| `METHOD_INFO` | `std::string` | `"INFO"` | RFC 2976 | Sends mid-dialog session control information. |
| `METHOD_REFER` | `std::string` | `"REFER"` | RFC 3515 | Requests recipient to contact a third party. |
| `METHOD_PUBLISH` | `std::string` | `"PUBLISH"` | RFC 3903 | Publishes event state (e.g. presence data). |
| `METHOD_UPDATE` | `std::string` | `"UPDATE"` | RFC 3311 | Modifies session state before call is answered. |
| `METHOD_PRACK` | `std::string` | `"PRACK"` | RFC 3262 | Provisional response acknowledgement. |

### Valid Methods Array

```cpp
static constexpr std::string_view SIP_VALID_METHODS[] = {
    "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER",
    "SUBSCRIBE", "NOTIFY", "REFER", "PUBLISH", "UPDATE", "PRACK",
    "INFO", "MESSAGE"
};
```

## Protocol & Network Constants

| Constant Name | Type | Value | Description |
| :--- | :--- | :--- | :--- |
| `SIPVER_20` | `std::string` | `"SIP/2.0"` | Standard SIP version identifier token. |
| `DEFAULT_SERVER_PORT` | `constexpr int` | `5060` | Default IANA-assigned port for SIP over UDP and TCP. |
| `URI_SCHEME_SIP` | `std::string` | `"sip:"` | Unencrypted SIP URI scheme prefix. |
| `URI_SCHEME_SIPS` | `std::string` | `"sips:"` | Secure TLS-encrypted SIP URI scheme prefix. |
| `VIA_BRANCH_PREFIX` | `std::string` | `"z9hG4bK"` | RFC 3261 magic cookie required for all transaction branch IDs. |
| `EMPTY_STD_STRING_VALUE` | `std::string` | `""` | Cached empty string instance to avoid heap reallocations. |

## Registration & TTL Intervals

| Constant Name | Type | Value | Time Representation |
| :--- | :--- | :--- | :--- |
| `DEFAULT_MAX_REGISTER_TTL` | `constexpr int` | `3600` | 1 hour in seconds. |
| `DEFAULT_MAX_REGISTER_TTL_MS` | `constexpr int` | `3600000` | 1 hour in milliseconds. |
| `DEFAULT_MIN_REGISTER_TTL` | `constexpr int` | `120` | 2 minutes in seconds. |
| `REGISTER_PERIOD_10MIN_SEC` | `constexpr int` | `600` | 10 minutes in seconds. |
| `REGISTER_PERIOD_10MIN_MS` | `constexpr int` | `600000` | 10 minutes in milliseconds. |
| `REGISTER_PERIOD_1MIN_SEC` | `constexpr int` | `60` | 1 minute in seconds. |
| `REGISTER_PERIOD_MIN_SEC` | `constexpr int` | `30` | 30 seconds. |

## Supported Content Types

MIME type string constants used for `Content-Type` header verification and SDP parsing dispatch:

| Constant Name | Value | Handling Behavior |
| :--- | :--- | :--- |
| `CONTENT_TYPE_APP_SDP` | `"application/sdp"` | Automatically parsed into structured `/b/sdp` array. |
| `CONTENT_TYPE_TEXT_PLAIN` | `"text/plain"` | Retained as verbatim UTF-8 string payload in `/b`. |
| `CONTENT_TYPE_TEXT_HTML` | `"text/html"` | Retained as verbatim UTF-8 string payload in `/b`. |
| `CONTENT_TYPE_TEXT_XML` | `"text/xml"` | Retained as verbatim string payload in `/b`. |
| `CONTENT_TYPE_APP_XML` | `"application/xml"` | Retained as verbatim string payload in `/b`. |
| `CONTENT_TYPE_APP_PKCS7MIME` | `"application/pkcs7-mime"` | Opaque binary/encrypted payload string in `/b`. |
| `CONTENT_TYPE_APP_XPRIVATE` | `"application/x-private"` | Proprietary custom data payload in `/b`. |
| `CONTENT_TYPE_TEXT_X_METATEL1_PRESENCE` | `"text/x-metatel1.0-presence"` | Presence information data format payload. |

## Authentication Schemes & Subscription States

| Category | Constant Name | Value | Purpose |
| :--- | :--- | :--- | :--- |
| **Auth** | `AUTHORIZATION_CLEAR` | `"Clear"` | Cleartext password scheme. |
| **Auth** | `AUTHORIZATION_BASIC` | `"Basic"` | RFC 7617 HTTP Basic authentication scheme. |
| **Auth** | `AUTHORIZATION_DIGEST` | `"Digest"` | RFC 2617 / RFC 7616 MD5 / SHA-256 Digest scheme. |
| **Subscription** | `SUBSTATE_ACTIVE` | `"active"` | Subscription accepted and active. |
| **Subscription** | `SUBSTATE_PENDING` | `"pending"` | Subscription request received but awaiting approval. |
| **Subscription** | `SUBSTATE_TERMINATED` | `"terminated"` | Subscription ended or timed out. |

## Wire Framing & Parsing Delimiters

Constants representing standard line delimiters, separators, and SDP markers across platforms:

| Constant Name | Wire Sequence | Escaped String | Description |
| :--- | :--- | :--- | :--- |
| `ELEM_NEWLINE` | CRLF | `"\r\n"` | Standard RFC 3261 header and line delimiter. |
| `ELEM_HEADERSECTIONDELIMITER` | Double CRLF | `"\r\n\r\n"` | Delimiter separating header dictionary from payload body. |
| `ELEM_SPACE` | Space | `" "` | Token separator in request line and status line. |
| `ELEM_SEPARATOR` | Colon | `":"` | Header key-value delimiter. |
| `ELEM_PADDED_SEPARATOR` | Colon + Space | `": "` | Standard formatted header key-value separator. |
| `ELEM_TAG_SEPARATOR` | Open Brace | `"{"` | Tag / parameter delimiter. |
| `ELEM_LWSP` | CRLF + Space | `"\r\n "` | Linear whitespace folding indicator (RFC 822 / 3261). |
| `ELEM_LWSP1` | CRLF + Tab | `"\r\n\t"` | Tab-based linear whitespace folding indicator. |
| `ELEM_SDPBlockStart` | Padded SDP | `" v=0\r\n"` | Wire indicator for start of SDP session description. |
| `ELEM_NEWLINE_LF` | LF | `"\n"` | UNIX newline fallback delimiter. |
| `ELEM_HEADERSECTIONDELIMITER_LF` | Double LF | `"\n\n"` | UNIX header-body separator fallback. |
| `ELEM_LWSP_LF` | LF + Space | `"\n "` | UNIX linear whitespace folding. |
| `ELEM_LWSP1_LF` | LF + Tab | `"\n\t"` | UNIX tab linear whitespace folding. |
| `ELEM_SDPBlockStart_LF` | LF SDP | `"v=0\n"` | UNIX SDP session block start indicator. |

## Canonical Header Key Constants (`HF_*`)

Static `std::string` definitions used as keys in `sipmessage["h"]` and `sipmessage.setHeader()` to ensure zero dynamic allocation during lookups:

| Constant Name | Canonical Header Name | RFC 3261 Role |
| :--- | :--- | :--- |
| `HF_FROM` | `"From"` | Identifies the originator of the request. |
| `HF_TO` | `"To"` | Identifies the desired recipient of the request. |
| `HF_CALLID` | `"Call-ID"` | Uniquely identifies a particular invitation or registration. |
| `HF_CSEQ` | `"CSeq"` | Sequential number identifying transaction ordering. |
| `HF_VIA` | `"Via"` | Records path taken by request and routes responses back. |
| `HF_CONTACT` | `"Contact"` | Direct routing URI for subsequent requests. |
| `HF_CONTENT_TYPE` | `"Content-Type"` | Media type of message payload body. |
| `HF_CONTENT_LENGTH` | `"Content-Length"` | Size in octets of the message payload. |
| `HF_CONTENT_ENCODING` | `"Content-Encoding"` | Compression or encoding scheme applied to body. |
| `HF_EXPIRES` | `"Expires"` | Relative time in seconds after which message expires. |
| `HF_ROUTE` | `"Route"` | Forces routing through specified proxy addresses. |
| `HF_RECORD_ROUTE` | `"Record-Route"` | Injected by proxies to remain on dialog path. |
| `HF_MAX_FORWARDS` | `"Max-Forwards"` | Hop count limit preventing routing loops (default 70). |
| `HF_USER_AGENT` | `"User-Agent"` | Client software name and version information. |
| `HF_SERVER` | `"Server"` | UAS software name and version information. |
| `HF_AUTHORIZATION` | `"Authorization"` | Client authentication credentials. |
| `HF_WWW_AUTHENTICATE` | `"WWW-Authenticate"` | Authentication challenge issued by UAS. |
| `HF_PROXY_AUTHENTICATE`| `"Proxy-Authenticate"`| Challenge issued by an intermediary proxy. |
| `HF_PROXY_AUTHORIZATION`| `"Proxy-Authorization"`| Client credentials for intermediary proxy. |
| `HF_PROXY_REQUIRE` | `"Proxy-Require"` | Sensitive features that proxies must support. |
| `HF_REQUIRE` | `"Require"` | Extensions that UAS must support to process request. |
| `HF_SUPPORTED` | `"Supported"` | Extensions supported by the sender. |
| `HF_UNSUPPORTED` | `"Unsupported"` | Extensions not supported by recipient. |
| `HF_ALLOW` | `"Allow"` | Lists SIP methods supported by the UA. |
| `HF_WARNING` | `"Warning"` | Diagnostic warning message and code. |
| `HF_ACCEPT` | `"Accept"` | Media types acceptable in responses. |
| `HF_ACCEPT_ENCODING` | `"Accept-Encoding"` | Content codings acceptable in responses. |
| `HF_ACCEPT_LANGUAGE` | `"Accept-Language"` | Preferred natural languages in reason phrases. |
| `HF_DATE` | `"Date"` | Date and time message was originated (RFC 1123). |
| `HF_TIMESTAMP` | `"Timestamp"` | Client time when request was initially sent. |
| `HF_PRIORITY` | `"Priority"` | Call urgency (e.g. `urgent`, `normal`, `emergency`). |
| `HF_SUBJECT` | `"Subject"` | Short text summary of call topic. |
| `HF_LOCATION` | `"Location"` | Geographical or network location of sender. |
| `HF_HIDE` | `"Hide"` | Request proxy anonymity / route concealment. |
| `HF_RESPONSE_KEY` | `"Response-Key"` | PGP key used to encrypt subsequent responses. |
| `HF_RETRY_AFTER` | `"Retry-After"` | Time interval client should wait before retrying. |
| `HF_SUBSCRIPTION_STATE`| `"Subscription-State"`| Current state of event notification subscription. |

## Header Key Sets & Compact Aliases (`HFS_*`)

RFC 3261 defines single-character compact aliases to minimize packet size over MTU-constrained networks. The parser normalizes all aliases to their canonical representation:

| Canonical Name | Compact Alias | Constant Instance | Multi-line Support |
| :--- | :--- | :--- | :--- |
| `From` | `f` | `siddiqsoft::HFS_FROM` | No |
| `To` | `t` | `siddiqsoft::HFS_TO` | No |
| `Call-ID` | `i` | `siddiqsoft::HFS_CALLID` | No |
| `Via` | `v` | `siddiqsoft::HFS_VIA` | **Yes (Array)** |
| `Contact` | `m` | `siddiqsoft::HFS_CONTACT` | No |
| `Content-Type` | `c` | `siddiqsoft::HFS_CONTENT_TYPE` | No |
| `Content-Length` | `l` | `siddiqsoft::HFS_CONTENT_LENGTH` | No |
| `Content-Encoding` | `e` | `siddiqsoft::HFS_CONTENT_ENCODING` | No |
| `Subject` | `s` | `siddiqsoft::HFS_SUBJECT` | No |
| `Supported` | `k` | `siddiqsoft::HFS_SUPPORTED` | **Yes (Array)** |
| `Authorization` | `uthorization` | `siddiqsoft::HFS_AUTHORIZATION` | No |

## Header Normalization & FNV-1a Hashing

The parser matches incoming headers in $O(1)$ time by evaluating a `constexpr` 64-bit Fowler-Noll-Vo-1a hash over the string:

```cpp
constexpr uint64_t hash_header_key(const char* s, size_t len) noexcept;
constexpr uint64_t hash_header_key(std::string_view sv) noexcept;
const HeaderKeySet& canonicalizeHeaderKey(std::string_view keyFromPayload);
```

### Live Test Example

```cpp
// Source: tests/regression/src/rule_of_five_tests.cpp:L40-L45
auto callId = siddiqsoft::createCallId();
siddiqsoft::sipmessage original(siddiqsoft::METHOD_INVITE, "sip:test@example.com", callId, 1);

// Setting canonical headers with static string constants (zero allocation):
original.setHeader(siddiqsoft::HF_TO, "sip:test@example.com");
original.setHeader(siddiqsoft::HF_FROM, "sip:sender@example.com");
original.setHeader(siddiqsoft::HF_CONTENT_TYPE, siddiqsoft::CONTENT_TYPE_APP_SDP);

EXPECT_EQ(original.getMethod(), siddiqsoft::METHOD_INVITE);
EXPECT_EQ(original.getHeader<std::string>(siddiqsoft::HF_CONTENT_TYPE), "application/sdp");
```
