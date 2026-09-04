# NuGet Package

Package: [`SiddiqSoft.sip2json`](https://www.nuget.org/packages/SiddiqSoft.sip2json) (header-only, native C++20).

---

## Installation

=== "Package Manager"

    ```powershell
    Install-Package SiddiqSoft.sip2json
    ```

=== "MSBuild (.vcxproj)"

    ```xml
    <ItemGroup>
      <PackageReference Include="SiddiqSoft.sip2json" Version="3.1.0" />
    </ItemGroup>
    ```

---

## Configuration

1. Set **C++ Language Standard** to **C++20 (`/std:c++20`)**.
2. Dependency [`nlohmann.json`](https://www.nuget.org/packages/nlohmann.json) (v3.12+) is resolved automatically.
3. Includes `siddiqsoft.sip2json.natvis` for Visual Studio debugger inspection.

---

## Example

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

int main() {
    std::string raw = "INVITE sip:alice@atlanta.com SIP/2.0\r\nContent-Length: 0\r\n\r\n";
    auto msg = siddiqsoft::sip2json::parse(raw);
    std::cout << msg.getMethod() << " " << msg.getUri() << "\n";
    return 0;
}
```
