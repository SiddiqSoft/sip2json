# Quick Start

## 1. Add to CMake

=== "CPM.cmake"

    ```cmake
    include(cmake/CPM.cmake)
    CPMAddPackage("gh:SiddiqSoft/sip2json#{ tag_version }")
    target_link_libraries(your_target PRIVATE sip2json::sip2json)
    ```

=== "FetchContent"

    ```cmake
    include(FetchContent)
    FetchContent_Declare(
        sip2json
        GIT_REPOSITORY https://github.com/SiddiqSoft/sip2json.git
        GIT_TAG        { tag_version }
    )
    FetchContent_MakeAvailable(sip2json)
    target_link_libraries(your_target PRIVATE sip2json::sip2json)
    ```

---

## 2. Include Header

```cpp
#include "siddiqsoft/sip2json.hpp"
```

---

## 3. Parse or Serialize

=== "Parse Stream"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    int main() {
        std::string raw = "REGISTER sip:example.com SIP/2.0\r\nCall-ID: abc-123\r\nCSeq: 1 REGISTER\r\nContent-Length: 0\r\n\r\n";
        
        siddiqsoft::sip2json::parseAsync(raw, [](siddiqsoft::sipmessage&& msg) {
            std::cout << msg.getMethod() << " " << msg.getUri() << "\n";
        });
        return 0;
    }
    ```

=== "Serialize"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    int main() {
        siddiqsoft::sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:user@example.com", "call-id-998", 1);
        msg.setHeader(siddiqsoft::HF_FROM, "sip:caller@example.com")
           .setHeader(siddiqsoft::HF_TO, "sip:user@example.com");

        std::cout << siddiqsoft::sip2json::serialize(msg) << "\n";
        return 0;
    }
    ```

---

## Topics

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [CMake & CPM Integration](cmake.md)

Detailed CMake, FetchContent, and build options.

[CMake Guide :octicons-arrow-right-24:](cmake.md)

</div>

<div class="card" markdown="1">

### [Project Dependencies](dependencies.md)

Dependency breakdown (`nlohmann_json`).

[Dependencies :octicons-arrow-right-24:](dependencies.md)

</div>

<div class="card" markdown="1">

### [NuGet Package](nuget.md)

Visual Studio and MSBuild setup.

[NuGet Guide :octicons-arrow-right-24:](nuget.md)

</div>

</div>

---

## Windows Prerequisites

Enable long paths for CPM package caching:

```powershell
# Run as Administrator
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
git config --global core.longpaths true
```
