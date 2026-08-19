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

👉 **[siddiqsoft.github.io/sip2json](https://siddiqsoft.github.io/sip2json/)**

* [**Quick Start & Integration**](https://siddiqsoft.github.io/sip2json/integration/)
* [**Asynchronous Stream Parsing**](https://siddiqsoft.github.io/sip2json/features/async/)
* [**Performance & Benchmarks**](https://siddiqsoft.github.io/sip2json/features/benchmarks/)
* [**JSON Schema Metaphor**](https://siddiqsoft.github.io/sip2json/features/json_schema/)
* [**API Reference**](https://siddiqsoft.github.io/sip2json/api/)

---

## Quick Example

```cpp
#include "siddiqsoft/sip2json.hpp"

using namespace siddiqsoft;

void onNetworkDataReceived(std::string& tcpReadBuffer)
{
    auto cursor = tcpReadBuffer.begin();

    // Asynchronously parse multiple SIP frames from buffer iterator
    sip2json::parseAsync(
        cursor,
        tcpReadBuffer.end(),
        [](sipmessage&& msg) {
            if (!msg.empty()) {
                std::cout << "Parsed " << msg.type << " (" << msg.method << ") Call-ID: " << msg.callid << "\n";
            }
        },
        [](sip2jsonErrors& errCode, const std::string& errMessage) {
            std::cerr << "Parser warning: " << errMessage << "\n";
        }
    );

    // Erase processed frames from front of buffer; partial frames stay for next read
    tcpReadBuffer.erase(tcpReadBuffer.begin(), cursor);
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

## License

Licensed under the [BSD 3-Clause License](LICENSE).