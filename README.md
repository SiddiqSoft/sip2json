# sip2json: A Focused SIP Parser for Modern C++

<!-- badges -->
[![Version](https://img.shields.io/badge/version-v2.6.0-4f46e5.svg)](https://github.com/SiddiqSoft/sip2json/releases)
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![Total Tests](https://img.shields.io/badge/Total%20Tests-321%20Passed%20(100%25)-2ea44f?logo=checkmarx&logoColor=white)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![RFC 4475 Torture Tests](https://img.shields.io/badge/RFC%204475-50%2F50%20Torture%20(100%25)-success?logo=testinglibrary&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![RFC 3261 Compliance](https://img.shields.io/badge/RFC%203261-SIP%20Core%20Validated-003366?logo=shield&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![SDP & WebRTC Suite](https://img.shields.io/badge/SDP%20%26%20WebRTC-RFC%208866%20%7C%20BUNDLE%20%7C%20ICE-1488C6?logo=webrtc&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/sdp/)
[![SIP Extensions](https://img.shields.io/badge/SIP%20Extensions-PRACK%20%7C%20NOTIFY%20%7C%20REFER-6f42c1?logo=gitbook&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![Security & Memory Safety](https://img.shields.io/badge/Security-Memory%20%26%20CRLF%20Safe-107C41?logo=securityscorecard&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/23)
[![License BSD-3](https://img.shields.io/badge/License-BSD--3--Clause-blue)](LICENSE)
<!-- end badges -->

**`sip2json`** is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

---

## Design Objectives

* **Header-Only Library**: Easy integration without compiled binary dependencies; include and build.
* **JSON as First-Class Metaphor**: Compact and intuitive representation of SIP start lines, headers, and SDP bodies.
* **Modern C++23**: Built for C++23 standards using concepts, string views, move semantics, and CTRE (Compile-Time Regular Expressions).
* **Asynchronous & Streaming Parsing**: High-performance stream iterator parsing with non-blocking callbacks for multi-frame TCP buffers.
* **Full SDP Support**: Native decoding and encoding of Session Description Protocol (`application/sdp`) payloads.
* Use this library to build your application server that suits the purpose of:
  * Firing events to your storage system (not just for logging)
  * AI/ML/analytics use
  * Firing to ServiceBus / MessageBus / RabbitMQ for processing by serverless or microservices
  * Building a REST server that allows handling of call processing via micro-services

---

## Documentation Site

The complete documentation, API reference, architecture guides, performance benchmarks, and interactive dependency charts are hosted on our documentation site:

**[siddiqsoft.github.io/sip2json](https://siddiqsoft.github.io/sip2json/)**

* [**Quick Start & CMake Integration**](https://siddiqsoft.github.io/sip2json/integration/cmake/)
* [**Project Dependencies**](https://siddiqsoft.github.io/sip2json/integration/dependencies/)
* [**Practical Examples**](https://siddiqsoft.github.io/sip2json/examples/)
* [**Performance & Benchmarks**](https://siddiqsoft.github.io/sip2json/features/benchmarks/)
* [**Optimization Choices & Hardware Study**](https://siddiqsoft.github.io/sip2json/features/optimization_choices/)
* [**Standards Compliance & Torture Tests**](https://siddiqsoft.github.io/sip2json/features/compliance/)
* [**API Reference**](https://siddiqsoft.github.io/sip2json/api/sipmessage/)

---

## Quick Examples

### 1. Asynchronous Stream Parsing

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

void onNetworkDataReceived(std::string& tcpReadBuffer)
{
    // Asynchronously parse multiple SIP frames from buffer
    sip2json::parseAsync(
        tcpReadBuffer,
        [](sipmessage&& msg) {
            if (!msg.empty()) {
                std::cout << "Parsed " << msg.method << " request for " << msg.uri
                          << " | Call-ID: " << msg.callId << "\n";
            }
        },
        [](const sip2json_exception& ex, std::string::iterator& start, const std::string::iterator& end) {
            std::cerr << "Parser warning: " << ex.what() << "\n";
        }
    );
    // Note: sip2json::parseAsync automatically erases decoded messages from tcpReadBuffer.
}
```

### 2. Message Construction & Serialization

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

int main()
{
    // Construct SIP INVITE request
    sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:user@example.com", "call-8849-xyz", 1);
    
    msg.setHeader(siddiqsoft::HF_FROM, "sip:caller@example.com")
       .setHeader(siddiqsoft::HF_TO, "sip:user@example.com")
       .setHeader(siddiqsoft::HF_USER_AGENT, "sip2json/2.6");

    // Serialize to standard SIP wire format string
    std::string rawSip = sip2json::serialize(msg);
    std::cout << rawSip << std::endl;

    return 0;
}
```

### 3. Direct JSON Conversion

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

int main()
{
    auto bs = rawSipString.begin();
    sipmessage msg = sip2json::parseFromBuffer(bs, rawSipString.end());

    // Convert sipmessage directly to nlohmann::json representation
    nlohmann::json doc = msg;
    std::cout << doc.dump(2) << std::endl;

    return 0;
}
```

---

## Quick Integration

> [!NOTE]
> On Windows machines, execute the [`prep_windows_machine.ps1`](scripts/prep_windows_machine.ps1) setup script to enable long paths and configure MSVC CTRE requirements.

### Using CMake CPM

```cmake
CPMAddPackage("gh:SiddiqSoft/sip2json#v2.6.0")
target_link_libraries(${PROJECT_NAME} INTERFACE sip2json::sip2json)
```

### Using CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    sip2json
    GIT_REPOSITORY https://github.com/SiddiqSoft/sip2json.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(sip2json)
target_link_libraries(${PROJECT_NAME} INTERFACE sip2json::sip2json)
```

---

## Requirements

| Requirement | Details |
| :--- | :--- |
| **Language Standard** | C++23 (`/std:c++latest` on MSVC, `-std=c++23` on Clang/GCC) |
| **Dependencies** | [`nlohmann/json`](https://github.com/nlohmann/json) v3.12.0+, [`ctre`](https://github.com/hanickadot/compile-time-regular-expressions) v3.11.0+ |
| **Platform Support** | Windows (MSVC 2022+), Linux (GCC 14+, Clang 17+), macOS (Apple Clang 15+) |

---

## Configuration Options

`sip2json` provides the following CMake options when integrated into host projects:

| CMake Option | Default | Description |
| :--- | :--- | :--- |
| `sip2json_HEADERKEY_MODE_INSENSITIVE` | `ON` | Enable RFC 3261 case-insensitive header key matching and normalization to canonical Pascal-Kebab-Case keys (`Content-Length`, `Via`, `Call-ID`, etc.) and compact form abbreviations (`l`, `v`, `i`, `c`, `m`, `f`, `t`, `s`, `e`). |
| `sip2json_BUILD_TESTS` | `OFF` | Build CTest unit test suite (321 tests). |
| `sip2json_BUILD_BENCHMARKS` | `OFF` | Build Google Benchmark & stream performance test suite. |

---

## Standards Compliance & Certification Test Suite

`sip2json` includes an automated test suite featuring dedicated RFC compliance and torture test suites located in `tests/compliance/`:

- **RFC 3261 Core Compliance** (`tests/compliance/src/rfc3261_compliance_tests.cpp`): Validates 14 standard RFC request methods, status line classes (1xx-6xx), case-insensitive header canonicalization (`vIa`, `fRoM`, `cALL-id`), 10 compact header abbreviations (`v`, `f`, `t`, `i`, `c`, `l`, `m`, `s`, `k`, `e`), and body framing.
- **RFC 4475 SIP Torture Tests** (`tests/compliance/src/rfc4475_torture_tests.cpp`): All **50 official bit-exact IETF torture test cases** (`.dat` files) including multiline header folding with LWSP (`\r\n\t` / `\r\n `), non-ASCII & empty reason phrases, URI escaping, unknown extension header preservation, multiple `Via` header array formatting, negative `Content-Length` rejection, and truncated stream buffer handling.
- **SIP Standard Certification Suite** (`tests/compliance/src/sip_certification_suite.cpp`): Full end-to-end certification for RFC 3261, RFC 3262 (`PRACK`), RFC 6665 (`Event`/`Subscription-State`), RFC 3515 (`REFER`), and RFC 3903 (`PUBLISH`).
- **SDP RFC 4566 / 8866 / 3264 / WebRTC (RFC 8829 / 8839) Compliance Suite** (`tests/compliance/src/sdp_compliance_tests.cpp`): Complete Session Description Protocol parsing, Offer/Answer direction flags (`sendrecv`, `sendonly`, `recvonly`, `inactive`), WebRTC BUNDLE media grouping (`a=group:BUNDLE`), ICE candidates (`a=candidate`), ICE credentials (`a=ice-ufrag`, `a=ice-pwd`), DTLS fingerprints (`a=fingerprint`), multiple SDP session blocks (`v=0` demarcation), and UNIX `\n` line endings.

---

## License

Licensed under the [BSD 3-Clause License](LICENSE).