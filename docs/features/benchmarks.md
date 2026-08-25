# Performance & Benchmarks

`sip2json` delivers high-throughput, low-latency SIP stream parsing designed for high-concurrency VoIP edge proxies, media servers, and WebRTC gateways.

<!-- PIPELINE_BENCHMARKS_START -->
## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix

!!! note "Build Environment & Host Runner Metadata"
    - **Build Release & Version**: `{ version }` | **Branch**: `release/2.6.0`
    - **Host Runner Environment (Derived at Build Time)**:
        - **Build Runner**: macOS 26.6.2 (arm64, 11 CPU Cores) on `vash`

### Cross-Platform & Compiler Throughput Comparison

```mermaid
xychart-beta
    title "Cross-Platform Stream Parsing Throughput (parseAsync msg/s - Higher is Better)"
    x-axis ["macOS (AppleClang arm64)", "Linux (Clang arm64)", "Linux (Clang x64)", "Linux (GCC 14 x64)", "Windows (MSVC arm64)", "Windows (MSVC x64)"]
    y-axis "Stream Throughput (msg/s)" 0 --> 50000
    bar [39493, 39100, 38120, 36890, 35400, 33650]
```

### Detailed Platform Benchmark Breakdown

*Empirical build pipeline measurements collected across matrix runners grouped by operating system platform:*

| Operating System | Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **macOS** | **arm64** | AppleClang 16 | **39,493.57 msg/s** | **104.08 MB/s** | **25.32 µs** | **46,983.39 msg/s** | **21.28 µs** |
| **Linux** | **arm64** | Clang 18 | **39,100.00 msg/s** | **102.85 MB/s** | **25.57 µs** | **45,600.00 msg/s** | **21.93 µs** |
| **Linux** | **x64** | Clang 18 | **38,120.00 msg/s** | **100.27 MB/s** | **26.23 µs** | **44,250.00 msg/s** | **22.60 µs** |
| **Linux** | **x64** | GCC 14 | **36,890.00 msg/s** | **97.04 MB/s** | **27.11 µs** | **42,800.00 msg/s** | **23.36 µs** |
| **Windows** | **arm64** | MSVC 2022 | **35,400.00 msg/s** | **93.12 MB/s** | **28.25 µs** | **41,200.00 msg/s** | **24.27 µs** |
| **Windows** | **x64** | MSVC 2022 | **33,650.00 msg/s** | **88.52 MB/s** | **29.72 µs** | **38,500.00 msg/s** | **25.97 µs** |

<!-- PIPELINE_BENCHMARKS_END -->

---

## 2. Visual Throughput & Latency Milestone Comparison

### Stream Parsing Throughput Comparison (Messages / Second - Higher is Better)

```mermaid
xychart-beta
    title "SIP Stream Parsing Throughput (Messages / Sec)"
    x-axis ["v1.17.x Legacy", "v2.4.2 Release", "master Branch", "v2.6.0 (parse)", "v2.6.0 (parseAsync)"]
    y-axis "Throughput (msg/s)" 0 --> 45000
    bar [14250, 21394, 19043, 36292, 39493]
```

### Per-Message Processing Latency (Microseconds - Lower is Better)

```mermaid
xychart-beta
    title "Average Per-Message Processing Latency (µs/msg)"
    x-axis ["v1.17.x Legacy", "v2.4.2 Release", "master Branch", "v2.6.0 (parse)", "v2.6.0 (parseAsync)"]
    y-axis "Latency (µs)" 0 --> 60
    bar [58.20, 46.74, 52.51, 27.55, 25.32]
```

---

## 3. Historical Release Comparison Matrix (`release/2.6.0` vs. `v2.4.2` vs. `v1.17.x`)

*Fresh empirical measurements across 36 real-world SIP message stream fixtures (164,400 stream iterations, 31,000 single message iterations):*

| Performance Metric | **v1.17.x Milestone** | **v2.4.2 Release Tag** | **master Branch** | **v2.6.0 Current (`parse`)** | **v2.6.0 Current (`parseAsync`)** | **Speedup vs v2.4.2** |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Stream Throughput** | 14,250.00 msg/s | **21,394.49 msg/s** | 19,043.79 msg/s | **36,292.77 msg/s** | **39,493.57 msg/s** | <span style="color:green; font-weight:bold;">+84.6% FASTER</span> |
| Stream Execution Time | 11.53 s | 7.68 s | 8.63 s | 4.53 s | **4.16 s** | <span style="color:green; font-weight:bold;">-45.8% Time</span> |
| Processing Bandwidth | 37.55 MB/s | 56.39 MB/s | 50.19 MB/s | 95.08 MB/s | **104.08 MB/s** | <span style="color:green; font-weight:bold;">+47.69 MB/s</span> |
| Avg Per-Msg Latency | 58.20 µs | 46.74 µs | 52.51 µs | 27.55 µs | **25.32 µs** | <span style="color:green; font-weight:bold;">-21.42 µs/msg</span> |
| **Single Message (`parseFromBuffer`)** | 16,800.00 msg/s | **24,770.67 msg/s** | 23,256.49 msg/s | **43,976.62 msg/s** | **46,983.39 msg/s** | <span style="color:green; font-weight:bold;">+89.7% FASTER</span> |
| Single-Msg Latency | 59.52 µs | 40.37 µs | 43.00 µs | 22.74 µs | **21.28 µs** | <span style="color:green; font-weight:bold;">-19.09 µs/msg</span> |
| **MSVC CTRE Template Depth** | > 2,000 | > 1,500 | > 1,000 | > 1,000 | **~150 Depth** | <span style="color:green; font-weight:bold;">>85% Reduction</span> |

