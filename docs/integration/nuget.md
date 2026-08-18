# NuGet Package Integration

For Visual Studio projects on Windows, `sip2json` is available on NuGet.

---

## Installation via Visual Studio Package Manager

Run the following command in the **Package Manager Console**:

```powershell
Install-Package SiddiqSoft.sip2json
```

Or search for **`SiddiqSoft.sip2json`** in the NuGet Package Manager UI.

---

## MSBuild Properties

Including the `SiddiqSoft.sip2json` NuGet package automatically injects include paths into your Visual Studio C++ project settings. Ensure your project is set to `/std:c++latest` or C++23 standard in **Project Properties -> C/C++ -> Language -> C++ Language Standard**.
