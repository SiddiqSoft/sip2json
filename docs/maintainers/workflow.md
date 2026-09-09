# Local Development & Workflow

Step-by-step maintainer workflow for local building, testing, standalone validation subprojects, and macOS toolchain management.

---

## 1. Configure, Build, and Test

Maintainers can build and execute the full test suite (320+ unit and compliance tests) with standard CMake commands:

```bash
# Configure with desired preset (e.g. Apple-Clang-Release, Linux-GCC-Release, Windows-x64-Release)
cmake --preset <preset-name>

# Build all targets
cmake --build --preset <preset-name>

# Execute test suite (use -j 4 or higher for fast parallel execution)
ctest --preset <preset-name> -j 4 --output-on-failure
```

> [!TIP]
> **Test Execution Parallelism**: Specifying `-j <num_workers>` (e.g. `-j 4`) with `ctest` runs tests efficiently across worker threads without hitting operating system process limits.

---

## 2. Standalone Validation Subproject Testing

To test `sip2json` against historical versions or external CPM consumers, use the subproject located in `tests/validation/`:

```bash
# Configure standalone validation client
cd tests/validation
cmake --preset Apple-Clang-Release

# Build and run client test suite
cmake --build --preset Apple-Clang-Release
ctest --preset Apple-Clang-Release -j 4
```

---

## 3. macOS Toolchain & xcode-select Coexistence

> [!TIP]
> **Zero-Flipping Workflow**: Developing with Xcode and CLI Concurrently
> Maintainers frequently develop native macOS/iOS applications in Xcode IDE while simultaneously working on `sip2json` via terminal, VS Code, or CLion. You can leave `xcode-select` permanently set to `/Applications/Xcode.app/Contents/Developer` without encountering CLI build failures or having to toggle `xcode-select`.

### The Problem: xcrun, xcodebuild, and Exit Code 65280

On macOS, running generic compiler shims such as `/usr/bin/c++`, `/usr/bin/clang++`, or `/usr/bin/make` triggers Apple's `/usr/bin/xcrun` dispatcher. When `xcode-select` points to `/Applications/Xcode.app`, `xcrun` invokes:

```bash
xcodebuild -sdk macosx -find clang++
```

If there is a version mismatch between macOS system frameworks and installed Xcode system resources (for example, a dyld missing symbol error such as `Symbol not found: _XPCTypeBool` in `Mercury.framework` referenced by `/Library/Developer/PrivateFrameworks/CoreDevice.framework`), `xcodebuild` crashes with exit code 255 (which the shell surfaces as `c++: error: ... failed with exit code 65280`).

Historically, developers were forced to repeatedly flip `xcode-select`:

```bash
# Flipping to Command Line Tools for CLI CMake builds:
sudo xcode-select -s /Library/Developer/CommandLineTools

# Flipping back to Xcode for GUI/iOS/simulator development:
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
```

---

### How sip2json Eliminates Toolchain Flipping

`sip2json` handles toolchain resolution automatically through two complementary mechanisms:

1. **Direct Compiler Paths in Presets**:
   The `Apple-Clang-Debug` and `Apple-Clang-Release` presets in `CMakePresets.json` explicitly configure `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` to direct binary paths (e.g. `/Library/Developer/CommandLineTools/usr/bin/clang++`). Because the binary path is fully qualified, CMake and Ninja invoke the compiler directly, completely bypassing `/usr/bin/xcrun` and `xcodebuild`.

2. **Automatic Toolchain Discovery in `CMakeLists.txt`**:
   If you configure CMake outside presets (e.g. `cmake -B build` or using IDE plugins), `CMakeLists.txt` inspects candidate toolchains before `project()`:
   * Standalone Command Line Tools (`/Library/Developer/CommandLineTools/usr/bin/clang++`)
   * Xcode Default Toolchain (`/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++`)
   * Homebrew LLVM (`/opt/homebrew/opt/llvm/bin/clang++`)

   If a developer has only Xcode installed (without standalone CLT), CMake falls back to the internal Xcode toolchain binary automatically.

---

### Caveats & Best Practices

* **Environment Override with `DEVELOPER_DIR`**: If you need to temporarily direct Apple toolchains to a specific location for a single terminal session without modifying system-wide settings with `sudo xcode-select`, export `DEVELOPER_DIR`:
  ```bash
  export DEVELOPER_DIR=/Library/Developer/CommandLineTools
  # or
  export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer
  ```
* **Xcode Generator vs. Ninja**: The repository presets standardly use the `Ninja` generator. If you explicitly generate an Xcode IDE project (`cmake -G Xcode`), CMake must interact directly with `xcodebuild`. Ensure that your installed Xcode version matches your macOS version if using the Xcode generator. For command-line builds, CI, and test execution, Ninja with AppleClang is recommended.
* **Homebrew LLVM**: If you prefer building with upstream Clang from Homebrew (`brew install llvm`), you can override the compiler by setting `-DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++`.

---

## Related Topics

* [**Maintainer Guide**](maintainer_guide.md): Core maintainer guide and bulk clang-formatting
* [**CMake Presets**](cmake_presets.md): Presets hierarchy and configure reference
* [**CI/CD Pipelines**](pipelines.md): Automated CI build matrix
