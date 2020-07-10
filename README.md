# sip2json
<b>SIP Parser for Modern C++</b><br/>
<small>Copyright &copy;2020 Abdelkareem Siddiq. All rights reserved.</small>

[![Build Status](https://dev.azure.com/loopup/sys4/_apis/build/status/siddiqsoftware.sip2json?branchName=feature%2Fbootstrap)](https://dev.azure.com/loopup/sys4/_build/latest?definitionId=118&branchName=feature%2Fbootstrap)

<version>1.0.0</version>


[[TOC]]


## Design goals

A SIP parser for Modern C++. Here, we target `C++17` with an eye towards `C++20`.

A lot of parsers exist but they tend to be written (a long time ago) and are primarily in C or wrap around the C library.

JSON is one of the most widely used document storage formats and is the lingua-franca of the web and the various client applications written in JavaScript/TypeScript, C# as well as the myriad of No-SQL data stores.

Modern software architecture relies on many distributed systems and favors horizontal scaling rather than a very efficient, fast single node. 

When solving for resilience and high-availability, using systems such as No-SQL stores from Microsoft, Google or Amazon tends to lean towards JSON as the document store.

JSON libraries feature patching, merging and diff'ing of JSON documents therefore we can offload the transforms to templates which can accomplish this task instead of manipulating bespoke C++ classes.

We skewed towards the following tradeoffs:
- API must be native C++17 without any wrappers or compromises.
- Simple, small code
- Dependencies should be widely-used and respected
- Transforms must be easy

### Features
- Header only!
- Everything is stored in as json document in a simple [json container](#container)
  - [Start Line](#sip-start-line)
  - [Headers](#sip-headers)
  - [Content](#sip-body)
- Serialize to SIP message
- Deserialize from SIP message stream buffer to json document(s).
- Dependencies
  - The primary datastore is the [json](https://github.com/nlohmann/json) data structure
  - The fmtlib (for `C++17` this is a nuget package whereas with `C++20` this would be native)
  - C++17

### Out of scope
This library is intendended to be used as a basis for you application and does not provide:
- IO facility
- Buffer management
- Encryption
- Managing CSeq
- Thread safety is your responsibility
- No statemachine is provided and the `sipmessage` class as well as the `sip2json` class are stateless.

## Usage

The library is provided as a nuget package but can also be used as a header-only dependency.

```c
#include "siddiqsoftware/sip2json.hpp"

// Assume a method invoked by the IO system on each "frame" read
// from the tcp stream.
void onReadCompleted(std::string& readBuffer)
{
  auto bufferStart= readBuffer.begin();
  
  // Invokes the callback per each decoded sipmessage from the buffer
  // Keep track of the bufferStart as it is advanced to reach the readBuffer.end()
  // as objects are parsed.
  processAllFromBuffer(bufferStart, readBuffer.end(), [](sipmessage& msg){
      // Got a valid sipmessage object..
      if(msg.empty()) {
         // Do something..
      }
  },
  [](sip2jsonErrors& errCode){
      // Invoked for an error during the processing of the buffer.
  };

  // Client is responsible for managing the state of the readBuffer
  // the library uses an iterator to parse the stream and advances
  // as each SIP frame is processed.
  readBuffer.erase(readBuffer.begin(), bufferStart); // what remains can be processed as more data arrives
}
```

## Roadmap

 Release | Notes
---------|---------
v1.0.0   | Basic decoder and decoder with support for CloudEvent envelope.

## Tests

- There is a single C++ Native Unit Test using the Microsoft C++ Framework under vstest.
- Code Coverage is enabled (only if you're using Visual Studio Enterprise).
- Azure pipelines CI reports on the test results and the coverage results.
- There are to date `64` tests covering parsing and serialization.
- Use live SIP data found under the [`test\samples`](test/samples) folder.
- [Clang-Tidy](.clang-tidy) is used as a linter to highlight issues with best-practices and static code analysis.
- Consistent formatting using [Clang Format](.clang-format).


## References

### Json Schema

#### Container

The document contains single character entries: `z`, `s`, `h`, `b`.

```json
{
    "z": 0,
    "s": {},
    "h": {},
    "b": null
}
```

This approach acknowledges the fact that SIP messages are generated at a very high rate and processing size of data matters.
Keeping the data compact and mapping the raw SIP and SDP means less intermediate processing is involved and keeps our container usage to a single model (map).


Field | Type | Description
-----:|:-----|--------------------
**`s`**   | object | Represents the [SIP start line](#sip-start-line).
**`z`**   | number | Number of ticks since 1900.
**`h`**   | object | Contains the [SIP headers](#sip-headers).
`b`   | object | Contains the [SIP body](#sip-body). Optional.<br/>Currently, only the `application/sdp` body encode/decode is supported.<br/>If the `Content-Length` is `0`, despite the value of the `Content-Type` this element is skipped.

#### SIP Start Line

Field | Type | Description
-----:|:-----|--------------------
**`type`**   | string | One of `request` or `response`.
`method`   | string | Present for `request` type. Represents the SIP method.
`uri`   | string | Present when the type is `request`, this represents the SIP URI element of the SIP request line.
`status` | string | Present when the type is `response` and represents the status code of the SIP response line.
`reason` | string | Present when the type is `response` and represents the response phrase of the SIP response line.
**`version`**   | string | Always `SIP/2.0`.

#### SIP Headers

The object contains key-value elements found in the SIP header section.

- SIP headers with boolean value types are stored as JSON boolean.
- Default storage type is string
- The header field `Content-Length` is encoded as JSON number.
- When more than one item with the same header name is found, it is stored in an array.
   ```json
    "h": { "Authorization": "",
           "Via": [ "via-1",
                    "via-2",
                    "via-3"
           ],
           "Content-Length": 0 }
   ```
- For header elements that are "empty", the JSON value `""` is stored against that header key instead of `null`.

#### SIP Body

The object contains key-value elements found in the body section.

Field | Type | Description
------|------|-------------
**`v`** | integer | Contant; Set to `0`. Do not modify! This tag is used to delimit a session descriptor block.
**`o`** | string |
**`s`** | string | This can be set to `null` if the item is empty in the SIP message.
`i` | string | Optional.
`u` | string | Optional.
`e` | string | Optional.
`p` | string | Optional.
`c` | string | Optional.
`b` | string | Optional.
**`t`** | Array | Timing for this block. Array of integer values representing the start and end time of the leg.
`z` | string | Optional.
`k` | string | Optional. Encryption key.
**`m`** | string | Media descriptors
**`a`** | array | Media-level a-line items. **Session-level a-line items are not supported.**

- Elements with boolean value types are stored as JSON boolean.
- Default storage type is string
- When more than one item with the same name is found, it is stored in an array. `"a":{"rtpmap":""}` or `"a":{"rtpmap":["",""]}`.
   ```json
    "a": { "remote": "",
           "rtpmap": [ "",
                       ""],
           "new_change": true }
   ```
- Attribute keys that are `a=new_change` are stored as `"a":{"new_change":true}`



#### Request Document
```json
{
    "z": 900000000,
    "s": {
        "type": "request",
        "method": "INVITE",
        "uri": "sip:hello@world.com",
        "version": "SIP/.20"
    },
    "h": {},
    "b": {
        "sdp": [{
            "v": 0,
            "s": "",
            "c": {
                "type": "",
                "subtype": "",
                "dn": ""
            },
            "i": { "dn": "",
                   "name": "",
                   "type": ""
            },
            "o": {
                "user": "",
                "t1": "",
                "t2": "",
                "type": "",
                "subtype": "",
                "host": ""
            },
            "m": "",
            "t": [0, 0],
            "a": {}
        }]
    }
}
```

#### Response Document
```json
{
    "z": 900000000,
    "s": {
        "type": "response",
        "status": 100,
        "reason": "Trying",
        "version": "SIP/.20"
    },
    "h": {}
}
```

#### Request Sample
The source for the following is this [sample SIP](test/samples/NOTIFY_generic_1.sip).

```json
{
    "b": {
        "sdp": [
            {
                "a": {
                    "access_code": "0000000",
                    "acs_guid": "001010000004",
                    "audio_payload": "PCMU",
                    "cdr_start_time": "1594555399.0",
                    "cli-screening": "00",
                    "clir": "false",
                    "dial_once": "aaaa1-aaa13.aaaa2.com",
                    "dialout": "click_in",
                    "far_end": "10.254.254.33:12196",
                    "flags": "1049122",
                    "fmtp": "101 0-15",
                    "ivr": "dialout",
                    "legCallid": "1000000009@10.100.100.100",
                    "leg_no": "3",
                    "mediastatus": "nomedia",
                    "new_change": true,
                    "privs": "participant",
                    "remote": "10.254.254.38:12224",
                    "rtpmap": [
                        "0 pcmu/8000/1",
                        "101 telephone-event/8000"
                    ],
                    "server": "ukdc1-edm18.ring2.com",
                    "sipphone": "usecallid_80000000000000aa-aa-aaaaaa-00@10.254.254.33;port=5060",
                    "status": "(205) answered hold ",
                    "trunk": "8:chan:0",
                    "useforfrom": "hello@world.com",
                    "user-agent": "LoopUp eDial ACS 9.1.0b8050"
                },
                "c": {
                    "dn": "10.254.254.33",
                    "subtype": "IP4",
                    "type": "IN"
                },
                "i": {
                    "dn": "usecallid-leg-3",
                    "name": "usecallid-leg-3",
                    "type": "CallByPhone"
                },
                "m": "audio 8766 RTP/AVP 0 101",
                "o": {
                    "host": "localhost",
                    "subtype": "IP4",
                    "t1": "1011084562",
                    "t2": "804064065",
                    "type": "IN",
                    "user": "hello@world.com"
                },
                "s": "",
                "t": [
                    3803029099,
                    0
                ],
                "v": 0
            }
        ]
    },
    "h": {
        "CSeq": "9 NOTIFY",
        "Call-ID": "80000000000000aa-aa-aaaaaa-00",
        "Contact": "<sip:localhost:8443;transport=ssl>",
        "Content-Length": 880,
        "Content-Type": "application/sdp",
        "From": "sip:hello@world.com;pool=uk-ed-thames;box=ukdc1-edm18.ring2.com;tag=12345678",
        "To": "\"mmyers\" <sip:hello@world.com>",
        "Via": [
            "SIP/2.0/tcp localhost:8443",
            "SIP/2.0/tcp localhost:8443;branch=hello@world.com__eDial_sep__hello@world.com"
        ],
        "X-Billing-code-required": false,
        "X-Call-Instance-ID": "ODQ0NDMaNaU5MaaaOTa1aa1lZC10aGFtZXMtMDE6MTU5NDA0MDI3Naa2NjQ5Nja=",
        "X-Call-Start-Time": "1594040277.665005",
        "X-Call-URL": true,
        "X-Conf_no": "236398",
        "X-From": "hello@world.com",
        "X-Route-ID": "1",
        "X-Sticky": "1",
        "X-Video-SingleView": "0",
        "X-Video-UsingMCU": false,
        "X-client-address": "10.44.200.95",
        "X-control-master": "hello@world.com",
        "X-dialout": "allowed",
        "X-domain": "DEFAULT",
        "X-last-change": "1594040299",
        "X-leader-required": true,
        "X-legs-on-server": "244",
        "X-no-audio": false,
        "X-no-unmute": false,
        "X-no-video": false,
        "X-notify-im": false,
        "X-recording-enabled": true,
        "X-restrict-notify": false,
        "X-restrict-participants": false,
        "X-rollcall": "disabled",
        "X-rss-id": "",
        "X-slave-site": "localhost",
        "X-start-muted": false,
        "X-subject": "Robin Myers' Meeting Room",
        "X-suppress-system-im": false
    },
    "s": {
        "method": "NOTIFY",
        "type": "request",
        "uri": "sip:hello@world.com",
        "version": "SIP/2.0"
    }
}
```

### External resources
- [JSON for Modern C++](https://nlohmann.github.io/json/)
- [FMT Library](https://fmt.dev/latest/index.html)
- [GoogleTest primer](https://github.com/google/googletest/blob/master/googletest/docs/primer.md)
- [SIP Messages Definition](https://tools.ietf.org/html/rfc3261#section-7)
- [SDP specification](https://en.wikipedia.org/wiki/Session_Description_Protocol)
- [SIP Response Codes](https://en.wikipedia.org/wiki/List_of_SIP_response_codes)