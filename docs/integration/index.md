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
