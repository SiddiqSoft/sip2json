# Multi-Platform Developer Environment Setup

This guide details how to configure a pristine development workstation for `sip2json` on **macOS**, **Linux**, and **Windows**.

> [!IMPORTANT]
> **Development System Requirements (`libcurl` & `OpenSSL` / `libopenssl`)**:
> In addition to the C++23 compiler, build generator, and Python, development environments require **`libcurl`** and **`OpenSSL`** (`libopenssl`) development libraries and header packages installed for tooling, security test suites, and client network validation.

---

## 1. macOS Setup (Apple Silicon & Intel)

macOS is the primary local development environment for `sip2json`. Both native Apple Clang (Xcode) and Homebrew LLVM Clang toolchains are supported.

### Prerequisites & Homebrew Tools
```bash
# 1. Install Xcode Command Line Tools
xcode-select --install

# 2. Install build generators, LLVM, libcurl, OpenSSL dev libraries, and pkg-config
brew update
brew install cmake ninja llvm git python3 openssl@3 curl pkg-config

# 3. Verify toolchain versions
cmake --version    # Requires >= 3.29
ninja --version    # Requires >= 1.11
clang --version    # Apple Clang 15+ or Homebrew LLVM Clang 18+
```

### Environment Variables (Homebrew OpenSSL & libcurl Integration)
Because Homebrew installs `openssl@3` and `curl` as keg-only formulas to avoid conflicting with built-in macOS system binaries, configure your shell environment:

```bash
# Set Homebrew prefix (Apple Silicon: /opt/homebrew, Intel: /usr/local)
BREW_PREFIX="$(brew --prefix)"

# Add LLVM, curl, and OpenSSL binaries to PATH
export PATH="$BREW_PREFIX/opt/llvm/bin:$BREW_PREFIX/opt/curl/bin:$BREW_PREFIX/opt/openssl@3/bin:$PATH"

# Linker & Compiler flags for OpenSSL and libcurl
export LDFLAGS="-L$BREW_PREFIX/opt/llvm/lib -L$BREW_PREFIX/opt/openssl@3/lib -L$BREW_PREFIX/opt/curl/lib $LDFLAGS"
export CPPFLAGS="-I$BREW_PREFIX/opt/llvm/include -I$BREW_PREFIX/opt/openssl@3/include -I$BREW_PREFIX/opt/curl/include $CPPFLAGS"

# CMake & pkg-config discovery paths
export PKG_CONFIG_PATH="$BREW_PREFIX/opt/openssl@3/lib/pkgconfig:$BREW_PREFIX/opt/curl/lib/pkgconfig:$PKG_CONFIG_PATH"
export CMAKE_PREFIX_PATH="$BREW_PREFIX/opt/openssl@3;$BREW_PREFIX/opt/curl;$CMAKE_PREFIX_PATH"
```

---

## 2. Linux Setup (Red Hat Enterprise Linux 10.2 / Fedora / Debian)

`sip2json` requires a C++23 compliant compiler (`GCC 14+` or `Clang 18+`), CMake 3.31+, Ninja, and **`libcurl`** / **`OpenSSL`** development packages.

### Red Hat Enterprise Linux 10.2 / Rocky Linux 10 / AlmaLinux 10
```bash
# 1. Enable EPEL and CodeReady Linux Builder (CRB) repositories
sudo dnf install -y epel-release
sudo dnf config-manager --set-enabled crb || sudo dnf config-manager --set-enabled powertools

# 2. Install Development Tools, GCC 14+, Clang 18+, CMake, Ninja, Python, libcurl, and OpenSSL
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y \
    gcc-c++ \
    clang \
    clang-tools-extra \
    lld \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    libcurl-devel \
    openssl-devel \
    tar \
    curl

# 3. Verify toolchain versions
g++ --version      # Requires >= GCC 14
clang++ --version  # Requires >= Clang 18
cmake --version    # Requires >= 3.29
ninja --version    # Requires >= 1.11
```

