# Performance & Benchmarks

`sip2json` delivers high-throughput, low-latency SIP stream parsing engineered for high-concurrency VoIP edge proxies, SBCs, and WebRTC media gateways.

<!-- PIPELINE_BENCHMARKS_START -->
## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix

!!! info "Pipeline-Derived Performance Data"
    Benchmark metrics and host runner environment details are compiled dynamically from CI/CD pipeline build matrix artifacts across our release matrix runners (Apple macOS, Red Hat Enterprise Linux, and Microsoft Windows). When release builds complete, live benchmark data will populate automatically.

| Platform & Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| *Pipeline Build Pending* | *CI Matrix* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* |

---

## 2. Visual Platform Performance Comparison

!!! info "Comparative Visual Performance Graphs Pending Build"
    Visual comparative bar charts contrasting throughput and latency across Apple macOS, Red Hat Enterprise Linux (RHEL), and Microsoft Windows runners will render automatically when pipeline benchmark artifacts are compiled.

<!-- PIPELINE_BENCHMARKS_END -->

---

## 3. Running Benchmarks Locally

To compile and execute the benchmark suite on your local machine:

```bash
# macOS (AppleClang Release)
cmake --preset Darwin-Clang-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Darwin-Clang-Release --target sip2json_benchmarks
./build/Darwin-Clang-Release/benchmarks/sip2json_benchmarks

# Linux (GCC Release)
cmake --preset Linux-GCC-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Linux-GCC-Release --target sip2json_benchmarks
./build/Linux-GCC-Release/benchmarks/sip2json_benchmarks

# Windows (MSVC Release)
cmake --preset Windows-x64-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Windows-x64-Release --target sip2json_benchmarks
.\build\Windows-x64-Release\benchmarks\sip2json_benchmarks.exe
```
