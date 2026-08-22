# Performance Benchmark Report

Official benchmark results for `siddiqsoft/sip2json` comparing performance across releases: `v2.4.2` tag, `master` branch, and `release/2.6.0` (with 64-bit packed switch matching and merged `HeaderKeySet` zero-copy architecture).

---

## 1. Key Performance Highlights

- **Stream Throughput (`parseAsync`)**: Increased from **21,364.24 msg/sec** (`v2.4.2`) and **19,227.57 msg/sec** (`master`) to **35,541.38 msg/sec** (**+66.4% faster than v2.4.2** and **+84.8% faster than master**).
- **Single Message Throughput (`parseFromBuffer`)**: Increased from **24,770.67 msg/sec** (`v2.4.2`) and **23,256.49 msg/sec** (`master`) to **41,527.70 msg/sec** (**+67.6% faster than v2.4.2** and **+78.6% faster than master**).
- **Latency Reduction**: Average per-message latency dropped from **46.74 µs/msg** (`v2.4.2`) and **52.51 µs/msg** (`master`) down to **28.14 µs/msg** (`parseAsync`) and **24.08 µs/msg** (`parseFromBuffer`).
- **Data Bandwidth**: Stream parsing bandwidth reached **93.67 MB/sec** on single-core network execution.
- **100% Standards & Regression Pass**: All 38 compliance tests and 78 validation/regression tests passed cleanly.

---

## 2. Comparative Benchmark Matrix

*Evaluated across 36 real-world SIP message stream fixtures (164,400 total stream messages per run and 31,000 single message iterations).*

| Metric | **v2.4.2 Tag** (`v2.4.2`) | **master Branch** (`master`) | **Current Build (`parse`)** | **Current Build (`parseAsync`)** | **`parseAsync` vs v2.4.2** | **`parseAsync` vs master** |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Stream Parsing** | | | | | | |
| **Throughput (msg/sec)** | **21,394.49** | **19,043.79** | **34,653.48** | **35,541.38** | **+66.1% FASTER** | **+86.6% FASTER** |
| Execution Time (300 iters / 164.4k msgs) | 7.68 s | 8.63 s | 4.74 s | **4.63 s** | **-3.05 s (-39.7%)** | **-4.00 s (-46.3%)** |
| Data Bandwidth | 56.39 MB/s | 50.19 MB/s | 90.78 MB/s | **93.67 MB/s** | **+37.28 MB/s** | **+43.48 MB/s** |
| Avg Per-Msg Latency | 46.74 µs | 52.51 µs | 28.86 µs | **28.14 µs** | **-18.60 µs (-39.7%)** | **-24.37 µs (-46.3%)** |
| **Single-Msg Parsing (`parseFromBuffer`)** | | | | | | |
| **Throughput (msg/sec)** | **24,770.67** | **23,256.49** | **41,527.70** | **41,527.70** | **+67.6% FASTER** | **+78.6% FASTER** |
| Execution Time (1k iters / 31k msgs) | 1,251.48 ms | 1,332.96 ms | **746.49 ms** | **746.49 ms** | **-504.99 ms (-40.4%)** | **-586.47 ms (-44.0%)** |
| Avg Per-Msg Latency | 40.37 µs | 43.00 µs | **24.08 µs** | **24.08 µs** | **-16.29 µs (-40.4%)** | **-18.92 µs (-44.0%)** |

---

## 3. Stream Inspection & Per-Message SDP Element Metrics

*Inspection of header presence and SDP element counts across stream fixtures:*

| File | Messages Received | Total SDP Elements | Avg SDP Elements/Msg | `X-domain` Headers | `X-Seamless` Headers | `X-Call-Instance-ID` | SDP `a=x-voice-callowner-login_alias` |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **`Mixed_Stream_1.sip`** | 18 | 853 | **47.39** | 18 | 0 | 16 | 22 |
| **`Mixed_Stream_2.sip`** | 9 | 349 | **38.78** | 9 | 0 | 9 | 9 |
| **`Mixed_Stream_3.sip`** | 21 | 739 | **35.19** | 21 | 0 | 21 | 8 |
| **`RandomStream_Recv_File_1.sip`** | 459 | 21,409 | **46.64** | 459 | 34 | 344 | 549 |

---

## 4. Benchmark Environment & Methodology

- **Processor**: Apple M-series / 64-bit ARM / x86-64 single-threaded process isolation
- **Compiler**: Modern C++23 Clang / GCC with `-O3` Release optimization
- **Harness Executable**: `tests/benchmark/src/benchmark.cpp` (`sip2json_benchmark`)
- **Isolation Protocol**: Each release build (`v2.4.2`, `master`, `release/2.6.0`) was compiled and executed independently in dedicated process space to prevent memory pool interference.
