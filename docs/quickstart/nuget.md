# NuGet Package Integration

`SiddiqSoft.sip2json` is distributed on [nuget.org](https://www.nuget.org/packages/SiddiqSoft.sip2json) as a header-only native C++ package for Visual Studio 2022+ and MSBuild projects.

[![NuGet Version](https://img.shields.io/nuget/v/siddiqsoft.sip2json)](https://www.nuget.org/packages/siddiqsoft.sip2json)
[![NuGet Downloads](https://img.shields.io/nuget/dt/siddiqsoft.sip2json)](https://www.nuget.org/packages/siddiqsoft.sip2json)

---

## Installation

### 1. Package Manager Console

In Visual Studio, open **Tools > NuGet Package Manager > Package Manager Console** and execute:

```powershell
Install-Package SiddiqSoft.sip2json
```

### 2. Visual Studio NuGet Package Manager UI

1. Right-click your C++ project in **Solution Explorer** and select **Manage NuGet Packages...**
2. In the **Browse** tab, search for `SiddiqSoft.sip2json`.
3. Select the package and click **Install**.

### 3. MSBuild Project File (`.vcxproj`)

Alternatively, add the package reference directly inside an `<ItemGroup>` in your `.vcxproj`:

```xml
<ItemGroup>
  <PackageReference Include="SiddiqSoft.sip2json" Version="3.0.0" />
</ItemGroup>
```

---

## Project Configuration & Prerequisites

Because `sip2json` is a modern header-only library built for C++23, ensure your project properties are configured:

1. **C++ Language Standard**:
   - Set **Project Properties > Configuration Properties > C/C++ > Language > C++ Language Standard** to **Preview - Features from the Latest C++ Working Draft (`/std:c++latest`)** or **ISO C++23 Standard (`/std:c++23`)**.
2. **Dependencies**:
   - NuGet automatically resolves and links the transitive [`nlohmann.json`](https://www.nuget.org/packages/nlohmann.json) (v3.12+) package.
3. **Debugger Visualization (`.natvis`)**:
   - The package automatically includes [`siddiqsoft.sip2json.natvis`](https://github.com/siddiqsoftware/sip2json/blob/master/siddiqsoft.sip2json.natvis) for interactive debugger display of SIP messages in Visual Studio.

---

## Quick Example

Once installed, include the primary header and parse SIP messages:

```cpp
#include <iostream>
#include "siddiqsoft/sip2json.hpp"

int main()
{
    std::string raw =
        "INVITE sip:alice@atlanta.example.com SIP/2.0\r\n"
        "Via: SIP/2.0/UDP pc33.atlanta.example.com;branch=z9hG4bK776asdhds\r\n"
        "Max-Forwards: 70\r\n"
        "To: Bob <sip:bob@biloxi.example.com>\r\n"
        "From: Alice <sip:alice@atlanta.example.com>;tag=1928301774\r\n"
        "Call-ID: a84b4c76e66710@pc33.atlanta.example.com\r\n"
        "CSeq: 314159 INVITE\r\n"
        "Contact: <sip:alice@pc33.atlanta.example.com>\r\n"
        "Content-Length: 0\r\n\r\n";

    auto bs = raw.begin();
    siddiqsoft::sipmessage msg = siddiqsoft::sip2json::parseFromBuffer(bs, raw.end());

    std::cout << "Method: " << msg.getMethod() << std::endl;
    std::cout << "JSON:   " << msg.dump(2) << std::endl;
    return 0;
}
```
