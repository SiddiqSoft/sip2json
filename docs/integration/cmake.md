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

`CPMAddPackage` automatically resolves dependent packages (`nlohmann_json` and `ctre`).

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

## Git Submodule Integration

For vendored repository workflows:

```bash
git submodule add https://github.com/SiddiqSoft/sip2json.git vendor/sip2json
```

```cmake
add_subdirectory(vendor/sip2json)
target_link_libraries(your_target PRIVATE sip2json::sip2json)
```

---

## CMake Configuration Options

| Option | Default | Description |
| :--- | :---: | :--- |
| `sip2json_HEADERKEY_MODE_INSENSITIVE` | `ON` | Enable RFC 3261 case-insensitive header key matching and normalization to canonical Pascal-Kebab-Case keys (`Content-Length`, `Via`, `Call-ID`, etc.) and compact form abbreviations (`l`, `v`, `i`, `c`, `m`, `f`, `t`, `s`, `e`). |
| `sip2json_BUILD_TESTS` | `OFF` | Build compliance, torture, and validation test suites (requires GoogleTest via CPM). |
| `sip2json_BUILD_BENCHMARKS` | `OFF` | Build the stream parsing performance benchmark executable. |

---

## Windows Prerequisites & Long Paths (CTRE Support)

When building on Windows, the underlying **CTRE (Compile-Time Regular Expressions)** library and CMake package caches generate deeply nested header and template expansion paths that can exceed the legacy 260-character Windows path limit (`MAX_PATH`).

> [!TIP]
> **Automated Windows Machine Configuration**
> Run [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) in an Administrator PowerShell session to enable Registry `LongPathsEnabled = 1` and Git `core.longpaths = true`:
> ```powershell
> powershell -ExecutionPolicy Bypass -File .\scripts\prep_windows_machine.ps1
> ```

### Manual Windows Configuration Steps

1. **Enable Windows Registry Long Paths (`LongPathsEnabled`)**:
   ```powershell
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
   ```
2. **Enable Git Long Paths Support**:
   ```bash
   git config --global core.longpaths true
   ```
3. **CPM Cache Path Optimization**:
   ```cmake
   set(CPM_SOURCE_CACHE "C:/cpmcache" CACHE PATH "CPM Cache Directory")
   ```