### Fedora (40+)
```bash
# 1. Update package index and install toolchain, libcurl, and OpenSSL
sudo dnf update -y
sudo dnf install -y \
    gcc-c++ \
    clang \
    clang-tools-extra \
    lld \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    python3-devel \
    libcurl-devel \
    openssl-devel \
    tar \
    curl

# 2. Verify toolchain versions
g++ --version
clang++ --version
cmake --version
```

### Debian 12 (Bookworm) & Debian 13 (Trixie)
```bash
# 1. Update package lists and install base utilities, libcurl, and OpenSSL
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-venv \
    python3-pip \
    libcurl4-openssl-dev \
    libssl-dev \
    curl \
    tar

# 2. Install Clang 18+ via official LLVM installer script
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18
sudo apt-get install -y clang-18 clang-tools-18 lld-18

# 3. Set default compiler alternatives (Optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
                        --slave /usr/bin/clang++ clang++ /usr/bin/clang++-18
```

---

## 3. Windows Setup (MSVC 2022 & Long Paths)

Windows compilation requires **Visual Studio 2022** (MSVC toolset `v143` or later) with C++23 standard support.

### Step 1: Visual Studio 2022 Components
Install Visual Studio 2022 (Community, Professional, or Enterprise) with the **Desktop development with C++** workload and ensure the following individual components are selected:
- **MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)**
- **MSVC v143 - VS 2022 C++ ARM64/ARM64EC build tools (Latest)** *(required for ARM64 cross-builds)*
- **C++ CMake tools for Windows**
- **Windows 11 SDK** (or Windows 10 SDK 10.0.19041+)
- **Git for Windows**

*(Optional)* If installing `libcurl` and `OpenSSL` via Microsoft `vcpkg`:
```powershell
vcpkg install curl openssl:x64-windows
```

### Step 2: Automated Machine Setup Script (`prep_windows_machine.ps1`)
Because `sip2json` and **CTRE (Compile-Time Regular Expressions)** generate deeply nested template and cache directories, you **MUST** enable Windows Long Path support to avoid `MAX_PATH` (260 characters) compilation and cache errors.

Open an **Administrator PowerShell** session and run:
```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\prep_windows_machine.ps1
```

This script automatically executes:
1. `Set-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1`
2. `git config --system core.longpaths true`
3. `git config --global core.longpaths true`
4. Creates global CPM cache directory `C:\cpmcache`

### Step 3: Initializing MSVC Developer Environment
When running from terminal or PowerShell, you must initialize MSVC compiler environment variables (`cl.exe`, `link.exe`, `INCLUDE`, `LIB`):

=== "PowerShell"
    ```powershell
    # Automatically locate and source VS 2022 environment (x64 or arm64)
    .\scripts\init_msvc_env.ps1 -Architecture x64
    ```

=== "Command Prompt (cmd.exe)"
    ```cmd
    :: Initialize x64 developer environment
    scripts\init_msvc_env.bat x64
    ```

=== "Visual Studio IDE"
    Launch Visual Studio 2022, choose **Open a Local Folder**, and select the `sip2json` root directory. Visual Studio will automatically detect `CMakePresets.json` and configure the project.

---

## Compiler Flags & Template Limits for Maintainers

Because `sip2json` leverages compile-time regular expressions via CTRE and 64-bit FNV-1a constexpr hash matching, specific compiler flags are defined in `CMakeLists.txt`:

| Compiler | Optimization & Limit Flags | Rationale |
| :--- | :--- | :--- |
| **MSVC** | `/std:c++latest`, `/constexpr:depth4096`, `/constexpr:steps2000000`, `/utf-8`, `/bigobj` | Prevents MSVC `error C2999` and `error C1061` during deeply recursive CTRE template instantiation. |
| **Clang** | `-std=c++23`, `-fconstexpr-depth=4096`, `-fconstexpr-steps=20000000` | Ensures maximum compile-time constexpr recursion budget. |
| **GCC** | `-std=c++23`, `-fconstexpr-depth=4096`, `-fconstexpr-loop-limit=20000000` | Ensures constexpr recursion depth parity with Clang/MSVC. |
