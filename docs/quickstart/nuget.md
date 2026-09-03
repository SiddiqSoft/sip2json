# NuGet Package Integration (Deprecated)

!!! caution "NuGet Integration Deprecated"
    Beginning with `v2.5.0+`, `sip2json` moved to a modern C++23 architecture, and publication of `SiddiqSoft.sip2json` to nuget.org has been deprecated.

    **Why NuGet is No Longer Supported**:

    1. Static header-only NuGet packages cannot reliably resolve modern C++ dependencies (`nlohmann_json`) or manage C++23 standard library settings across different MSVC toolchain versions.
    2. CMake (`CPM.cmake` / `FetchContent`) provides first-class cross-platform dependency resolution across Windows, Linux, and macOS.
    3. Publication of `SiddiqSoft.sip2json` to nuget.org is disabled for main releases.

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

!!! tip "Windows Setup Note"
    Run [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) as Administrator to configure Windows Registry `LongPathsEnabled = 1` and Git `core.longpaths = true` for CPM package caches.
