# NuGet Package Integration (Deprecated)

> [!CAUTION]
> **NuGet Integration Deprecated**
> Beginning with `v2.5.0+`, `sip2json` relies on [CTRE](https://github.com/ctre-mc/compile-time-regular-expressions) (Compile-Time Regular Expressions) for compile-time SIP regex parsing.
>
> **Why NuGet is No Longer Supported**:
> 1. CTRE is a third-party C++23 header-only library that is **not distributed or available on nuget.org**.
> 2. Static header-only NuGet packages cannot resolve nested modern C++ CMake dependencies or handle C++23 header include paths cleanly across different Visual Studio toolchain versions.
> 3. Publication of `SiddiqSoft.sip2json` to nuget.org is disabled by default for main/master releases.

---

## Recommended Alternatives

Users on Windows and Visual Studio are strongly advised to migrate to modern CMake integration methods:

### 1. CPM.cmake Integration (Recommended)

Add [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) to your CMake configuration:

```cmake
include(cmake/CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/sip2json#{ tag_version }")
target_link_libraries(your_target PRIVATE sip2json::sip2json)
```

### 2. CMake FetchContent

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

> [!TIP]
> **Windows Setup Note**: Run [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) as Administrator to configure Windows Registry `LongPathsEnabled = 1` and Git `core.longpaths = true` for CTRE header resolution.
