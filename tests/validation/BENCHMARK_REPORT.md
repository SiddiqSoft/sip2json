# sip2json Performance Benchmark Report

This document presents a comprehensive performance benchmark analysis comparing multiple versions of the [`siddiqsoft/sip2json`](https://github.com/siddiqsoftware/sip2json) C++ library (**v1.17.1**, **v2.4.0**, **v2.4.2**, and **v2.5.7** in both case-sensitive and case-insensitive header modes) against real-world anonymized SIP message fixtures.

---

## 1. Executive Summary

- **Major Speedup in v2.x**: The transition from `v1.17.1` to `v2.x` delivered a **>5.3x throughput increase** (from 271.74 msg/sec to 1,445.38 msg/sec) by replacing runtime `std::regex` with Compile-Time Regular Expressions (CTRE) and stream parsing optimizations.
- **Peak Performance in v2.4.2**: **v2.4.2** achieved the highest overall stream parsing throughput (**1,445.38 msg/sec**), closely followed by **v2.4.0** (**1,421.78 msg/sec**).
- **v2.5.7 Performance Overhead**: **v2.5.7** introduces a ~11% latency overhead compared to v2.4.2 (1,278.38 msg/sec vs. 1,445.38 msg/sec).
- **Header Key Insensitivity Impact**: Toggling `sip2json_HEADERKEY_MODE_INSENSITIVE` to `OFF` in v2.5.7 yields only a minor throughput gain of **+0.1% to +0.7%** (1,279.49 msg/sec vs 1,278.38 msg/sec), confirming that the v2.5.7 performance delta is driven by internal parsing/JSON structure refinements rather than case-insensitivity processing.

---

## 2. Comparative Benchmark Matrix

The benchmark suite evaluated each library version using 34 anonymized real-world SIP message fixtures loaded into memory.

### Stream Parsing (`sip2json::parse`) — 163,800 Total Messages Parsed
*Parses multi-message stream buffers across all 34 sample files over 300 iterations.*

| Metric | **v1.17.1** | **v2.4.0** | **v2.4.2** | **v2.5.7** (`INSENSITIVE=OFF`) | **v2.5.7** (`INSENSITIVE=ON` / Default) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Total Execution Time** | 602.78 s | 115.21 s | **113.33 s** | 128.02 s | 128.13 s |
| **Throughput (msg/sec)** | 271.74 msg/sec | 1,421.78 msg/sec | **1,445.38 msg/sec** | 1,279.49 msg/sec | 1,278.38 msg/sec |
| **Data Bandwidth** | 0.71 MB/sec | 3.74 MB/sec | **3.80 MB/sec** | 3.36 MB/sec | 3.36 MB/sec |
| **Avg Per-Msg Latency** | 3,679.96 µs | 703.35 µs | **691.86 µs** | 781.56 µs | 782.24 µs |
| **Speedup vs. v1.17.1** | 1.0x (Baseline) | **5.23x faster** | **5.32x faster** | **4.71x faster** | **4.70x faster** |

---

### Single-Message Parsing (`sip2json::parseFromBuffer`) — 29,000 Total Messages Parsed
*Parses discrete single SIP message buffers across 29 valid sample files over 1,000 iterations.*

| Metric | **v1.17.1** | **v2.4.0** | **v2.4.2** | **v2.5.7** (`INSENSITIVE=OFF`) | **v2.5.7** (`INSENSITIVE=ON` / Default) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Total Execution Time** | 63.68 s | 15.88 s | **15.97 s** | 17.38 s | 17.50 s |
| **Throughput (msg/sec)** | 455.44 msg/sec | **1,826.31 msg/sec** | 1,815.40 msg/sec | 1,668.37 msg/sec | 1,656.94 msg/sec |
| **Avg Per-Msg Latency** | 2,195.70 µs | **547.55 µs** | 550.84 µs | 599.39 µs | 603.52 µs |
| **Speedup vs. v1.17.1** | 1.0x (Baseline) | **4.01x faster** | **3.99x faster** | **3.66x faster** | **3.64x faster** |

---

## 3. Methodology & Test Environment

- **OS**: macOS (Apple Silicon ARM64)
- **Compiler**: Clang / LLVM (`-std=c++23`, `-O3` / Release optimization)
- **Test Fixtures**: 34 anonymized `.sip` files representing real-world SIP traffic (`REGISTER`, `INVITE`, `NOTIFY`, multi-part SDP, and multi-message streams).
- **Harness Implementation**: C++ `std::chrono::high_resolution_clock` measuring in-memory buffer parsing iterations.

---

## 4. How to Reproduce

Run the benchmark executable using the build target:

```bash
cmake --preset Apple-Debug
cmake --build --preset Apple-Debug --target sip2json_benchmark
./build/Apple-Debug/sip2json_benchmark samples
```
