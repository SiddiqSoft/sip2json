# sip2json: A Focused SIP Parser for Modern C++

<div class="badge-container">
  <img src="https://img.shields.io/badge/version-v{{ version }}-4f46e5.svg" alt="Version {{ version }}"/>
  <a href="https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master"><img src="https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master" alt="Build Status"></a>
  <a href="https://www.nuget.org/packages/siddiqsoft.sip2json"><img src="https://img.shields.io/nuget/v/siddiqsoft.sip2json" alt="NuGet Version"></a>
  <a href="https://www.nuget.org/packages/siddiqsoft.sip2json"><img src="https://img.shields.io/nuget/dt/siddiqsoft.sip2json" alt="NuGet Downloads"></a>
  <a href="https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master"><img src="https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/21/master.svg" alt="Tests" /></a>
</div>

**`sip2json`** (`v{{ version }}`) is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

---

## Design Objectives

* **Header-Only Library**: Easy integration without compiled binary dependencies; include and build.
* **JSON as First-Class Metaphor**: Compact and intuitive representation of SIP start lines, headers, and SDP bodies.
* **Modern C++23**: Built for C++23 standards using concepts, zero-copy string views, and move semantics with zero regex dependencies.
* **Asynchronous & Streaming Parsing**: High-performance stream iterator parsing with non-blocking callbacks for multi-frame TCP buffers.
* **Full SDP Support**: Native decoding and encoding of Session Description Protocol (`application/sdp`) payloads.

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
| **Dependencies** | [`nlohmann/json`](https://github.com/nlohmann/json) v3.12.0+ (zero regex dependencies) |
| **Platform Support** | Windows (MSVC 2022+), Linux (GCC 14+, Clang 17+), macOS (Apple Clang 15+) |

---

## Documentation Themes

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [Quick Start](quickstart/index.md)

Get up and running in minutes. Includes CMake CPM & FetchContent integration, project dependencies, and Windows environment prerequisites.

[Start Quick Start :octicons-arrow-right-24:](quickstart/index.md)

</div>

<div class="card" markdown="1">

### [API Reference](api/index.md)

Comprehensive reference for `sipmessage`, `sip2json` parser/serializer functions, error handling, JSON schema mapping, SDP support, and [**Code Examples**](api/examples/index.md).

[Explore API Reference :octicons-arrow-right-24:](api/index.md)

</div>

<div class="card" markdown="1">

### [Architecture & Design](architecture/index.md)

Deep-dive into zero-copy data flow, design patterns, 64-bit FNV-1a hash matching, [**Performance Benchmarks**](architecture/benchmarks.md), and [**Standards Compliance**](architecture/compliance.md).

[Read Architecture Guide :octicons-arrow-right-24:](architecture/index.md)

</div>

<div class="card" markdown="1">

### [Maintainer Guide](maintainers/pipelines.md)

Complete CI/CD workflow guide, cross-platform CMake presets architecture, dynamic versioning hooks, and Azure DevOps release automation.

[View Maintainer Guide :octicons-arrow-right-24:](maintainers/pipelines.md)

</div>

</div>

