# sip2json: A Focused SIP Parser for Modern C++

<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![NuGet Version](https://img.shields.io/nuget/v/siddiqsoft.sip2json)](https://www.nuget.org/packages/siddiqsoft.sip2json)
[![NuGet Downloads](https://img.shields.io/nuget/dt/siddiqsoft.sip2json)](https://www.nuget.org/packages/siddiqsoft.sip2json)
[![Tests](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/21/master.svg)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
<!-- end badges -->

**`sip2json`** is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

---

## Documentation Site

The complete documentation, API reference, architecture guides, performance benchmarks, and interactive dependency charts are hosted on our documentation site:

**[siddiqsoft.github.io/sip2json](https://siddiqsoft.github.io/sip2json/)**

* [**Quick Start & Integration**](https://siddiqsoft.github.io/sip2json/integration/)
* [**Asynchronous Stream Parsing**](https://siddiqsoft.github.io/sip2json/features/async/)
* [**Performance & Benchmarks**](https://siddiqsoft.github.io/sip2json/features/benchmarks/)
* [**JSON Schema Metaphor**](https://siddiqsoft.github.io/sip2json/features/json_schema/)
* [**API Reference**](https://siddiqsoft.github.io/sip2json/api/)

---

## Quick Example

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
                std::cout << "Parsed " << msg.getMethod() << " Call-ID: " << msg.getCallID() << "\n";
            }
        },
        [](const sip2json_exception& ex, std::string::iterator& start, const std::string::iterator& end) {
            std::cerr << "Parser warning: " << ex.what() << "\n";
        }
    );
    // Note: sip2json::parseAsync automatically erases decoded messages from tcpReadBuffer.
}
```

---

## Quick Integration

### Using CPM

```cmake
CPMAddPackage("gh:SiddiqSoft/sip2json#v0.0.0.0")
target_link_libraries(${PROJECT_NAME} INTERFACE sip2json::sip2json)
```

For full setup guides, submodules, and NuGet usage, visit the [Integration Guide](https://siddiqsoft.github.io/sip2json/integration/).

---

## Configuration Options

`sip2json` provides the following CMake options when integrated into host projects:

| CMake Option | Default | Description |
| :--- | :--- | :--- |
| `sip2json_HEADERKEY_MODE_INSENSITIVE` | `ON` | Enable RFC 3261 case-insensitive header key matching and normalization to canonical Pascal-Kebab-Case keys (`Content-Length`, `Via`, `Call-ID`, etc.) and compact form abbreviations (`l`, `v`, `i`, `c`, `m`, `f`, `t`, `s`, `e`). |
| `sip2json_BUILD_TESTS` | `OFF` | Build CTest unit test suite. |
| `sip2json_BUILD_BENCHMARKS` | `OFF` | Build Google Benchmark performance test suite. |

---

## Standards Compliance & Certification Test Suite

`sip2json` includes an automated 226-test suite featuring dedicated RFC compliance and torture test suites located in `tests/compliance/`:

- **RFC 3261 Core Compliance** (`tests/compliance/rfc3261_compliance_tests.cpp`): Validates 14 standard RFC request methods, status line classes (1xx-6xx), case-insensitive header canonicalization (`vIa`, `fRoM`, `cALL-id`), 10 compact header abbreviations (`v`, `f`, `t`, `i`, `c`, `l`, `m`, `s`, `k`, `e`), and body framing.
- **RFC 4475 SIP Torture Tests** (`tests/compliance/rfc4475_torture_tests.cpp`): Official IETF torture test cases including multiline header folding with LWSP (`\r\n\t` / `\r\n `), unknown extension header preservation, multiple `Via` header array formatting, negative `Content-Length` rejection, and truncated stream buffer handling.
- **SIP Standard Certification Suite** (`tests/compliance/sip_certification_suite.cpp`): Full end-to-end certification for RFC 3261, RFC 3262 (`PRACK`), RFC 6665 (`Event`/`Subscription-State`), RFC 3515 (`REFER`), and RFC 3903 (`PUBLISH`).
- **SDP RFC 4566 / 8866 / 3264 Compliance Suite** (`tests/compliance/sdp_compliance_tests.cpp`): Complete Session Description Protocol parsing, Offer/Answer direction flags (`sendrecv`, `sendonly`, `recvonly`, `inactive`), WebRTC ICE/DTLS attributes (`a=candidate`, `a=ice-ufrag`, `a=fingerprint`), multiple SDP session blocks (`v=0` demarcation), and UNIX `\n` line endings.

---

## License

Licensed under the [BSD 3-Clause License](LICENSE).