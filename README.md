# sip2json
SIP Parser for Modern C++

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

## Usage

The library is provided as a nuget package but can also be used as a header-only dependency.

## References

[JSON for Modern C++](https://nlohmann.github.io/json/)
