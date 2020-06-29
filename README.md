# sip2json
<b>SIP Parser for Modern C++</b><br/>
<small>Copyright &copy;2020 Abdelkareem Siddiq. All rights reserved.</small>


## Design goals

A SIP parser for Modern C++. Here, we target `C++17` with an eye towards `C++20`.

A lot of parsers exist but they tend to be written (a long time ago) and are primarily in C or wrap around the C library.

JSON is one of the most widely used document storage formats and is the lingua-franca of the web and the various client applications written in JavaScript/TypeScript, C# as well as the myriad of No-SQL data stores.

Modern software architecture relies on many distributed systems and favors horizontal scaling rather than a very efficient, fast single node. 

When solving for resilience and high-availability, using systems such as No-SQL stores from Microsoft, Google or Amazon tends to lean towards JSON as the document store.

JSON libraries feature patching, merging and diff'ing of JSON documents therefore we can offload the transforms to templates which can accomplish this task instead of manipulating bespoke C++ classes.

We skewed towards the following tradeoffs:
- Not the fastest but the most convenient.
- API must be native C++17 without any wrappers or compromises.
- Current data systems require json as the native format.
- Simple, small code
- Let the client worry about transforming the documents


### Features
- Everything is stored in a json document
  - Request Line
  - Headers
  - Content
- Serialize to SIP message
- Deserialize from SIP message stream buffer to json document(s).
- Dependencies
  - The primary datastore is the [json](https://github.com/nlohmann/json) data structure
  - C++17

### Out of scope
This library is intendended to be used as a basis for you application and does not provide:
- IO facility
- Buffer management
- Encryption
- Managing CSeq
- Thread safety is your responsibility
  - The functions do not use shared data, however, any paramter provided must be protected/available for the duration of the call.
- Async/callbacks are not the design goal of this library: no IO is performed and thus no likelyhood of the calls being suspended. Trying to force callbacks here in this simple library would likely create unnecessary overhead.

## Usage

The library is provided as a nuget package but can also be used as a header-only dependency.

## Roadmap

 Release | Notes
---------|---------
v1.0.0   | Basic decoder and encoder for response messages: NOTIFY, ACK, 

## References

### Json Schema

#### Request Sample
```json
{
  "type":"request",
  "rl":"INVITE requestURI SIP/2.0",
  "mh":[  {"Call-ID":null},
          {"Content-Type":"application/sdp"}
  ],
  "mb":{  "sdp":[ {  "v":0,    // Start of session block
                     "o":"",
                     "s":"",
                     "i":"",
                     "t":{"start":0, "stop":0},
                     "a_sl":[] // Session-level a= values.
                     "m":[],   // Media descriptors
                     "a_ml":[] // Media-level a= values.
                  }
          ]
  }
}
```

[JSON for Modern C++](https://nlohmann.github.io/json/)
[SIP Messages Definition](https://tools.ietf.org/html/rfc3261#section-7)
[FMT Library](https://fmt.dev/latest/index.html)
