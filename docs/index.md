# sip2json: A Focused SIP Parser for Modern C++

<div class="badge-container">
  <img src="https://img.shields.io/badge/version-v{{ version }}-4f46e5.svg" alt="Version {{ version }}"/>
  <a href="https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master"><img src="https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master" alt="Build Status"></a>
  <a href="https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master"><img src="https://img.shields.io/badge/Total%20Tests-321%20Passed%20(100%25)-2ea44f?logo=checkmarx&logoColor=white" alt="Total Tests 321/321 Passed" /></a>
  <a href="features/compliance/"><img src="https://img.shields.io/badge/RFC%204475-50%2F50%20Torture%20(100%25)-success?logo=testinglibrary&logoColor=white" alt="RFC 4475 Torture Tests" /></a>
  <a href="features/compliance/"><img src="https://img.shields.io/badge/RFC%203261-SIP%20Core%20Validated-003366?logo=shield&logoColor=white" alt="RFC 3261 Compliance" /></a>
  <a href="features/sdp/"><img src="https://img.shields.io/badge/SDP%20%26%20WebRTC-RFC%208866%20%7C%20BUNDLE%20%7C%20ICE-1488C6?logo=webrtc&logoColor=white" alt="SDP and WebRTC Suite" /></a>
  <a href="features/compliance/"><img src="https://img.shields.io/badge/SIP%20Extensions-PRACK%20%7C%20NOTIFY%20%7C%20REFER-6f42c1?logo=gitbook&logoColor=white" alt="SIP Extensions Suite" /></a>
  <a href="features/compliance/"><img src="https://img.shields.io/badge/Security-Memory%20%26%20CRLF%20Safe-107C41?logo=securityscorecard&logoColor=white" alt="Security and Memory Safety" /></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus" alt="C++23 Standard" /></a>
  <a href="license.md"><img src="https://img.shields.io/badge/License-BSD--3--Clause-blue" alt="License" /></a>
</div>

**`sip2json`** (`v{{ version }}`) is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

---

## Design Objectives

* **Header-Only Library**: Easy integration without compiled binary dependencies; include and build.
* **JSON as First-Class Metaphor**: Compact and intuitive representation of SIP start lines, headers, and SDP bodies.
* **Modern C++23**: Built for C++23 standards using concepts, string views, move semantics, and CTRE (Compile-Time Regular Expressions).
* **Asynchronous & Streaming Parsing**: High-performance stream iterator parsing with non-blocking callbacks for multi-frame TCP buffers.
* **Full SDP Support**: Native decoding and encoding of Session Description Protocol (`application/sdp`) payloads.
* Use this library to build your application server that suits the purpose 

### Why json?

- 

---

## Quick Example

=== "Asynchronous Stream Parsing"

    ```cpp
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void processIncomingTcpData(std::string& readBuffer)
    {
        auto bufferStart = readBuffer.begin();

        // Parses multiple SIP messages from buffer iterator asynchronously
        sip2json::parseAsync(
            bufferStart,
            readBuffer.end(),
            [](sipmessage&& msg) {
                if (!msg.empty()) {
                    std::cout << "Received " << msg.method << " request for " << msg.uri << "\n";
                }
            },
            [](sip2jsonErrors& errCode, const std::string& errMessage) {
                std::cerr << "Parser warning/error: " << errMessage << "\n";
            }
        );

        // Remove processed SIP frames from buffer
        readBuffer.erase(readBuffer.begin(), bufferStart);
    }
    ```

=== "Message Serialization"

    ```cpp
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main()
    {
        // Construct SIP INVITE request
        sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:user@example.com", "call-8849-xyz", 1);
        
        msg.setHeader(siddiqsoft::HF_FROM, "sip:caller@example.com")
           .setHeader(siddiqsoft::HF_TO, "sip:user@example.com")
           .setHeader(siddiqsoft::HF_USER_AGENT, "sip2json/2.0");

        // Serialize to standard SIP string format
        std::string rawSip = sip2json::serialize(msg);
        std::cout << rawSip << std::endl;

        return 0;
    }
    ```

=== "JSON Metaphor"

    ```cpp
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main()
    {
        // Parse raw SIP string to sipmessage structure
        sipmessage msg = sip2json::parse(rawSipString);

        // Convert sipmessage directly to nlohmann::json representation
        nlohmann::json doc = msg;
        
        std::cout << "JSON document: " << doc.dump(2) << std::endl;

        return 0;
    }
    ```

---

## Requirements

| Requirement | Details |
| :--- | :--- |
| **Language Standard** | C++23 (`/std:c++latest` on MSVC, `-std=c++23` on Clang/GCC) |
| **Dependencies** | [`nlohmann/json`](https://github.com/nlohmann/json) v3.12.0+, [`ctre`](https://github.com/hanickadot/compile-time-regular-expressions) v3.11.0+ |
| **Platform Support** | Windows (MSVC 2022+), Linux (GCC 14+, Clang 17+), macOS (Apple Clang 15+) |

---

## Navigation & Documentation Sections

- [**Getting Started**](integration/cmake.md): Quick start, CMake CPM integration, [Project Dependencies](integration/dependencies.md), and [Practical Examples](examples/index.md).
- [**API Reference**](api/sipmessage.md): Complete class reference for [`sipmessage`](api/sipmessage.md), [`sip2json`](api/sip2json.md) parser functions, and [Exceptions & Errors](api/errors.md).
- [**Core Concepts & Architecture**](features/json_schema.md): JSON schema metaphor, [Stream Data Flow](architecture/dataflow.md), [SDP Media Processing](features/sdp.md), and [Standards Compliance & Torture Tests](features/compliance.md).
- [**Performance & Benchmarks**](features/benchmarks.md): Multi-platform throughput and latency metrics, [Optimization Choices](features/optimization_choices.md), and [Native Struct vs JSON Study](architecture/native_vs_json.md).
