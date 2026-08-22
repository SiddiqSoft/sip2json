# sip2json Performance Benchmark & Compliance Report

This document presents a comparative performance benchmark analysis evaluating the [`siddiqsoft/sip2json`](https://github.com/siddiqsoftware/sip2json) C++ library across three builds:
1. **v2.4.2 Tag**: The baseline release tag (`v2.4.2`).
2. **master Branch**: The official `master` branch upstream code.
3. **Current Optimized Code**: The latest optimized build featuring 64-bit packed `canonicalizeHeaderKey` switch optimization.

---

## 1. Executive Summary

- **Isolated Execution**: All benchmark runs were executed sequentially in dedicated, single-threaded processes to eliminate thread contention and process scheduling variance.
- **Stream Throughput Speedup**: The current optimized build achieves **33,104.58 msg/sec** stream callback throughput (`parseAsync`) and **32,365.12 msg/sec** stream vector parsing (`parse`) — representing a **+54.8% speedup over v2.4.2** and **+73.8% speedup over master**.
- **Single-Message Speedup**: Single-message parsing (`parseFromBuffer`) reached **38,485.07 msg/sec** (**+55.4% over v2.4.2**, **+65.5% over master**).
- **Latency Reduction**: Average per-message latency dropped from **46.74 µs/msg** (`v2.4.2`) and **52.51 µs/msg** (`master`) down to **30.21 µs/msg** (`parseAsync`) and **25.98 µs/msg** (`parseFromBuffer`).
- **100% Standards & Regression Pass**: All 38 compliance tests and 78 validation/regression tests passed cleanly.

---

## 2. Comparative Benchmark Matrix

*Evaluated across 36 real-world SIP message stream fixtures (164,400 total stream messages per run and 31,000 single message iterations).*

| Metric | **v2.4.2 Tag** (`v2.4.2`) | **master Branch** (`master`) | **Current Build (`parse`)** | **Current Build (`parseAsync`)** | **`parseAsync` vs v2.4.2** | **`parseAsync` vs master** |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Stream Parsing** | | | | | | |
| **Throughput (msg/sec)** | **21,394.49** | **19,043.79** | **32,365.12** | **33,104.58** | **+54.8% FASTER** | **+73.8% FASTER** |
| Execution Time (300 iters / 164.4k msgs) | 7.68 s | 8.63 s | 5.08 s | **4.97 s** | **-2.71 s (-35.3%)** | **-3.66 s (-42.4%)** |
| Data Bandwidth | 56.39 MB/s | 50.19 MB/s | 84.79 MB/s | **87.25 MB/s** | **+30.86 MB/s** | **+37.06 MB/s** |
| Avg Per-Msg Latency | 46.74 µs | 52.51 µs | 30.90 µs | **30.21 µs** | **-16.53 µs (-35.3%)** | **-22.30 µs (-42.4%)** |
| **Single-Msg Parsing (`parseFromBuffer`)** | | | | | | |
| **Throughput (msg/sec)** | **24,770.67** | **23,256.49** | **38,485.07** | **38,485.07** | **+55.4% FASTER** | **+65.5% FASTER** |
| Execution Time (1k iters / 31k msgs) | 1,251.48 ms | 1,332.96 ms | **805.51 ms** | **805.51 ms** | **-445.97 ms (-35.6%)** | **-527.45 ms (-39.6%)** |
| Avg Per-Msg Latency | 40.37 µs | 43.00 µs | **25.98 µs** | **25.98 µs** | **-14.39 µs (-35.6%)** | **-17.02 µs (-39.6%)** |

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

## 4. Standard Compliance & Open-Source Test Suite Summary

- **Official IETF RFC 4475 SIP Torture Suite**: Executed against all **50 bit-exact test cases** (`.dat` files) from Appendix A of RFC 4475.
- **SDP RFC 4566 / 8866 / 3264 / WebRTC (RFC 8829 / 8839) Suite**: Covers full session-level descriptions, Offer/Answer direction flags (`sendrecv`, `sendonly`, `recvonly`, `inactive`), WebRTC BUNDLE grouping, ICE candidate/credential attributes, and DTLS fingerprints.
- **Ecosystem Open-Source Test Vectors**: Incorporated standard scenario fixtures from open-source projects including **SIPp** (`sipp_uac_invite.sip`, `sipp_uas_200ok.sip`), **OpenSIPS `sipssert`** (digest authentication & registration), **PROTOS c07-sip** (malformed header fuzzing vectors), **W3C Web Platform Tests** (WebRTC SDP blobs), and **baresip / re** (C parser test fixtures).
- **Compliance Pass Rate**: **38 / 38 CTest Compliance Tests Passed (100%)**
- **Validation Pass Rate**: **78 / 78 CTest Validation Tests Passed (100%)**

---

## 5. Methodology & Environment

- **OS**: macOS (Apple Silicon ARM64)
- **Compiler**: Clang / LLVM (`-std=c++23`, `-O3` Release optimization)
- **Test Fixtures**: 36 anonymized `.sip` files representing real-world SIP traffic (`REGISTER`, `INVITE`, `NOTIFY`, multi-part SDP, and multi-message streams).
- **Harness Implementation**: C++ `std::chrono::high_resolution_clock` measuring in-memory buffer parsing iterations in `sip2json_benchmark`.

---

## 6. How to Reproduce

```bash
# Benchmark Current Code
./build/Apple-Release/tests/benchmark/sip2json_benchmark tests/validation/samples

# Benchmark Master Branch
cmake -B build/Benchmark-master -S tests/benchmark -DCMAKE_BUILD_TYPE=Release -DSIP2JSON_VERSION=master
cmake --build build/Benchmark-master
./build/Benchmark-master/sip2json_benchmark tests/validation/samples

# Benchmark v2.4.2 Tag
cmake -B build/Benchmark-v2.4.2 -S tests/benchmark -DCMAKE_BUILD_TYPE=Release -DSIP2JSON_VERSION=2.4.2
cmake --build build/Benchmark-v2.4.2
./build/Benchmark-v2.4.2/sip2json_benchmark tests/validation/samples
```
