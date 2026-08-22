# sip2json Performance Benchmark Report

This document presents a performance benchmark analysis comparing versions of the [`siddiqsoft/sip2json`](https://github.com/siddiqsoftware/sip2json) C++ library (**v2.4.2**, **v2.5.7**, **v2.5.8**, and **v2.6.0 Release**) against real-world anonymized SIP message fixtures under isolated, single-threaded execution conditions.

---

## 1. Executive Summary

- **Isolated Execution**: All benchmark suites were executed sequentially in total isolation (one version at a time) to eliminate process scheduling and CPU contention artifacts.
- **v2.6.0 Architectural Breakthrough**: **v2.6.0** achieves unprecedented parsing throughput (**17,448 msg/sec** stream / **19,738 msg/sec** single) — delivering a **12.7x speedup** over v2.4.2 for stream parsing and an **11.1x speedup** for single-message parsing.
- **Latency Reduction**: Per-message average latencies were reduced from **726.65 µs** down to **57.31 µs** (stream) and from **560.23 µs** down to **50.66 µs** (single).
- **Optimization Drivers**: Zero-copy header key canonicalization via `HeaderKeySet`, fast-path line parsing using string views, memory allocation reuse, and `std::format` serialization enhancements.

---

## 2. Comparative Benchmark Matrix

The benchmark suite evaluated each library version using 34 anonymized real-world SIP message fixtures loaded into memory.

### Stream Parsing (`sip2json::parse`) — 163,800 Total Messages Parsed
*Parses multi-message stream buffers across all 34 sample files over 300 iterations.*

| Metric | **v2.4.2** | **v2.5.7** | **v2.5.8** | **v2.6.0 (Current)** | **v2.6.0 vs v2.4.2 Improvement** |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Total Execution Time** | 119.02 s | 131.83 s | 135.68 s | **9.39 s** | **12.7x faster** |
| **Throughput (msg/sec)** | 1,376.18 msg/sec | 1,242.48 msg/sec | 1,207.23 msg/sec | **17,448.26 msg/sec** | **+1,168% (+16,072 msg/sec)** |
| **Data Bandwidth** | 3.62 MB/sec | 3.26 MB/sec | 3.17 MB/sec | **45.85 MB/sec** | **12.7x higher** |
| **Avg Per-Msg Latency** | 726.65 µs | 804.84 µs | 828.34 µs | **57.31 µs** | **12.7x reduction** |

---

### Single-Message Parsing (`sip2json::parseFromBuffer`) — 29,000 Total Messages Parsed
*Parses discrete single SIP message buffers across 29 valid sample files over 1,000 iterations.*

| Metric | **v2.4.2** | **v2.5.7** | **v2.5.8** | **v2.6.0 (Current)** | **v2.6.0 vs v2.4.2 Improvement** |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Total Execution Time** | 16.25 s | 17.84 s | 18.01 s | **1.47 s** | **11.1x faster** |
| **Throughput (msg/sec)** | 1,784.98 msg/sec | 1,625.20 msg/sec | 1,609.97 msg/sec | **19,737.79 msg/sec** | **+1,006% (+17,953 msg/sec)** |
| **Avg Per-Msg Latency** | 560.23 µs | 615.31 µs | 621.13 µs | **50.66 µs** | **11.1x reduction** |

---

## 3. Stream Inspection & SDP Element Counts

*Detailed header and SDP element inspection across multi-message stream fixtures in v2.6.0:*

| File | Messages Received | Total SDP Elements | Avg SDP Elements/Msg | `X-domain` Headers | `X-Seamless` Headers | `X-Call-Instance-ID` | SDP `a=x-voice-callowner-login_alias` |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **`Mixed_Stream_1.sip`** | 18 | 853 | **47.39** | 18 | 0 | 16 | 22 |
| **`Mixed_Stream_2.sip`** | 9 | 349 | **38.78** | 9 | 0 | 9 | 9 |
| **`Mixed_Stream_3.sip`** | 21 | 739 | **35.19** | 21 | 0 | 21 | 8 |
| **`RandomStream_Recv_File_1.sip`** | 459 | 21,409 | **46.64** | 459 | 34 | 344 | 549 |

---

## 4. Methodology & Test Environment

- **OS**: macOS (Apple Silicon ARM64)
- **Compiler**: Clang / LLVM (`-std=c++23`, `-O3` / Release optimization)
- **Test Fixtures**: 34 anonymized `.sip` files representing real-world SIP traffic (`REGISTER`, `INVITE`, `NOTIFY`, multi-part SDP, and multi-message streams).
- **Harness Implementation**: C++ `std::chrono::high_resolution_clock` measuring in-memory buffer parsing iterations.
- **Isolation Constraint**: Benchmarks executed strictly sequentially in dedicated single-threaded processes to prevent thread scheduling noise and core contention.

---

## 5. How to Reproduce

Configure and build the benchmark executable:

```bash
# Build & run benchmark suite with Apple-Release preset
cmake --preset Apple-Release
cmake --build --preset Apple-Release --target sip2json_benchmark
./build/Apple-Release/tests/validation/sip2json_benchmark
```
