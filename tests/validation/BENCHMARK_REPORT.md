# sip2json Performance Benchmark Report

This document presents a performance benchmark analysis comparing versions of the [`siddiqsoft/sip2json`](https://github.com/siddiqsoftware/sip2json) C++ library (**v2.4.2**, **v2.5.7**, **v2.5.8**, and **current local workspace**) against real-world anonymized SIP message fixtures under isolated, single-threaded execution conditions.

---

## 1. Executive Summary

- **Isolated Execution**: All benchmark suites were executed sequentially in total isolation (one version at a time) to eliminate process scheduling and CPU contention artifacts.
- **Peak Performance in v2.4.2**: **v2.4.2** achieved the highest overall parsing throughput (**1,376.18 msg/sec** stream / **1,784.98 msg/sec** single) with the lowest per-message latencies (**726.65 µs** stream / **560.23 µs** single).
- **v2.5.7, v2.5.8, and Local Workspace Consistency**: Performance across **v2.5.7**, **v2.5.8**, and the **current local workspace** remains consistent within ~1,200–1,242 msg/sec for multi-message stream parsing and ~1,600–1,625 msg/sec for single-message parsing. Additional input validation, bounds checks, and safety logic introduced in the 2.5.x releases account for the minor throughput difference relative to 2.4.2.

---

## 2. Comparative Benchmark Matrix

The benchmark suite evaluated each library version using 34 anonymized real-world SIP message fixtures loaded into memory.

### Stream Parsing (`sip2json::parse`) — 163,800 Total Messages Parsed
*Parses multi-message stream buffers across all 34 sample files over 300 iterations.*

| Metric | **v2.4.2** | **v2.5.7** | **v2.5.8** | **current** *(Local Workspace)* |
| :--- | :---: | :---: | :---: | :---: |
| **Total Execution Time** | **119.02 s** | 131.83 s | 135.68 s | 132.93 s |
| **Throughput (msg/sec)** | **1,376.18 msg/sec** | 1,242.48 msg/sec | 1,207.23 msg/sec | 1,232.24 msg/sec |
| **Data Bandwidth** | **3.62 MB/sec** | 3.26 MB/sec | 3.17 MB/sec | 3.24 MB/sec |
| **Avg Per-Msg Latency** | **726.65 µs** | 804.84 µs | 828.34 µs | 811.53 µs |

---

### Single-Message Parsing (`sip2json::parseFromBuffer`) — 29,000 Total Messages Parsed
*Parses discrete single SIP message buffers across 29 valid sample files over 1,000 iterations.*

| Metric | **v2.4.2** | **v2.5.7** | **v2.5.8** | **current** *(Local Workspace)* |
| :--- | :---: | :---: | :---: | :---: |
| **Total Execution Time** | **16.25 s** | 17.84 s | 18.01 s | 18.13 s |
| **Throughput (msg/sec)** | **1,784.98 msg/sec** | 1,625.20 msg/sec | 1,609.97 msg/sec | 1,599.24 msg/sec |
| **Avg Per-Msg Latency** | **560.23 µs** | 615.31 µs | 621.13 µs | 625.30 µs |

---

## 3. Methodology & Test Environment

- **OS**: macOS (Apple Silicon ARM64)
- **Compiler**: Clang / LLVM (`-std=c++23`, `-O3` / Release optimization)
- **Test Fixtures**: 34 anonymized `.sip` files representing real-world SIP traffic (`REGISTER`, `INVITE`, `NOTIFY`, multi-part SDP, and multi-message streams).
- **Harness Implementation**: C++ `std::chrono::high_resolution_clock` measuring in-memory buffer parsing iterations.
- **Isolation Constraint**: Benchmarks executed strictly sequentially in dedicated single-threaded processes to prevent thread scheduling noise and core contention.

---

## 4. How to Reproduce

Configure and build the benchmark executable for any targeted version:

```bash
# Benchmark a specific version (e.g. 2.4.2, 2.5.7, 2.5.8, or 'current')
cmake -S tests/validation -B tests/validation/build -DSIP2JSON_VERSION=2.4.2
cmake --build tests/validation/build --target sip2json_benchmark
./tests/validation/build/sip2json_benchmark 2>/dev/null
```
