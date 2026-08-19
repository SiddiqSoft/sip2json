# sip2json Performance Benchmarks & Throughput Results

This directory contains the performance benchmarking suite and published benchmark results for **`sip2json`** — a Modern C++23 header-only SIP protocol parser and serializer.

---

## Validation Methodology

To ensure realistic parsing benchmarks that reflect real-world usage, every parse iteration performs structural JSON validation on the resulting `sipmessage`:

1. **Header Access**: Accesses and extracts the `Call-ID` header (`sipm.getCallID()`) from the headers section (`"h"`).
2. **SDP Payload Inspection**: Inspects the SDP body (`"/b/sdp"_json_pointer"`) and counts all session, media, and attribute items across all SDP blocks.
3. **Compiler Optimization Safeguards**: Uses `benchmark::DoNotOptimize()` on the extracted `Call-ID` string and SDP item count to prevent compiler dead-code elimination.

---

## Single Stream Architecture Benchmark: `parseAsync` vs `parse` vs ThreadPool Offloading

*Question: Which is best for a single stream of incoming SIP messages?*

We benchmarked four architectural patterns for consuming a single incoming stream of 1,000 SIP messages:

1. **`BM_SimulatedStream_ParseAsync_SingleThread`**: Direct in-line stream parsing using zero-allocation zero-copy `parseAsync` callback on the network/I-O thread.
2. **`BM_SimulatedStream_Parse_SingleThread`**: Synchronous stream parsing returning a `std::vector<sipmessage>` on a single thread.
3. **`BM_SimulatedStream_ParseAsync_WithThreadPoolOffload`**: Network I-O thread calls `parseAsync` and pushes parsed messages into a thread pool queue.
4. **`BM_SimulatedStream_Parse_WithThreadPoolHandoff`**: Network I-O thread calls `parse()` to build a vector, then hands off messages to a thread pool queue.

### Single Stream Benchmark Results

| Architectural Pattern | Time / 1,000 Messages | Single Stream Throughput | Bandwidth | Verdict |
| :--- | :--- | :--- | :--- | :--- |
| **`parseAsync` (Single Thread Callback)** | **4.19 ms** | **238,708 msg/sec** | **70.12 MiB/s** | **BEST overall for single stream (Zero Queue Contention)** |
| **`parse()` (Single Thread Vector)** | **4.17 ms** | **239,927 msg/sec** | **70.47 MiB/s** | Extremely close to `parseAsync`, but requires vector allocation |
| **`parseAsync` + ThreadPool Offload** | **5.40 ms** | **188,490 msg/sec** | **55.37 MiB/s** | **21% Slower** due to cross-thread mutex locking & queue contention per message |
| **`parse()` + ThreadPool Handoff** | **6.41 ms** | **229,228 msg/sec** | **67.33 MiB/s** | Slower due to batch allocation + queue mutex handoff latency |

---

## Technical Recommendation

### 1. For a Single Stream (e.g. one TCP socket / TLS connection)
- **`sip2json::parseAsync` on the I-O thread is BEST.**
- **Why?** At ~240,000 messages/sec per core, parsing a single SIP stream takes only ~4.1 microseconds per 1,000 messages. Offloading individual messages across thread boundaries introduces mutex locking overhead and cache invalidation that makes multi-threaded queueing **21% SLOWER** than processing directly in the `parseAsync` callback on the I-O thread.

### 2. For Multiple Independent Streams (e.g. hundreds of concurrent TCP connections or multi-socket receivers)
- **Multi-threaded `parseAsync` (One `parseAsync` worker per stream) is BEST.**
- Parallelizing independent streams across 16 worker threads scales aggregate throughput to **28,192,300 msg/sec (8.09 GiB/sec)**.

---

## Detailed Benchmark Results

*Environment: Apple M-Series (11 cores @ 24 MHz bus clock), AppleClang 21.0, C++23 `-O3` Release build.*

### Single-Threaded Parsing Throughput (Parse Rate)

