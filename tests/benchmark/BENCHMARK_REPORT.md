# Performance Benchmark Report

Official benchmark results for `siddiqsoft/sip2json` comparing performance across releases: `v2.4.2` tag, `master` branch, and `release/2.6.0` (with 64-bit FNV-1a zero-collision precomputed hash matching and merged `HeaderKeySet` zero-copy architecture).

---

## 1. Key Performance Highlights

- **Stream Throughput (`parseAsync`)**: Increased from **21,364.24 msg/sec** (`v2.4.2`) and **19,227.57 msg/sec** (`master`) to **37,260.49 msg/sec** (**+74.4% faster than v2.4.2** and **+93.8% faster than master**).
- **Single Message Throughput (`parseFromBuffer`)**: Increased from **24,770.67 msg/sec** (`v2.4.2`) and **23,256.49 msg/sec** (`master`) to **43,976.62 msg/sec** (**+77.5% faster than v2.4.2** and **+89.1% faster than master**).
- **Latency Reduction**: Average per-message latency dropped from **46.81 µs/msg** (`v2.4.2`) and **52.01 µs/msg** (`master`) down to **26.84 µs/msg** (`parseAsync`) and **22.74 µs/msg** (`parseFromBuffer`).
- **Data Bandwidth**: Stream parsing bandwidth reached **98.20 MB/sec** on single-core network execution.
- **100% Standards & Regression Pass**: All 39 compliance tests and 78 validation/regression tests passed cleanly.

---

## 2. Comparative Benchmark Matrix

*Evaluated across 36 real-world SIP message stream fixtures (164,400 total stream messages per run and 31,000 single message iterations).*

| Metric | **v2.4.2 Tag** (`v2.4.2`) | **master Branch** (`master`) | **Current Build (`parse`)** | **Current Build (`parseAsync`)** | **`parseAsync` vs v2.4.2** | **`parseAsync` vs master** |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Stream Parsing** | | | | | | |
| **Throughput (msg/sec)** | **21,394.49** | **19,043.79** | **36,292.77** | **37,260.49** | **+74.4% FASTER** | **+93.8% FASTER** |
| Execution Time (300 iters / 164.4k msgs) | 7.68 s | 8.63 s | 4.53 s | **4.41 s** | **-3.27 s (-42.6%)** | **-4.22 s (-48.9%)** |
| Data Bandwidth | 56.39 MB/s | 50.19 MB/s | 95.08 MB/s | **98.20 MB/s** | **+41.81 MB/s** | **+48.01 MB/s** |
| Avg Per-Msg Latency | 46.74 µs | 52.51 µs | 27.55 µs | **26.84 µs** | **-19.97 µs (-42.7%)** | **-25.17 µs (-48.4%)** |
| **Single-Msg Parsing (`parseFromBuffer`)** | | | | | | |
| **Throughput (msg/sec)** | **24,770.67** | **23,256.49** | **43,976.62** | **43,976.62** | **+77.5% FASTER** | **+89.1% FASTER** |
| Execution Time (1k iters / 31k msgs) | 1,251.48 ms | 1,332.96 ms | **704.92 ms** | **704.92 ms** | **-546.56 ms (-43.7%)** | **-628.04 ms (-47.1%)** |
| Avg Per-Msg Latency | 40.37 µs | 43.00 µs | **22.74 µs** | **22.74 µs** | **-17.63 µs (-43.7%)** | **-20.26 µs (-47.1%)** |

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
