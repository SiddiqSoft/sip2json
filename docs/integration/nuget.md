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

---

> [!TIP]
> **Windows Setup Note**: To avoid `filename too long` errors during CTRE header checkout on Windows, run [`scripts/prep_windows_machine.ps1`](https://github.com/SiddiqSoft/sip2json/blob/master/scripts/prep_windows_machine.ps1) as Administrator to enable Windows Registry `LongPathsEnabled = 1` and Git `core.longpaths = true`.
