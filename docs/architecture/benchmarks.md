# Performance & Benchmarks

<!-- PIPELINE_BENCHMARKS_START -->
## Performance & Benchmarks

!!! info "Pipeline-Derived Performance Data"
    Benchmark metrics and host runner environment details are compiled dynamically from CI/CD pipeline build matrix artifacts across our release matrix runners (Apple macOS, Red Hat Enterprise Linux, and Microsoft Windows — both GCC and Clang for Linux). When release builds complete, live benchmark data will populate automatically.

| Platform & Architecture | Compiler | Stream Throughput<br>`parseAsync` | Bandwidth | Per-Msg Latency | Single Msg Throughput<br>`parseFromBuffer` | Single Msg Latency |
| :--- | :---: | :--- | :---: | :--- | :--- | :--- |
| *Pipeline Build Pending* | *CI Matrix* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* |

<!-- PIPELINE_BENCHMARKS_END -->

> [!NOTE]
> **Host Architecture Context**: All matrix runners reside on the same physical host hardware (Apple Mac Mini M4 Pro, 24 GB RAM, external NVMe SSD). For an analysis of bare-metal host vs. guest virtual machine execution and compiler code generation, see [Platform Performance Variance & Host Architecture](#3-platform-performance-variance--host-architecture).

---

## 2. Running Benchmarks Locally

To compile and execute the benchmark suite on your local machine:

```bash
# macOS (AppleClang Release)
cmake --preset Darwin-Clang-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Darwin-Clang-Release --target sip2json_benchmarks
./build/Darwin-Clang-Release/benchmarks/sip2json_benchmarks

# Linux — GCC Release
cmake --preset Linux-GCC-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Linux-GCC-Release --target sip2json_benchmarks
./build/Linux-GCC-Release/benchmarks/sip2json_benchmarks

# Linux — Clang Release
cmake --preset Linux-Clang-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Linux-Clang-Release --target sip2json_benchmarks
./build/Linux-Clang-Release/benchmarks/sip2json_benchmarks

# Windows (MSVC Release)
cmake --preset Windows-x64-Release -Dsip2json_BUILD_BENCHMARKS=ON
cmake --build --preset Windows-x64-Release --target sip2json_benchmarks
.\build\Windows-x64-Release\benchmarks\sip2json_benchmarks.exe
```

---

## 3. Platform Performance Variance & Host Architecture

All CI/CD release matrix runners execute on the same physical host machine:
- **Physical Host**: Apple Mac Mini (Apple M4 Pro, 24 GB Unified Memory, 273 GB/s memory bandwidth, external NVMe SSD storage).
- **Execution Topology**:
  - **macOS Runner**: Runs natively on the bare-metal Darwin host kernel.
  - **Linux & Windows Runners**: Run inside guest virtual machines allocated 4 vCPUs and 6 GB RAM managed by the host hypervisor.

### Key Factors Influencing Cross-Platform Measurements

#### 1. Bare-Metal vs. Hypervisor Virtualization (Memory & Address Translation)
- **Bare-Metal Execution (macOS)**: Memory address translation is direct ($L1 \rightarrow L2 \rightarrow \text{RAM}$). The parser accesses buffer pages with zero hypervisor intervention and utilizes the full 273 GB/s host memory bus.
- **Virtualized Execution (Linux & Windows)**: Every memory access requires **Two-Stage Address Translation (SLAT)** through the hypervisor (Guest Virtual $\rightarrow$ Guest Physical $\rightarrow$ Host Physical). In buffer-scanning, pointer-chasing, and allocation workloads, TLB misses and page table traversals incur hypervisor trap-and-emulate overhead.
- **Guest Buffer Cache Capacity**: The guest VMs are allocated 6 GB of RAM, of which 2–3 GB is consumed by the guest OS kernel and background services, leaving constrained memory for filesystem and buffer caching compared to the host's 24 GB pool.

#### 2. Heterogeneous Core Scheduling (Performance vs. Efficiency Cores)
- The M4 Pro features an asymmetric core topology comprising high-frequency Performance (P) cores (~4.4 GHz, 8-wide decode) and Efficiency (E) cores (~2.8 GHz, narrow execution pipelines).
- On bare-metal macOS, the Darwin thread scheduler pins CPU-intensive benchmark loops to P-cores.
- Inside virtual machines with 4 unpinned vCPUs, the host hypervisor worker threads may be context-switched between P-cores and E-cores by the host scheduler under background host activity, introducing variance in measured throughput.

#### 3. Microarchitecture Target Defaults & Build Optimization Flags
- **Compiler Target Defaults**:
  - **AppleClang (macOS)**: Automatically emits instruction scheduling tuned for Apple Silicon core pipelines, and links against Apple's `libc++` which features vectorized NEON implementations of `std::string_view` search operations (`memchr`, `std::char_traits::find`).
  - **GCC / Clang (Linux)**: By default, compilers target baseline `armv8-a` to preserve distribution portability across generic ARM hardware. Build optimization flags (`-O3 -mcpu=native -flto=auto -fno-semantic-interposition -fomit-frame-pointer`) are applied to enable host-specific instruction scheduling and eliminate PLT indirection overhead.
  - **MSVC (Windows)**: Uses `/O2 /Oi /Ot /Ob3 /GL` and `/LTCG` for Whole Program Optimization and intrinsic expansion.
