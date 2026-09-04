# sip2json Performance Benchmarks & Throughput Results

This directory contains the performance benchmarking suite and published benchmark results for **`sip2json`** — a Modern C++20 header-only SIP protocol parser and serializer.

---

## Validation Methodology

To ensure realistic parsing benchmarks that reflect real-world usage, every parse iteration performs structural JSON validation on the resulting `sipmessage`:

1. **Header Extraction Verification**: Extracts `Call-ID` or `Via` from the parsed JSON document, ensuring the parser successfully populates the header table.
2. **SDP Extraction Verification**: If the message contains an SDP payload, extracts the media descriptor count and attributes from `/b/sdp`, guaranteeing full SDP parsing is exercised.
3. **`benchmark::DoNotOptimize`**: All extracted fields are passed to `benchmark::DoNotOptimize` to prevent compiler dead-code elimination.

---

## Benchmark Categories

The benchmark suite covers five primary operational dimensions:

1. **Single-Message Parsing**: Throughput and latency for realistic SIP requests and responses (INVITE, REGISTER, 200 OK) with varying header complexity and SDP bodies.
2. **Streaming Callback Parsing (`parseAsync`)**: High-throughput stream decoding simulating continuous network buffers with zero copies.
3. **SIP Message Construction & Serialization**: In-memory message instantiation, header mutations, and RFC 3261 wire-format serialization.
4. **Worst-Case Noisy Stream Parsing**: Robustness and throughput under heavy noise, garbage line skipping, and framing boundary recovery.

---

## Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix

Multi-platform benchmark measurements and runner host environment details are compiled dynamically from CI/CD pipeline build matrix artifacts across release matrix runners (Apple macOS, Red Hat Enterprise Linux, and Microsoft Windows).

Refer to the live, pipeline-generated performance matrix at [**docs/architecture/benchmarks.md**](../docs/architecture/benchmarks.md) or online at [**siddiqsoft.github.io/sip2json/architecture/benchmarks/**](https://siddiqsoft.github.io/sip2json/architecture/benchmarks/).

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
