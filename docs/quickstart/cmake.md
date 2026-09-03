# CMake & Submodules Integration

`sip2json` provides full CMake target support with exported target `sip2json::sip2json`.

---

## Modern CMake Integration via CPM

[CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) is the recommended package manager for C++ CMake projects:

```cmake
cmake_minimum_required(VERSION 3.23)
project(MySipApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/sip2json#{ tag_version }")

add_executable(MySipApp main.cpp)
target_link_libraries(MySipApp PRIVATE sip2json::sip2json)
```

`CPMAddPackage` automatically resolves the dependent `nlohmann_json` package.

---

## FetchContent Integration

If you prefer CMake's built-in `FetchContent` module:

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

## Build Options

| Option | Default | Description |
| :--- | :--- | :--- |
| `sip2json_HEADERKEY_MODE_INSENSITIVE` | `ON` | Enable RFC 3261 case-insensitive header key matching and normalization to canonical Pascal-Kebab-Case keys (`Content-Length`, `Via`, `Call-ID`, etc.) and compact form abbreviations (`l`, `v`, `i`, `c`, `m`, `f`, `t`, `s`, `e`). |
| `sip2json_BUILD_TESTS` | `OFF` | Build unit tests (requires GoogleTest) |
| `sip2json_BUILD_BENCHMARKS` | `OFF` | Build performance benchmark suite |

---

## Building from Source with CMake Presets

If you are cloning and developing `sip2json` directly, the repository provides multi-platform CMake Presets (`Apple-Debug`, `Apple-Release`, `Darwin-Clang-Release`, `Linux-Clang-Release`, `Linux-GCC-Release`, `Windows-x64-Release`):

```bash
# Configure, build, and test
cmake --preset Darwin-Clang-Release
cmake --build --preset Darwin-Clang-Release
ctest --preset Darwin-Clang-Release -j 4
```

For full preset architecture, project configuration via `project-base.json`, and CI/CD workflows, see the [Maintainer Guide & CI/CD Pipeline](../maintainers/pipelines.md).

---

!!! tip "Windows Environment Setup Note"
    When building on Windows, run [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) as Administrator to enable Registry `LongPathsEnabled = 1` and Git `core.longpaths = true` for CPM package caches. See the [Quick Start Guide](index.md#windows-prerequisites-long-paths) for full details.