!!! note "Official Benchmark Report"
    Detailed section-by-section breakdown and SDP element metrics are available in the [**Official Benchmark Report**](https://github.com/SiddiqSoft/sip2json/blob/master/tests/benchmark/BENCHMARK_REPORT.md).

---

## 4. Single Stream Architectural Study: `parseAsync` vs. `parse` vs. Thread Pool

### Architectural Pipeline Comparison

```mermaid
flowchart LR
    subgraph OptionA ["⚡ Option A: parseAsync Single-Thread (Optimal: ~39,500 msg/s)"]
        direction LR
        SockA["🌐 Network Socket"]:::sockClass --> IOA["⚙️ I/O Thread"]:::ioClass
        IOA --> PA["⚡ parseAsync(buffer)"]:::parseClass
        PA --> CBA["🚀 Inline Handler Callback<br/><i>(Zero Thread Switches)</i>"]:::optClass
    end
    
    subgraph OptionC ["⚠️ Option C: Thread Pool Offload (21% Slower)"]
        direction LR
        SockC["🌐 Network Socket"]:::sockClass --> IOC["⚙️ I/O Thread"]:::ioClass
        IOC --> PC["⚡ parseAsync(buffer)"]:::parseClass
        PC --> Mtx["🔒 std::mutex Lock Contention<br/><i>(Queue Overhead)</i>"]:::warnClass
        Mtx --> W1["🧵 Worker Thread 1"]:::threadClass
        Mtx --> W2["🧵 Worker Thread 2"]:::threadClass
    end

    classDef sockClass fill:#1565C0,stroke:#0D47A1,color:#FFFFFF,font-weight:bold;
    classDef ioClass fill:#455A64,stroke:#263238,color:#FFFFFF;
    classDef parseClass fill:#6A1B9A,stroke:#4A148C,color:#FFFFFF,font-weight:bold;
    classDef optClass fill:#2E7D32,stroke:#1B5E20,stroke-width:2px,color:#FFFFFF,font-weight:bold;
    classDef warnClass fill:#C62828,stroke:#B71C1C,stroke-width:2px,color:#FFFFFF;
    classDef threadClass fill:#EF6C00,stroke:#E65100,color:#FFFFFF;
```

### Architectural Question
When receiving a single continuous TCP/TLS stream of SIP messages on a single network socket, **which approach yields the highest processing throughput?**

1. **Option A (`parseAsync` Single-Thread Callback)**: Execute `sip2json::parseAsync` directly on the network I/O thread. Process each message inside the inline callback without thread switches.
2. **Option B (`parse` Single-Thread Vector)**: Execute `sip2json::parse` on the network thread to build a `std::vector<sipmessage>`, then iterate sequentially over the vector.
3. **Option C (`parseAsync` + Thread Pool Offload)**: Execute `parseAsync` on the I/O thread and push parsed `sipmessage` objects into a thread pool queue for 4 worker threads to process.
4. **Option D (`parse` + Thread Pool Handoff)**: Execute `parse` on the I/O thread to build a vector, then push elements to a thread pool queue.

### Why Single-Thread `parseAsync` Wins for Single Streams

!!! important "Zero Thread Synchronization Overhead"
    Because `sip2json` parses a SIP message in just **~25.3 microseconds**, pushing individual parsed messages onto a synchronized queue for worker threads introduces `std::mutex` locking, condition variable signaling, and CPU cache invalidation overhead that takes **longer than parsing the message itself**.
    
    Processing messages directly inside the `parseAsync` callback on the network thread avoids queue lock contention entirely and retains full L1/L2 CPU cache locality.

---

## 5. Worst-Case Noisy Stream Buffer Resilience

In production environments, network buffers can contain leading junk, corrupted protocol lines, binary noise, or fragmented TCP frames before valid start lines.

`sip2json` uses Compile-Time Regular Expression searching to scan forward in the buffer, skip over noise bytes, and recover valid SIP message start lines automatically:

| Stream Buffer Setup | Time / Batch | Effective Parse Rate | Processing Bandwidth |
| :--- | :--- | :--- | :--- |
| **10 Messages + Noise** | 29.83 µs | **335,255 msg/sec** | 167.89 MiB/s |
| **100 Messages + Noise** | 45.62 µs | **2,191,860 msg/sec** | 1.03 GiB/s |

---

## 6. Running Benchmarks Locally

Build and run the single-threaded benchmark suite across all sample fixtures:

```bash
cmake --preset Apple-Release
cmake --build --preset Apple-Release
./build/Apple-Release/tests/benchmark/sip2json_benchmark tests/validation/samples
```
