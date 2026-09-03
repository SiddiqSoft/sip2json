# sip2json

<div class="badge-container">
  <img src="https://img.shields.io/badge/version-v{{ version }}-4f46e5.svg" alt="Version {{ version }}"/>
  <a href="https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master"><img src="https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master" alt="Build Status"></a>
  <a href="https://www.nuget.org/packages/siddiqsoft.sip2json"><img src="https://img.shields.io/nuget/v/siddiqsoft.sip2json" alt="NuGet Version"></a>
  <a href="https://www.nuget.org/packages/siddiqsoft.sip2json"><img src="https://img.shields.io/nuget/dt/siddiqsoft.sip2json" alt="NuGet Downloads"></a>
  <a href="https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master"><img src="https://dev.azure.com/siddiqsoft/siddiqsoft/21/master.svg" alt="Tests" /></a>
</div>

Header-only C++23 SIP protocol parser and serializer. Represents SIP messages, headers, and SDP bodies as `nlohmann::json` documents with zero regex overhead and zero-copy string views.

---

## Quick Example

=== "Stream Parsing"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    // Parse stream buffer asynchronously
    siddiqsoft::sip2json::parseAsync(
        buffer,
        [](siddiqsoft::sipmessage&& msg) {
            std::cout << msg.getMethod() << " " << msg.getUri() << "\n";
        }
    );
    ```

=== "Serialization"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    siddiqsoft::sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:user@example.com", "call-id-123", 1);
    msg.setHeader(siddiqsoft::HF_FROM, "sip:caller@example.com")
       .setHeader(siddiqsoft::HF_TO, "sip:user@example.com");

    std::string wire = siddiqsoft::sip2json::serialize(msg);
    ```

=== "JSON Access"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    auto msg = siddiqsoft::sip2json::parse(rawSip);
    nlohmann::json doc = msg; // Direct JSON conversion
    std::cout << doc.dump(2) << "\n";
    ```

---

## Requirements

| Requirement | Details |
| :--- | :--- |
| **Language** | C++23 (`/std:c++latest` on MSVC, `-std=c++23` on Clang/GCC) |
| **Dependencies** | [`nlohmann/json`](https://github.com/nlohmann/json) v3.12+ (zero regex dependencies) |
| **Platforms** | Windows (MSVC 2022+), Linux (GCC 14+, Clang 17+), macOS (Apple Clang 15+) |

---

## Documentation

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [Quick Start](quickstart/index.md)

CMake, CPM, NuGet, and build setup.

[Quick Start :octicons-arrow-right-24:](quickstart/index.md)

</div>

<div class="card" markdown="1">

### [API Reference](api/index.md)

`sipmessage`, parser functions, error codes, and examples.

[API Reference :octicons-arrow-right-24:](api/index.md)

</div>

<div class="card" markdown="1">

### [Architecture & Design](architecture/index.md)

Zero-copy data flow, hash dispatch, benchmarks, and RFC compliance.

[Architecture :octicons-arrow-right-24:](architecture/index.md)

</div>

<div class="card" markdown="1">

### [Maintainer Guide](maintainers/pipelines.md)

CI/CD matrix, CMake presets, and release automation.

[Maintainers :octicons-arrow-right-24:](maintainers/pipelines.md)

</div>

</div>
