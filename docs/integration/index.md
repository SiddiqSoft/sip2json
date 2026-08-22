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
