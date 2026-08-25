# Performance & Benchmarks

`sip2json` delivers high-throughput, low-latency SIP stream parsing designed for high-concurrency VoIP edge proxies, media servers, and WebRTC gateways.

<!-- PIPELINE_BENCHMARKS_START -->
## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix

*Empirical build pipeline measurements collected dynamically across live matrix runners (Build Version `{ version }`):*

| Operating System | Architecture | Compiler | Host Environment (CPU & RAM) | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |
| :--- | :---: | :---: | :--- | :---: | :---: | :---: | :---: | :---: |
| **macOS** | **arm64** | AppleClang | macOS 26.6.2 (arm64, 11 CPU Cores, 18 GB RAM) | **30,988.47 msg/s** | **81.67 MB/s** | **32.27 µs** | **33,874.63 msg/s** | **29.52 µs** |

<!-- PIPELINE_BENCHMARKS_END -->

---

## 2. Stream Inspection & Per-Message SDP Element Metrics

*Empirical inspection of header presence and SDP element counts across real-world stream fixtures:*

| Fixture Stream File | Messages Received | Total SDP Elements | Avg SDP Elements / Msg | `X-domain` Headers | `X-Seamless` Headers | `X-Call-Instance-ID` | SDP `a=x-voice-callowner-login_alias` |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **`Mixed_Stream_1.sip`** | 18 | 853 | **47.39** | 18 | 0 | 16 | 22 |
| **`Mixed_Stream_2.sip`** | 9 | 349 | **38.78** | 9 | 0 | 9 | 9 |
| **`Mixed_Stream_3.sip`** | 21 | 739 | **35.19** | 21 | 0 | 21 | 8 |
| **`RandomStream_Recv_File_1.sip`** | 459 | 21,409 | **46.64** | 459 | 34 | 344 | 549 |

---

## 3. Single Stream Architectural Study: `parseAsync` vs. `parse` vs. Thread Pool

### Architectural Pipeline Comparison

```mermaid
flowchart LR
    subgraph OptionA ["⚡ Option A: parseAsync Single-Thread (Optimal: Zero Locks)"]
        direction LR
        SockA["🌐 Network Socket"]:::sockClass --> IOA["⚙️ I/O Thread"]:::ioClass
        IOA --> PA["⚡ parseAsync(buffer)"]:::parseClass
        PA --> CBA["🚀 Inline Handler Callback<br/><i>(Zero Thread Switches)</i>"]:::optClass
    end
    
    subgraph OptionC ["⚠️ Option C: Thread Pool Offload (Lock Contention)"]
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
3. **Option C (`parseAsync` + Thread Pool Offload)**: Execute `parseAsync` on the I/O thread and push parsed `sipmessage` objects into a thread pool queue for worker threads to process.
4. **Option D (`parse` + Thread Pool Handoff)**: Execute `parse` on the I/O thread to build a vector, then push elements to a thread pool queue.

### Why Single-Thread `parseAsync` Wins for Single Streams

!!! important "Zero Thread Synchronization Overhead"
    Because `sip2json` parses a SIP message with microsecond-level latency, pushing individual parsed messages onto a synchronized queue for worker threads introduces `std::mutex` locking, condition variable signaling, and CPU cache invalidation overhead that takes **longer than parsing the message itself**.
    
    Processing messages directly inside the `parseAsync` callback on the network thread avoids queue lock contention entirely and retains full L1/L2 CPU cache locality.

---

## 4. Running Benchmarks Locally

Build and run the single-threaded benchmark suite across all sample fixtures:

```bash
cmake --preset Apple-Release
cmake --build --preset Apple-Release
./build/Apple-Release/tests/benchmark/sip2json_benchmark tests/validation/samples
```


