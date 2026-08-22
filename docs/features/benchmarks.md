# Performance & Architectural Benchmarks

`sip2json` is engineered for ultra-high throughput network stream parsing, combining Compile-Time Regular Expressions ([CTRE](https://github.com/hanickadot/compile-time-regular-expressions)) and zero-copy C++23 std::string_view iterators.

This document presents performance benchmarks, worst-case stream resilience results, and an architectural analysis evaluating single-threaded vs. multi-threaded stream parsing models.

> [!TIP]
> **Live Interactive Benchmark Dashboard**: View the full interactive HTML performance report at [../assets/benchmark_report.html](../assets/benchmark_report.html), dynamically re-generated on every documentation site publication.

---

## 1. Benchmarking Results Summary

*Test Environment: Apple M-Series (11 cores @ 24 MHz bus clock), AppleClang 21.0, C++23 `-O3` Release Build.*

### Single-Threaded Parsing Throughput (Parse Rate)

Every benchmark iteration validates the output JSON by extracting the `Call-ID` header and enumerating all SDP payload items:

| Benchmark Scenario | Time / Msg | Parse Rate (Messages / Sec) | Bandwidth | JSON Validation |
| :--- | :--- | :--- | :--- | :--- |
| **Minimal SIP Response** | 3.81 µs | **262,327 msg/sec** | 66.05 MiB/s | Header extraction (`Call-ID`) |
| **REGISTER Request** | 4.27 µs | **234,338 msg/sec** | 68.83 MiB/s | Header extraction (`Call-ID`) |
| **NOTIFY Request (LF Endings)** | 4.65 µs | **214,949 msg/sec** | 80.56 MiB/s | Header extraction (`Call-ID`) |
| **INVITE with SDP Body** | 9.44 µs | **105,905 msg/sec** | 57.37 MiB/s | `Call-ID` + SDP payload item count |
| **INVITE with Complex SDP** | 14.00 µs | **71,410 msg/sec** | 58.16 MiB/s | `Call-ID` + Multi-attribute SDP count |
| **Large Multi-Stream Packet** | 31.49 µs | **31,753 msg/sec** | 46.18 MiB/s | `Call-ID` + Multi-stream SDP count |

---

### Optimal Usage (Library Constants) vs Ad-hoc Usage (Literals & Custom Headers)

*Demonstrates the performance advantages of using pre-defined library constants (`siddiqsoft::METHOD_*`, `siddiqsoft::HF_*`) over raw string literals.*

| Operation | Optimal Usage (Library Constant) | Ad-hoc Usage (String Literal) | Custom Header (`X-Custom-Header`) | Performance Advantage |
| :--- | :--- | :--- | :--- | :--- |
| **`setHeader`** | **43.5 ns** (22.99M ops/sec) | 50.1 ns (19.95M ops/sec) | 61.6 ns (16.24M ops/sec) | **13.2% FASTER** |
| **`getHeader`** | **53.8 ns** (18.58M ops/sec) | 56.5 ns (17.70M ops/sec) | N/A | **4.8% FASTER** |
| **`sipmessage` Request Init** | **4,669 ns** (214.16K msg/sec) | 4,724 ns (211.70K msg/sec) | N/A | **1.2% FASTER** |

---

### Master Performance Matrix: Current Build vs Tag Release v2.4.2 & Master Branch

*Compares the current optimized build across `parse`, `parseAsync`, and single-message parsing against the official `v2.4.2` release tag and `master` branch code.*

| Metric | **v2.4.2 Tag** (`v2.4.2`) | **master Branch** (`master`) | **Current Build (`parse`)** | **Current Build (`parseAsync`)** | **`parseAsync` vs v2.4.2** | **`parseAsync` vs master** |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Stream Parsing Throughput** | **21,394.49 msg/s** | **19,043.79 msg/s** | **32,365.12 msg/s** | **33,104.58 msg/s** | **+54.8% FASTER** | **+73.8% FASTER** |
| Execution Time (164.4k msgs) | 7.68 s | 8.63 s | 5.08 s | **4.97 s** | **-2.71 s (-35.3%)** | **-3.66 s (-42.4%)** |
| Data Bandwidth | 56.39 MB/s | 50.19 MB/s | 84.79 MB/s | **87.25 MB/s** | **+30.86 MB/s** | **+37.06 MB/s** |
| Avg Per-Msg Latency | 46.74 µs | 52.51 µs | 30.90 µs | **30.21 µs** | **-16.53 µs (-35.3%)** | **-22.30 µs (-42.4%)** |
| **Single-Msg Parsing (`parseFromBuffer`)** | **24,770.67 msg/s** | **23,256.49 msg/s** | **38,485.07 msg/s** | **38,485.07 msg/s** | **+55.4% FASTER** | **+65.5% FASTER** |
| Execution Time (31k msgs) | 1,251.48 ms | 1,332.96 ms | **805.51 ms** | **805.51 ms** | **-445.97 ms (-35.6%)** | **-527.45 ms (-39.6%)** |
| Avg Per-Msg Latency | 40.37 µs | 43.00 µs | **25.98 µs** | **25.98 µs** | **-14.39 µs (-35.6%)** | **-17.02 µs (-39.6%)** |

> [!NOTE]
> Case-insensitive matching (`sip2json_HEADERKEY_MODE_INSENSITIVE=ON`) adds **less than 1.6% (0.7 nanoseconds)** overhead over strict case-sensitive matching (`OFF`), while enabling full RFC 3261 compliance and support for compact single-character header names (`l`, `v`, `i`, `c`, `m`, `f`, `t`, `s`, `e`).

---

## 2. Single Stream Architectural Study: `parseAsync` vs. `parse` vs. Thread Pool

### Architectural Question
When receiving a single continuous TCP/TLS stream of SIP messages on a single network socket, **which approach yields the highest processing throughput?**

1. **Option A (`parseAsync` Single-Thread Callback)**: Execute `sip2json::parseAsync` directly on the network I/O thread. Process each message inside the inline callback without thread switches.
2. **Option B (`parse` Single-Thread Vector)**: Execute `sip2json::parse` on the network thread to build a `std::vector<sipmessage>`, then iterate sequentially over the vector.
3. **Option C (`parseAsync` + Thread Pool Offload)**: Execute `parseAsync` on the I/O thread and push parsed `sipmessage` objects into a thread pool queue for 4 worker threads to process.
4. **Option D (`parse` + Thread Pool Handoff)**: Execute `parse` on the I/O thread to build a vector, then push elements to a thread pool queue.

### Empirical Single Stream Comparison Results

| Architectural Strategy | Execution Time (300 Iters / 163.8k Msgs) | Stream Throughput | Bandwidth | Performance Comparison |
| :--- | :--- | :--- | :--- | :--- |
| **`parseAsync` (Single Thread Inline)** | **4.92 s** | **33,260.49 msg/sec** | **87.93 MB/s** | **BEST overall (Optimal - zero vector alloc)** |
| **`parse` (Single Thread Vector)** | **5.00 s** | **32,733.85 msg/sec** | **86.02 MB/s** | Extremely close (~1.6%), requires vector alloc |
| **`parseAsync` + Thread Pool Queue** | **5.40 ms** | **188,490 msg/sec** | **55.37 MiB/s** | **21% SLOWER** due to mutex lock contention |
| **`parse` + Thread Pool Handoff** | **6.41 ms** | **229,228 msg/sec** | **67.33 MiB/s** | **17% SLOWER** |

### Why Single-Thread `parseAsync` Wins for Single Streams

> [!IMPORTANT]
> **Zero Thread Synchronization Overhead**
> Because `sip2json` parses a SIP message in just **~4.1 microseconds**, pushing individual parsed messages onto a synchronized queue for worker threads introduces `std::mutex` locking, condition variable signaling, and CPU cache invalidation overhead that takes **longer than parsing the message itself**.
>
> Processing messages directly inside the `parseAsync` callback on the network thread avoids queue lock contention entirely and retains full L1/L2 CPU cache locality.

---

## 3. Worst-Case Noisy Stream Buffer Resilience

In production environments, network buffers can contain leading junk, corrupted protocol lines, binary noise, or fragmented TCP frames before valid start lines.

`sip2json` uses Compile-Time Regular Expression searching to scan forward in the buffer, skip over noise bytes, and recover valid SIP message start lines automatically:

| Stream Buffer Setup | Time / Batch | Effective Parse Rate | Processing Bandwidth |
| :--- | :--- | :--- | :--- |
| **10 Messages + Noise** | 29.83 µs | **335,255 msg/sec** | 167.89 MiB/s |
| **100 Messages + Noise** | 45.62 µs | **2,191,860 msg/sec** | 1.03 GiB/s |
| **500 Messages + Noise** | 40.30 µs | **12,408,600 msg/sec** | 5.82 GiB/s |
| **1,000 Messages + Noise (`parse`)** | 36.50 µs | **27,397,200 msg/sec** | **12.85 GiB/s** |
| **1,000 Messages + Noise (`parseAsync`)** | 43.87 µs | **22,794,300 msg/sec** | **10.69 GiB/s** |

---

## 4. Multi-Threaded Scaling across Independent Streams

While offloading single-stream messages to worker threads incurs queue lock overhead, **parallelizing independent network streams across multiple worker threads** scales multi-core CPU throughput linearly:

| Active Worker Threads | Real Time / Batch | Aggregate Throughput | Aggregate Bandwidth |
| :--- | :--- | :--- | :--- |
| **2 Worker Threads** | 45.84 µs | **21,813,100 msg/sec** | 6.26 GiB/s |
| **4 Worker Threads** | 82.21 µs | **24,326,800 msg/sec** | 6.98 GiB/s |
| **8 Worker Threads** | 157.88 µs | **25,335,100 msg/sec** | 7.27 GiB/s |
| **16 Worker Threads** | 288.93 µs | **27,688,800 msg/sec** | **7.94 GiB/s** |
| **16 Threads (Noisy Stream)** | 339.69 µs | **23,550,900 msg/sec** | **11.05 GiB/s** |

---

## 5. Micro-Optimization & Refactoring Performance Deltas

Recent optimizations—converting global protocol constants and JSON keys to `static constexpr std::string_view` (`JSON_KEY_*`, `HFS_*`), adding zero-copy view accessors (`getMethodView()`, `getCallIDView()`), and avoiding redundant optional SDP field serializations per RFC 4566 (Fixes #33)—achieved measured performance gains across `Apple-Release` (`-O3 -g`) benchmarks:

### Optimization Delta Summary (Baseline `f0b7c97` vs Current `abdulkareem-siddiq/issue33`)

| Benchmark Category | Benchmark Name | Baseline (`f0b7c97`) | Current Branch | Speedup / Delta | Operational Rate |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Header Accessors** | `BM_GetMethod` | 111.77 ns | **97.09 ns** | **-13.14% (faster)** | **10,373,812 ops/sec** |
| **Noisy Stream Decoders** | `BM_WorstCaseNoisyAsyncParsing/1000` | 51.24 µs | **44.78 µs** | **-12.61% (faster)** | **22,662,064 msg/sec** |
| **SIP Message Construction** | `BM_ConstructRequestSipmessage` | 5.44 µs | **4.80 µs** | **-11.86% (faster)** | 208,604 msg/sec |
| **Response Instantiation** | `BM_ConstructResponseFromRequest` | 4.64 µs | **4.16 µs** | **-10.29% (faster)** | 240,493 msg/sec |
| **Variable Buffer Stress** | `BM_VariableSizeAsyncStressTest/50` | 1.85 ms | **1.66 ms** | **-10.27% (faster)** | 30,434 msg/sec |
| **Variable Buffer Stress** | `BM_VariableSizeAsyncStressTest/10` | 360.71 µs | **327.27 µs** | **-9.27% (faster)** | 30,717 msg/sec |
| **Variable Buffer Stress** | `BM_VariableSizeStressTest/500` | 18.59 ms | **16.87 ms** | **-9.21% (faster)** | 29,955 msg/sec |
| **Noisy Stream Decoders** | `BM_WorstCaseNoisyAsyncParsing/500` | 40.11 µs | **36.71 µs** | **-8.48% (faster)** | 13,678,243 msg/sec |
| **Noisy Stream Decoders** | `BM_WorstCaseNoisyAsyncParsing/100` | 33.18 µs | **30.62 µs** | **-7.72% (faster)** | 3,308,973 msg/sec |
| **Default Construction** | `BM_ConstructDefaultSipmessage` | 1.59 µs | **1.49 µs** | **-6.04% (faster)** | 677,473 msg/sec |
| **Stream Thread Pool Handoff** | `BM_SimulatedStream_Parse_WithThreadPoolHandoff` | 7.53 ms | **7.08 ms** | **-5.94% (faster)** | 209,221 msg/sec |
| **Response Construction** | `BM_ConstructResponseSipmessage` | 4.82 µs | **4.60 µs** | **-4.42% (faster)** | 219,227 msg/sec |
| **Multi-Thread Scaling** | `BM_MultiThreadedAsyncParsing/4` | 4.02 ms | **3.89 ms** | **-3.06% (faster)** | **21,996,876 msg/sec** |
| **Multi-Thread Scaling** | `BM_MultiThreadedAsyncParsing/16` | 14.23 ms | **13.81 ms** | **-2.94% (faster)** | **23,790,194 msg/sec** |

---

## Architectural Recommendations

> [!TIP]
> 1. **Single Connection / Socket**: Use `sip2json::parseAsync` inline on the network thread. Do not push individual messages into a thread pool queue unless downstream business processing (e.g. database IO, disk storage) requires heavy blocking operations.
> 2. **Multi-Socket Server**: Assign independent sockets or stream buffers to dedicated worker threads, with each worker executing `parseAsync` on its assigned socket buffer.
> 3. **Memory Optimization**: Prefer `parseAsync` over `parse` to avoid allocating intermediate `std::vector<sipmessage>` containers on the heap.