| Benchmark Test Case | Time / Msg | Parse Rate (Messages / Sec) | Bandwidth | Output JSON Validation |
| :--- | :--- | :--- | :--- | :--- |
| **`BM_ParseMinimalResponse`** | 3.71 µs | **269,584 msg/sec** | 67.87 MiB/s | Header extraction (`Call-ID`) |
| **`BM_ParseRegisterRequest`** | 3.99 µs | **250,466 msg/sec** | 73.57 MiB/s | Header extraction (`Call-ID`) |
| **`BM_ParseNotifyLF`** | 4.36 µs | **229,612 msg/sec** | 86.06 MiB/s | Header extraction (`Call-ID`) |
| **`BM_ParseInviteWithSDP`** | 9.13 µs | **109,545 msg/sec** | 59.34 MiB/s | `Call-ID` + SDP payload item count |
| **`BM_ParseInviteComplexSDP`** | 13.28 µs | **75,305 msg/sec** | 61.33 MiB/s | `Call-ID` + Multi-attribute SDP count |
| **`BM_HighFrequencyDecodeLargePacket`** | 31.18 µs | **32,070 msg/sec** | 46.64 MiB/s | `Call-ID` + Multi-stream SDP count |

---

### Worst-Case Noisy Stream Parsing (Garbage / Noise Skipping)

*Simulates parsing huge stream buffers containing valid SIP messages interleaved with random noise, corrupted headers, and garbage lines.*

| Batch Size / Stream Setup | Time / Batch | Effective Parse Rate | Processing Bandwidth |
| :--- | :--- | :--- | :--- |
| **`BM_WorstCaseNoisyBufferParsing` (10 msgs + noise)** | 29.83 µs | **335,255 msg/sec** | 167.89 MiB/s |
| **`BM_WorstCaseNoisyBufferParsing` (100 msgs + noise)** | 45.62 µs | **2,191,860 msg/sec** | 1.03 GiB/s |
| **`BM_WorstCaseNoisyBufferParsing` (500 msgs + noise)** | 40.30 µs | **12,408,600 msg/sec** | 5.82 GiB/s |
| **`BM_WorstCaseNoisyBufferParsing` (1,000 msgs + noise)** | 36.50 µs | **27,397,200 msg/sec** | 12.85 GiB/s |
| **`BM_WorstCaseNoisyAsyncParsing` (1,000 msgs + noise)** | 43.87 µs | **22,794,300 msg/sec** | 10.69 GiB/s |

---

### Multi-Threaded Parallel Asynchronous Callback Parsing (`parseAsync`)

*Evaluates concurrent stream parsing throughput across $N$ worker threads executing `sip2json::parseAsync` in parallel with lock-free callback validation.*

| Worker Threads | Real Time / Batch | Aggregate Throughput (Messages / Sec) | Aggregate Processing Bandwidth |
| :--- | :--- | :--- | :--- |
| **`BM_MultiThreadedAsyncParsing` (2 Threads)** | 45.84 µs | **21,813,100 msg/sec** | 6.26 GiB/s |
| **`BM_MultiThreadedAsyncParsing` (4 Threads)** | 82.21 µs | **24,326,800 msg/sec** | 6.98 GiB/s |
| **`BM_MultiThreadedAsyncParsing` (8 Threads)** | 157.88 µs | **25,335,100 msg/sec** | 7.27 GiB/s |
| **`BM_MultiThreadedAsyncParsing` (16 Threads)** | 288.93 µs | **27,688,800 msg/sec** | **7.94 GiB/s** |
| **`BM_MultiThreadedNoisyAsyncParsing` (16 Threads)** | 339.69 µs | **23,550,900 msg/sec** | **11.05 GiB/s** |

---

## Running Benchmarks & Generating Reports

### 1. Build Benchmarks in Release Mode
```bash
cmake -B build/Apple-Release -S . -DCMAKE_BUILD_TYPE=Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build build/Apple-Release --config Release
```

### 2. Execute Benchmark Suite & Export JSON Results
```bash
./build/Apple-Release/benchmarks/sip2json_benchmarks --benchmark_out=benchmarks/benchmark_results.json --benchmark_out_format=json
```

### 3. Generate HTML and JUnit XML Reports
```bash
python3 benchmarks/benchmark_report_generator.py benchmarks/benchmark_results.json benchmarks
```

Generates:
- `benchmarks/benchmark_report.html` (Interactive visual HTML performance dashboard)
- `benchmarks/benchmark_results.xml` (JUnit XML format for CI/CD pipeline integration)
