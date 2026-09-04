# sip2json: A Focused SIP Parser for Modern C++

<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![NuGet Version](https://img.shields.io/nuget/v/SiddiqSoft.sip2json?logo=nuget)](https://www.nuget.org/packages/SiddiqSoft.sip2json/)
[![NuGet Downloads](https://img.shields.io/nuget/dt/SiddiqSoft.sip2json?logo=nuget)](https://www.nuget.org/packages/SiddiqSoft.sip2json/)
[![Tests](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/21/master.svg)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/20)
[![License BSD-3](https://img.shields.io/badge/License-BSD--3--Clause-blue)](LICENSE)

[![IETF RFC 3261](https://img.shields.io/badge/IETF-RFC%203261%20(SIP)-003366)](https://datatracker.ietf.org/doc/html/rfc3261)
[![IETF RFC 4475](https://img.shields.io/badge/IETF-RFC%204475%20(Torture%2050/50)-2ea44f)](https://datatracker.ietf.org/doc/html/rfc4475)
[![IETF RFC 8866](https://img.shields.io/badge/IETF-RFC%208866%20(SDP)-003366)](https://datatracker.ietf.org/doc/html/rfc8866)
[![W3C WebRTC SDP](https://img.shields.io/badge/W3C-WebRTC%20SDP-1488C6)](https://datatracker.ietf.org/doc/html/rfc8829)
<!-- end badges -->

**`sip2json`** is a header-only Modern C++20 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

---

## Documentation

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

## Standards Compliance & Certification Test Suite

`sip2json` includes an automated test suite featuring dedicated RFC compliance and torture test suites located in `tests/compliance/`:

- **RFC 3261 Core Compliance** (`tests/compliance/rfc3261_compliance_tests.cpp`): Validates 14 standard RFC request methods, status line classes (1xx-6xx), case-insensitive header canonicalization (`vIa`, `fRoM`, `cALL-id`), 10 compact header abbreviations (`v`, `f`, `t`, `i`, `c`, `l`, `m`, `s`, `k`, `e`), and body framing.
- **RFC 4475 SIP Torture Tests** (`tests/compliance/rfc4475_torture_tests.cpp`): All **50 official bit-exact IETF torture test cases** (`.dat` files) including multiline header folding with LWSP (`\r\n\t` / `\r\n `), non-ASCII & empty reason phrases, URI escaping, unknown extension header preservation, multiple `Via` header array formatting, negative `Content-Length` rejection, and truncated stream buffer handling.
- **SIP Standard Certification Suite** (`tests/compliance/sip_certification_suite.cpp`): Full end-to-end certification for RFC 3261, RFC 3262 (`PRACK`), RFC 6665 (`Event`/`Subscription-State`), RFC 3515 (`REFER`), and RFC 3903 (`PUBLISH`).
- **SDP RFC 4566 / 8866 / 3264 / WebRTC (RFC 8829 / 8839) Compliance Suite** (`tests/compliance/sdp_compliance_tests.cpp`): Complete Session Description Protocol parsing, Offer/Answer direction flags (`sendrecv`, `sendonly`, `recvonly`, `inactive`), WebRTC BUNDLE media grouping (`a=group:BUNDLE`), ICE candidates (`a=candidate`), ICE credentials (`a=ice-ufrag`, `a=ice-pwd`), DTLS fingerprints (`a=fingerprint`), multiple SDP session blocks (`v=0` demarcation), and UNIX `\n` line endings.

---

## License

Licensed under the [BSD 3-Clause License](LICENSE).