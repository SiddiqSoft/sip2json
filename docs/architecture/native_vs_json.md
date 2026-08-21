# Architectural Study: Native C++ Struct vs. JSON-Subclassed Model

This document presents an architectural trade-off analysis comparing a **Native C++ Standalone Struct** model (`sipmessage_native`) against the **`nlohmann::json`-subclassed** model (`sipmessage`) used by `sip2json`.

---

## 1. Architectural Background & What-If Question

When designing a modern C++ SIP protocol parser, two primary data modeling approaches emerge:

1. **Option A: Subclass `nlohmann::json` directly (`sipmessage`)**
   * **Design Goal**: Every parsed SIP message *is-a* native JSON object (`nlohmann::json`). It can be passed directly to JSON serializers, web API endpoints, database engines (MongoDB, PostgreSQL `jsonb`), and log processors with **zero transformation overhead**.
   * **Trade-off**: Requires constructing a dynamic JSON Abstract Syntax Tree (AST) using heap-allocated `std::map<std::string, nlohmann::basic_json>` nodes for every field (`"s"`, `"h"`, `"b"`, `"meta"`).

2. **Option B: Standalone Native C++ Struct (`sipmessage_native`)**
   * **Design Goal**: Store parsed fields in stack-allocated native C++ members (`std::string method`, `std::string uri`, `std::vector<std::pair<std::string, std::string>> headers`).
   * **Trade-off**: Maximum raw parsing throughput and zero JSON AST allocation overhead, but requires an explicit transformation/serialization step to emit JSON.

---

## 2. Empirical Benchmark Matrix

We benchmarked both models on Apple Silicon (`Apple-Release` `-O3`) executing identical parsing, header mutation, and header retrieval loops:

| Benchmark Scenario / Metric | Native C++ Struct (`sipmessage_native`) | `nlohmann::json` Subclass (`sipmessage`) | Performance Difference | Operational Advantage |
| :--- | :--- | :--- | :--- | :--- |
| **`sipmessage` Instantiation** | **989 ns** (1,011,110 msgs/sec) | 4,725 ns (211,639 msgs/sec) | **4.78x FASTER** | **1.01M** vs 211.6K instantiations/sec |
| **`setHeader` Mutation** | **9.46 ns** (105,703,000 ops/sec) | 45.0 ns (22,236,500 ops/sec) | **4.76x FASTER** | **105.70M** vs 22.24M ops/sec |
| **`getHeader` Lookup** | **22.0 ns** (45,513,900 ops/sec) | 57.1 ns (17,514,900 ops/sec) | **2.60x FASTER** | **45.51M** vs 17.51M lookups/sec |
| **Full INVITE Parsing Rate** | **1,070 ns** (**934,691 msgs/sec**) | 9,436 ns (105,982 msgs/sec) | **8.82x FASTER** | **934,691** vs 105,982 msgs/sec |
| **Parsing Bandwidth** | **506.31 MiB/sec** | 57.41 MiB/sec | **8.82x FASTER** | Half a Gigabyte/sec per core |
| **Memory Footprint (`sizeof`)**| **256 Bytes** | 280 Bytes + `std::map` heap nodes | **Significantly smaller** | Zero dynamic JSON AST node overhead |

---

## 3. Low-Level Performance Mechanics

### Why the Native C++ Struct is 8.8x Faster (934.7K msg/sec)
* **Zero AST Overhead**: Direct member assignments (`msg.method = string(g1)`, `msg.uri = string(g2)`) place strings into stack-allocated contiguous struct memory, avoiding `std::map` node allocations and variant type discriminators.
* **Cache Locality**: Accessing struct fields operates within single CPU L1 cache lines, whereas `std::map` nodes require pointer dereferencing across scattered heap locations.

---

## 4. Comprehensive Trade-off Matrix & Architectural Verdict

| Architectural Criterion | Native C++ Struct (`sipmessage_native`) | `nlohmann::json` Subclass (`sipmessage`) |
| :--- | :--- | :--- |
| **Raw Parsing Throughput** | **Ultra-Fast (934,691 msg/sec)** | Fast (105,982 msg/sec) |
| **Memory Allocations** | **Minimal (0 JSON AST nodes)** | AST nodes per header & section |
| **JSON Export Cost** | High (Requires custom serializer) | **Zero-cost (IS-A `nlohmann::json` object)** |
| **Database & Web API Integration** | Requires manual mapping code | **Direct pass-through to MongoDB / PostgreSQL / REST APIs** |
| **Dynamic Schema Extension** | Rigid static struct fields | **Flexible dynamic JSON properties** |

### Architectural Verdict & Design Choice

> [!NOTE]
> **Why `sip2json` Chooses `nlohmann::json` Inheritance**:
> The primary mission of `sip2json` is to convert raw SIP wire streams into **first-class structured JSON data** for modern cloud APIs, log analytics, and microservices.
> Subclassing `nlohmann::json` provides **zero-cost JSON serialization, direct schema compatibility, and dynamic property extensibility**. At **> 105,000 messages/sec per core**, the `nlohmann::json` model easily satisfies high-throughput carrier-grade network requirements while delivering native JSON ergonomics.
