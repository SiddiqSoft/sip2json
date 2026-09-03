# Quick Start Guide

`sip2json` is a header-only Modern C++23 SIP protocol parser and serializer library. It requires zero compiled binary dependencies and integrates into Windows, Linux, and macOS CMake builds in minutes.

---

## 3-Step Rapid Onboarding

### Step 1: Add to Your CMake Project

Choose your preferred integration method:

=== "CPM.cmake (Recommended)"

    ```cmake
    include(cmake/CPM.cmake)

    CPMAddPackage("gh:SiddiqSoft/sip2json#{ tag_version }")
    target_link_libraries(your_target PRIVATE sip2json::sip2json)
    ```

=== "CMake FetchContent"

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

=== "Git Submodule"

    ```bash
    git submodule add https://github.com/SiddiqSoft/sip2json.git vendor/sip2json
    ```

    ```cmake
    add_subdirectory(vendor/sip2json)
    target_link_libraries(your_target PRIVATE sip2json::sip2json)
    ```

### Step 2: Include the Header

```cpp
#include "siddiqsoft/sip2json.hpp"
```

### Step 3: Parse or Serialize

=== "Parse SIP Stream"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main() {
        std::string raw = "REGISTER sip:example.com SIP/2.0\r\nCall-ID: abc-123\r\nCSeq: 1 REGISTER\r\nContent-Length: 0\r\n\r\n";
        auto it = raw.begin();
        
        sip2json::parseAsync(it, raw.end(), [](sipmessage&& msg) {
            std::cout << "Parsed " << msg.method << " request for " << msg.uri << "\n";
            std::cout << "Call-ID: " << msg.callid << "\n";
        });
        return 0;
    }
    ```

=== "Construct & Serialize"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    int main() {
        sipmessage msg(METHOD_INVITE, "sip:user@example.com", "call-id-998", 1);
        msg.setHeader(HF_FROM, "sip:caller@example.com")
           .setHeader(HF_TO, "sip:user@example.com");

        std::string wire = sip2json::serialize(msg);
        std::cout << wire << std::endl;
        return 0;
    }
    ```

---

## Quick Start Topics

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [CMake & CPM Integration](cmake.md)

Detailed setup guide for CMake, CPM, FetchContent, build options, and CMake presets.

[View CMake Guide :octicons-arrow-right-24:](cmake.md)

</div>

<div class="card" markdown="1">

### [Project Dependencies](dependencies.md)

Dependency hierarchy diagram and version breakdown (`nlohmann_json`).

[View Dependencies :octicons-arrow-right-24:](dependencies.md)

</div>

<div class="card" markdown="1">

### [NuGet Package Integration](nuget.md)

Native Visual Studio and MSBuild package integration for C++23 projects.

[View NuGet Guide :octicons-arrow-right-24:](nuget.md)

</div>

</div>

---

## Windows Prerequisites & Long Paths

When building on Windows, CMake package caches (`.cpmcache`) can generate paths that approach the legacy 260-character Windows limit (`MAX_PATH`). Note that previous versions required CTRE compiler template workarounds, which have been completely removed in `v3.0.0+` thanks to zero-copy `std::string_view` parsing.

!!! tip "Windows Long Paths & CPM Cache"
    Enable Windows Extended Long Paths in the System Registry (`LongPathsEnabled = 1`) and Git configuration (`git config --global core.longpaths true`) for smooth CPM package caching.

    You can automatically configure your Windows machine by running the provided PowerShell script [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) as Administrator:
    ```powershell
    powershell -ExecutionPolicy Bypass -File .\scripts\prep_windows_machine.ps1
    ```

### Manual Configuration

1. **Windows Registry Long Paths (`LongPathsEnabled`)**:
   ```powershell
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
   ```
2. **Git Long Paths Support**:
   ```bash
   git config --global core.longpaths true
   ```
3. **CPM Cache Path Optimization (Optional)**:
   ```cmake
   set(CPM_SOURCE_CACHE "C:/cpmcache" CACHE PATH "CPM Cache Directory")
   ```
