# Quick Start Guide

`sip2json` is a header-only Modern C++23 SIP protocol parser and serializer library. It requires zero binary dependencies and integrates into Windows, Linux, and macOS CMake builds in minutes.

---

## 3-Step Rapid Onboarding

### Step 1: Add to Your CMake Project

Use [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) (recommended) or CMake's built-in `FetchContent`:

=== "CPM.cmake (Recommended)"

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

### Step 3: Parse or Serialize in 5 Lines

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

## Quick Start Sections

<div class="grid" markdown="1">

<div class="card" markdown="1">

### [CMake & CPM Integration](cmake.md)

Detailed setup guide for CMake, CPM, FetchContent, build options, and CMake presets.

[View CMake Guide :octicons-arrow-right-24:](cmake.md)

</div>

<div class="card" markdown="1">

### [Project Dependencies](dependencies.md)

Dependency hierarchy diagram and version breakdown (`nlohmann_json` and `ctre`).

[View Dependencies :octicons-arrow-right-24:](dependencies.md)

</div>

<div class="card" markdown="1">

### [NuGet Migration Notice](nuget.md)

Deprecation notice and migration advice for previous NuGet package consumers.

[View Migration Notice :octicons-arrow-right-24:](nuget.md)

</div>

</div>

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

### 5. SSH / Remote Command Line MSVC Setup (`init_msvc_env.bat` & `init_msvc_env.ps1`)

When building over SSH or remote non-interactive terminal sessions on Windows, `cl.exe` and Visual Studio toolchain environment variables are not loaded by default. Use the helper initialization scripts in `scripts/`:

**For Windows Command Prompt (CMD over SSH)**:
```cmd
scripts\init_msvc_env.bat [x64 | x64_arm64 | arm64]
```

**For PowerShell (over SSH)**:
```powershell
. .\scripts\init_msvc_env.ps1 -Arch x64
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
