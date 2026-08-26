# Building, Testing & Benchmarking

This guide covers configuring, building, testing, and benchmarking `sip2json` across all supported platforms.

---

## 1. CMake Presets Overview

`sip2json` uses standard `CMakePresets.json` (schema version 8) with CMake and Ninja to provide reproducible cross-platform builds.

### Available Configure Presets

| Platform | Preset Name | Compiler | Target Architecture | Build Type |
| :--- | :--- | :--- | :---: | :---: |
| **macOS** | `Apple-Release` *(Recommended)* | AppleClang (Xcode) | arm64 / x64 | Release |
| | `Apple-Debug` | AppleClang (Xcode) | arm64 / x64 | Debug |
| | `Apple-LLVM-Release` | Homebrew LLVM Clang | arm64 / x64 | Release |
| | `Apple-LLVM-Debug` | Homebrew LLVM Clang | arm64 / x64 | Debug |
| **Linux** | `Linux-GCC-Release` | GCC 14+ | x64 | Release |
| | `Linux-GCC-Debug` | GCC 14+ | x64 | Debug |
| | `Linux-GCC-arm64-Release` | GCC 14+ | arm64 | Release |
| | `Linux-Clang-Release` | Clang 18+ | x64 | Release |
| | `Linux-Clang-Debug` | Clang 18+ | x64 | Debug |
| | `Linux-Clang-arm64-Release` | Clang 18+ | arm64 | Release |
| **Windows** | `Windows-MSVC-Release` | MSVC v143 | x64 | Release |
| | `Windows-MSVC-Debug` | MSVC v143 | x64 | Debug |
| | `Windows-MSVC-arm64-Release` | MSVC v143 (Cross/Native) | arm64 | Release |

---

## 2. Step-by-Step Build Commands

### macOS (Apple Silicon & Intel)
```bash
# 1. Configure with Apple-Release preset
cmake --preset Apple-Release

# 2. Build library, test suites, and benchmarks
cmake --build --preset Apple-Release -j $(sysctl -n hw.ncpu)

# 3. Run all unit tests (324/324 passing)
ctest --preset Apple-Release --output-on-failure
```

### Linux (Red Hat 10.2 / Fedora / Debian)
```bash
# 1. Configure with GCC or Clang
cmake --preset Linux-GCC-Release
# OR: cmake --preset Linux-Clang-Release

# 2. Build with Ninja
cmake --build --preset Linux-GCC-Release -j $(nproc)

# 3. Execute test suite
ctest --preset Linux-GCC-Release --output-on-failure
```

### Windows (PowerShell / cmd.exe)
```powershell
# 1. Initialize MSVC environment
.\scripts\init_msvc_env.ps1 -Architecture x64

# 2. Configure with Windows-MSVC-Release preset
cmake --preset Windows-MSVC-Release

# 3. Build with Ninja
cmake --build --preset Windows-MSVC-Release -j $env:NUMBER_OF_PROCESSORS

# 4. Execute test suite
ctest --preset Windows-MSVC-Release --output-on-failure
```

---

## 3. Running Specific Test Suites

`sip2json` separates tests into distinct categories under `tests/`:

```bash
# Run only RFC 4475 Torture tests (50 test cases)
ctest --preset Apple-Release -R "rfc4475" --output-on-failure

# Run only RFC 3261 Core Compliance tests
ctest --preset Apple-Release -R "rfc3261" --output-on-failure

# Run only SDP & WebRTC compliance tests
ctest --preset Apple-Release -R "sdp" --output-on-failure

# Run security & CRLF injection tests
ctest --preset Apple-Release -R "security" --output-on-failure

# Run regression tests on real fixture streams
ctest --preset Apple-Release -R "regression" --output-on-failure
```

---

## 4. Running Performance Benchmarks Locally

`sip2json` provides two benchmark harnesses:

### 1. Dedicated Stream & Fixture Benchmark (`sip2json_benchmark`)
Evaluates multi-message stream throughput (`parseAsync`), discrete frame latency (`parseFromBuffer`), and SDP element traversal across all 36 fixture files:

```bash
./build/Apple-Release/tests/benchmark/sip2json_benchmark tests/validation/samples
```

**Sample Output:**
```text
================================================================================
  SIP2JSON Benchmark Harness
  Loaded 36 sample files (1538.42 KB total)
================================================================================

[BENCHMARK RESULTS - sip2json::parseAsync (Stream Callback)]
  Total Iterations    : 300
  Total Execution Time: 5304.82 ms (5.30 s)
  Total Messages      : 164400
  Total Data Processed: 450.71 MB
  Throughput          : 30988.47 msg/sec
  Data Bandwidth      : 84.96 MB/sec
  Avg Latency/Msg     : 32.27 us/msg

[BENCHMARK RESULTS - sip2json::parseFromBuffer (Single)]
  Valid Single Files  : 31
  Single Iterations   : 1000
  Total Execution Time: 915.13 ms
  Total Messages      : 31000
  Throughput          : 33874.63 msg/sec
  Avg Latency/Msg     : 29.52 us/msg
```

### 2. Google Benchmark Suite (`sip2json_benchmark_gbench`)
Runs micro-benchmarks with nanosecond timing and exports JSON results:

```bash
./build/Apple-Release/tests/benchmark/sip2json_benchmark_gbench \
    --benchmark_out=benchmark_results.json \
    --benchmark_out_format=json
```

---

## 5. Diagnostic & Sanitizer Builds (ASan / UBSan)

To diagnose memory safety, buffer overruns, or undefined behavior during development:

```bash
# Configure with AddressSanitizer and UndefinedBehaviorSanitizer enabled
cmake -B build/asan -S . -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -Dsip2json_BUILD_TESTS=ON \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer"

cmake --build build/asan
ctest --test-dir build/asan --output-on-failure
```
