# Integration Guide

`sip2json` is a header-only Modern C++23 library designed for fast, seamless integration into build setups on Windows, Linux, and macOS.

---

## Integration Options

Choose your preferred integration method:

=== "CPM / CMake"

    Recommended for cross-platform C++23 CMake projects.

    ```cmake
    CPMAddPackage("gh:SiddiqSoft/sip2json#v1.17.0")
    target_link_libraries(your_target PRIVATE sip2json::sip2json)
    ```

    [View CMake Integration Guide :octicons-arrow-right-24:](cmake.md)

=== "Git Submodule"

    Ideal for vendored source trees.

    ```bash
    git submodule add https://github.com/SiddiqSoft/sip2json.git vendor/sip2json
    ```

    ```cmake
    add_subdirectory(vendor/sip2json)
    target_link_libraries(your_target PRIVATE sip2json::sip2json)
    ```

    [View CMake Integration Guide :octicons-arrow-right-24:](cmake.md)

=== "NuGet Package"

    Supported for Visual Studio C++ projects on Windows.

    ```powershell
    Install-Package SiddiqSoft.sip2json
    ```

    [View NuGet Integration Guide :octicons-arrow-right-24:](nuget.md)

---

## Primary Header Include

Include the primary header in your application source files:

```cpp
#include "siddiqsoft/sip2json.hpp"
```

---

## Windows Prerequisites & Long Paths (CTRE Support)

When building on Windows, the underlying **CTRE (Compile-Time Regular Expressions)** library and CMake package caches generate deeply nested header and template expansion paths that can exceed the legacy 260-character Windows path limit (`MAX_PATH`). This can trigger `filename too long` warnings or C1083 compilation errors during Git checkout or MSVC build steps.

> [!WARNING]
> **Windows `filename too long` & MSVC CTRE Fix**
> Enable Windows Extended Long Paths in the System Registry (`LongPathsEnabled = 1`) and Git configuration (`git config --global core.longpaths true`) before building on Windows.
>
> You can automatically configure your Windows machine by running the provided PowerShell script [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) as Administrator:
> ```powershell
> powershell -ExecutionPolicy Bypass -File .\scripts\prep_windows_machine.ps1
> ```

### 1. Enable Windows Registry Long Paths (`LongPathsEnabled`)

Run PowerShell as Administrator:

```powershell
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
```

Or via Windows Command Prompt (Admin):

```cmd
reg add "HKLM\SYSTEM\CurrentControlSet\Control\FileSystem" /v LongPathsEnabled /t REG_DWORD /d 1 /f
```

### 2. Enable Git Long Paths Support

Configure Git globally to support extended file paths:

```bash
git config --global core.longpaths true
```

### 3. CPM Cache Path Optimization (Optional)

If using CPM package manager on Windows, relocate the cache directory closer to the drive root:

```cmake
set(CPM_SOURCE_CACHE "C:/cpmcache" CACHE PATH "CPM Cache Directory")
```

### 4. MSVC Template Instantiation Depth (`/templateDepth:4096`)

CTRE evaluates regular expression state machines during compilation. To prevent MSVC `error C2999: maximum template instantiation depth of 1000 exceeded`, `sip2json` exports `/templateDepth:4096` on its interface target. If consuming custom builds on MSVC, ensure `/templateDepth:4096` is set in your target compile options:

```cmake
if(MSVC)
    target_compile_options(your_target PRIVATE /templateDepth:4096)
endif()
```

---

## Building & Previewing Documentation Locally

To preview the documentation site locally without deploying to GitHub Pages, you can use the built-in MkDocs local development server or compile static HTML files.

### Method 1: Live-Reload Local Server (Recommended)

Start the local development server from the repository root:

```bash
# 1. Activate the Python virtual environment (or install dependencies)
source venv/bin/activate

# (Optional: install dependencies if setting up a fresh environment)
# pip install -r docs/requirements.txt

# 2. Start the local live-reloading server
mkdocs serve
```

- **Local URL**: Open [`http://127.0.0.1:8000/`](http://127.0.0.1:8000/) or [`http://localhost:8000/`](http://localhost:8000/) in your browser.
- **Live Reload**: Any edits saved in `docs/` files will automatically update in your browser in real time.

---

### Method 2: Build Static Local HTML Site

To compile static HTML files into the `site/` directory without serving:

```bash
source venv/bin/activate
mkdocs build --strict
```

The output will be placed in the `site/` directory. Open `site/index.html` directly in your browser.

---

### Method 3: Previewing Local Benchmark Data

To compile local benchmark results and update `docs/features/benchmarks.md` prior to serving:

```bash
source venv/bin/activate
python3 scripts/publish_benchmarks.py
mkdocs serve
```
