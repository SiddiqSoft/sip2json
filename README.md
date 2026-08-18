# sip2json: A Focused SIP Parser for Modern C++

<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
![](https://img.shields.io/nuget/v/siddiqsoft.sip2json)
![](https://img.shields.io/nuget/dt/siddiqsoft.sip2json)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/21/master.svg)
<!-- end badges -->

**`sip2json`** is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

* **Header-Only Library**: Direct include and zero compiled binary dependencies.
* **JSON-First Metaphor**: Request/status lines, headers, and SDP bodies are represented as compact `nlohmann::json` objects.
* **Asynchronous Stream Parsing**: High-performance stream iterator parser with non-blocking callbacks for multi-frame TCP buffers.
* **Full SDP Support**: Native decoding and encoding of Session Description Protocol (`application/sdp`) payloads.
* **Modern C++23**: Built for C++23 standards using Compile-Time Regular Expressions (CTRE), move semantics, and concepts.

---

## Documentation Site

Full guides, tutorials, API specifications, and interactive dependency graphs are hosted on our documentation site:

* 🚀 [**Features & Usage**](docs/features/index.md): Asynchronous stream parsing, JSON schema metaphor, SDP support.
* 📦 [**Integration & CMake**](docs/integration/cmake.md): `CPMAddPackage`, Git submodules, build options, testing.
* 📊 [**Dependency Graph**](docs/integration/dependencies.md): Automated visual dependency diagram and version matrix.
* 🏗️ [**Architecture & Design**](docs/architecture/index.md): Stateless design, design patterns, and data flow.
* 📖 [**API Reference**](docs/api/index.md): Specifications for `sipmessage`, `sip2json` static methods, and exceptions.
* 💡 [**Examples**](docs/examples/index.md): Standalone code examples for stream parsing and serialization.

---

## Quick Start

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

## Integration

### Using CPM / FetchContent

```cmake
CPMAddPackage("gh:SiddiqSoft/sip2json#v1.17.0")
target_link_libraries(${PROJECT_NAME} INTERFACE sip2json::sip2json)
```

### Git Submodule

```bash
git submodule add https://github.com/SiddiqSoft/sip2json.git vendor/sip2json
```

```cmake
add_subdirectory(vendor/sip2json)
target_link_libraries(your_target PRIVATE sip2json::sip2json)
```

For full setup guides and NuGet usage, view the [Integration Guide](docs/integration/index.md).

---

## Requirements & Building

| Requirement | Details |
| :--- | :--- |
| **Language Standard** | C++23 (`/std:c++latest` on MSVC, `-std=c++23` on Clang/GCC) |
| **Platforms** | Windows (MSVC 2022+), Linux (GCC 14+, Clang 17+), macOS (Apple Clang 15+) |
| **Dependencies** | [`nlohmann/json`](https://github.com/nlohmann/json), [`ctre`](https://github.com/hanickadot/compile-time-regular-expressions) |

### Building with CMake Presets

```bash
# Configure with a preset matching your OS/compiler (e.g. Apple-Debug, Linux-GCC-Debug, Windows-Debug)
cmake --preset Apple-Debug

# Build target
cmake --build --preset Apple-Debug

# Run test suite
ctest --preset Apple-Debug
```

---

## Tests

The project includes a comprehensive GoogleTest unit test suite covering parsing, serialization, edge cases, synthetic streams, stress tests, and Rule of Five copy/move compliance.

```bash
cmake -Dsip2json_BUILD_TESTS=ON -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## License

Licensed under the [BSD 3-Clause License](LICENSE).