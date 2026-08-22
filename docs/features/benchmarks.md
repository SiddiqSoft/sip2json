# Performance & Benchmarks

`sip2json` delivers high-throughput, low-latency SIP stream parsing designed for high-concurrency VoIP edge proxies, media servers, and WebRTC gateways.

---

## 1. Release Comparison Matrix (`release/2.6.0` vs. `master` vs. `v2.4.2`)

*Fresh empirical measurements across 36 real-world SIP message stream fixtures (164,400 stream iterations, 31,000 single message iterations):*

| Metric | **v2.4.2 Release Tag** | **master Branch** | **v2.6.0 Current (`parse`)** | **v2.6.0 Current (`parseAsync`)** | **Speedup vs v2.4.2** |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Stream Throughput** | **21,394.49 msg/s** | **19,043.79 msg/s** | **34,653.48 msg/s** | **35,541.38 msg/s** | **+66.1% FASTER** |
| Stream Execution Time | 7.68 s | 8.63 s | 4.74 s | **4.63 s** | **-39.7% Time** |
| Processing Bandwidth | 56.39 MB/s | 50.19 MB/s | 90.78 MB/s | **93.67 MB/s** | **+37.28 MB/s** |
| Avg Per-Msg Latency | 46.74 µs | 52.51 µs | 28.86 µs | **28.14 µs** | **-18.60 µs/msg** |
| **Single Message (`parseFromBuffer`)** | **24,770.67 msg/s** | **23,256.49 msg/s** | **41,527.70 msg/s** | **41,527.70 msg/s** | **+67.6% FASTER** |
| Single-Msg Latency | 40.37 µs | 43.00 µs | **24.08 µs** | **24.08 µs** | **-16.29 µs/msg** |

> [!NOTE]
> Detailed section-by-section breakdown and SDP element metrics are available in the [**Official Benchmark Report**](https://github.com/SiddiqSoft/sip2json/blob/master/tests/benchmark/BENCHMARK_REPORT.md).

---

## 2. Single Stream Architectural Study: `parseAsync` vs. `parse` vs. Thread Pool

### Architectural Pipeline Comparison

```mermaid
flowchart LR
    subgraph OptionA ["Option A: parseAsync Single-Thread (Optimal - 35,541 msg/sec)"]
        direction LR
        SockA["Network Socket"] --> IOA["I/O Thread"]
        IOA --> PA["parseAsync(buffer)"]
        PA --> CBA["Inline Handler Callback"]
    end
    
    subgraph OptionC ["Option C: parseAsync + Thread Pool Offload (21% Slower)"]
        direction LR
        SockC["Network Socket"] --> IOC["I/O Thread"]
        IOC --> PC["parseAsync(buffer)"]
        PC --> Mtx["std::mutex Queue Lock Contention"]
        Mtx --> W1["Worker Thread 1"]
        Mtx --> W2["Worker Thread 2"]
    end
```

### Architectural Question
When receiving a single continuous TCP/TLS stream of SIP messages on a single network socket, **which approach yields the highest processing throughput?**

1. **Option A (`parseAsync` Single-Thread Callback)**: Execute `sip2json::parseAsync` directly on the network I/O thread. Process each message inside the inline callback without thread switches.
2. **Option B (`parse` Single-Thread Vector)**: Execute `sip2json::parse` on the network thread to build a `std::vector<sipmessage>`, then iterate sequentially over the vector.
3. **Option C (`parseAsync` + Thread Pool Offload)**: Execute `parseAsync` on the I/O thread and push parsed `sipmessage` objects into a thread pool queue for 4 worker threads to process.
4. **Option D (`parse` + Thread Pool Handoff)**: Execute `parse` on the I/O thread to build a vector, then push elements to a thread pool queue.

### Why Single-Thread `parseAsync` Wins for Single Streams

> [!IMPORTANT]
> **Zero Thread Synchronization Overhead**
> Because `sip2json` parses a SIP message in just **~28.1 microseconds**, pushing individual parsed messages onto a synchronized queue for worker threads introduces `std::mutex` locking, condition variable signaling, and CPU cache invalidation overhead that takes **longer than parsing the message itself**.
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

---

## 4. Running Benchmarks Locally

Build and run the single-threaded benchmark suite across all sample fixtures:

```bash
cmake --preset Apple-Release
cmake --build --preset Apple-Release
./build/Apple-Release/tests/benchmark/sip2json_benchmark tests/validation/samples
```
