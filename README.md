# sip2json: A Focused SIP Parser for Modern C++

<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![Total Tests](https://img.shields.io/badge/Total%20Tests-321%20Passed%20(100%25)-2ea44f?logo=checkmarx&logoColor=white)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![RFC 4475 Torture Suite](https://img.shields.io/badge/RFC%204475-50%2F50%20Torture%20(100%25)-success?logo=testinglibrary&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![RFC 3261 Compliance](https://img.shields.io/badge/RFC%203261-SIP%20Core%20Validated-003366?logo=shield&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![SDP & WebRTC Suite](https://img.shields.io/badge/SDP%20%26%20WebRTC-RFC%208866%20%7C%20BUNDLE%20%7C%20ICE-1488C6?logo=webrtc&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/sdp/)
[![SIP Extensions](https://img.shields.io/badge/SIP%20Extensions-PRACK%20%7C%20NOTIFY%20%7C%20REFER-6f42c1?logo=gitbook&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![Security & Memory Safety](https://img.shields.io/badge/Security-Memory%20%26%20CRLF%20Safe-107C41?logo=securityscorecard&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/compliance/)
[![Regression & Streams](https://img.shields.io/badge/Regression-36%20Streams%20%7C%20164k%20Frames-00838F?logo=speedtest&logoColor=white)](https://siddiqsoft.github.io/sip2json/features/benchmarks/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/23)
[![License BSD-3](https://img.shields.io/badge/License-BSD--3--Clause-blue)](LICENSE)
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

> **NOTE**
>
> On Windows machines, in order to compile this project, please execute the [`prep_windows_machine.ps1`](scripts/prep_windows_machine.ps1) to avoid build errors.
>


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

`sip2json` includes an automated test suite featuring dedicated RFC compliance and torture test suites located in `tests/compliance/`:

- **RFC 3261 Core Compliance** (`tests/compliance/rfc3261_compliance_tests.cpp`): Validates 14 standard RFC request methods, status line classes (1xx-6xx), case-insensitive header canonicalization (`vIa`, `fRoM`, `cALL-id`), 10 compact header abbreviations (`v`, `f`, `t`, `i`, `c`, `l`, `m`, `s`, `k`, `e`), and body framing.
- **RFC 4475 SIP Torture Tests** (`tests/compliance/rfc4475_torture_tests.cpp`): All **50 official bit-exact IETF torture test cases** (`.dat` files) including multiline header folding with LWSP (`\r\n\t` / `\r\n `), non-ASCII & empty reason phrases, URI escaping, unknown extension header preservation, multiple `Via` header array formatting, negative `Content-Length` rejection, and truncated stream buffer handling.
- **SIP Standard Certification Suite** (`tests/compliance/sip_certification_suite.cpp`): Full end-to-end certification for RFC 3261, RFC 3262 (`PRACK`), RFC 6665 (`Event`/`Subscription-State`), RFC 3515 (`REFER`), and RFC 3903 (`PUBLISH`).
- **SDP RFC 4566 / 8866 / 3264 / WebRTC (RFC 8829 / 8839) Compliance Suite** (`tests/compliance/sdp_compliance_tests.cpp`): Complete Session Description Protocol parsing, Offer/Answer direction flags (`sendrecv`, `sendonly`, `recvonly`, `inactive`), WebRTC BUNDLE media grouping (`a=group:BUNDLE`), ICE candidates (`a=candidate`), ICE credentials (`a=ice-ufrag`, `a=ice-pwd`), DTLS fingerprints (`a=fingerprint`), multiple SDP session blocks (`v=0` demarcation), and UNIX `\n` line endings.

---

## License

Licensed under the [BSD 3-Clause License](LICENSE).