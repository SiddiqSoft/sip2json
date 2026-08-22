# sip2json-client

A comprehensive test client for the [sip2json](https://github.com/siddiqsoftware/sip2json) library - a modern C++ SIP protocol parser that converts SIP messages to JSON format.

<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.sip2json-client?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=DEFINITIONID&branchName=main)
![](https://img.shields.io/github/v/tag/SiddiqSoft/sip2json-client)
<!-- end badges -->

## Objective

This project provides a comprehensive test suite for the `sip2json` library, demonstrating its capabilities for parsing, manipulating, and serializing SIP protocol messages. The test client validates core functionality including:

- **SIP Message Parsing**: Parse SIP requests and responses from raw buffers
- **Message Creation**: Create SIP requests and responses programmatically
- **JSON Serialization**: Convert SIP messages to JSON format and back
- **Header Manipulation**: Get, set, and manage SIP headers
- **Body/SDP Handling**: Parse and manipulate Session Description Protocol (SDP) bodies
- **Error Handling**: Comprehensive error detection and reporting
- **Async Parsing**: Non-blocking message parsing with callbacks
- **Edge Cases**: Handle various protocol edge cases and malformed messages

## Requirements

- **CMake**: Minimum version 3.30
- **C++ Compiler**: Clang (minimum 18.x.x) or GCC with C++20 support
- **LLVM Tools**: For code coverage analysis (Linux/macOS)
- **Build System**: 
  - Windows: Visual Studio 2022
  - macOS: Homebrew with LLVM tools
  - Linux: LLVM/GCC toolchain

## Building

### Using CMake Presets

```bash
# Configure
cmake --preset <preset-name>

# Build
cmake --build --preset <preset-name>

# Run Tests
ctest --preset <preset-name>
```

Available presets:
- `Windows-Debug` / `Windows-Release`
- `Linux-GCC-Debug` / `Linux-GCC-Release`
- `Linux-Clang-Debug` / `Linux-Clang-Release`

### Environment Variables

Set `SAMPLES_DIR` to point to the samples directory for test execution:
```bash
export SAMPLES_DIR=/path/to/sip2json-client/samples
```

## Test Coverage

The test suite includes 80+ test cases organized into the following categories:

### Core Parser Tests
- User agent header handling
- Meta element validation
- Error code serialization
- Call ID generation
- Time formatting (RFC1123, RFC3339, ISO8601)

### SIP Message Helpers
- Request message creation (REGISTER, INVITE, NOTIFY)
- Response message creation
- Message serialization and deserialization
- Header manipulation and validation
- Body/SDP content handling
- Content-Type validation

### Error Handling Tests
- Incomplete buffer detection (parse, header, content)
- Invalid document detection
- Unsupported content type handling
- Invalid start line detection
- Empty message handling

### Async Parsing Tests
- Asynchronous message parsing with callbacks
- Error callback handling
- Multiple message parsing from streams
- Exception handling in callbacks

### Validation Tests
- Extension-specific parsing (ARAS, Nelson)
- Invalid string position handling
- Complex SDP structures with multiple sessions

### Edge Cases
- Response status codes (200 OK, 401 Unauthorized, 100 Trying)
- REGISTER requests without body
- NOTIFY with multiple SDP sessions
- Empty header values
- Header arrays (Via headers)
- Preceding junk in message streams
- Both CRLF and LF line endings

## Sample Files

The `samples/` directory contains 30+ real-world SIP message samples for testing:

- **REGISTER**: Registration messages and responses
- **NOTIFY**: Notification messages with various SDP configurations
- **INVITE**: Invitation messages with status responses
- **Mixed Streams**: Multiple messages in a single stream
- **Edge Cases**: Malformed messages, incomplete buffers, invalid content types

## Performance & Benchmarks

Detailed performance benchmark results comparing `sip2json` across versions **v2.4.2**, **v2.5.7**, **v2.5.8**, and **current local workspace** under isolated single-threaded execution are available in the [Benchmark Report](BENCHMARK_REPORT.md).

### Summary Highlights

- **Peak Stream Throughput**: **v2.4.2** achieved peak stream parsing throughput at **1,376.18 msg/sec** (3.62 MB/sec, 726.65 µs avg latency).
- **Peak Single Message Throughput**: **v2.4.2** achieved **1,784.98 msg/sec** single-message parsing throughput (560.23 µs avg latency).
- **v2.5.x & Current Version Performance**: **v2.5.7**, **v2.5.8**, and **current local workspace** targets perform consistently (~1,207–1,242 msg/sec stream / ~1,600–1,625 msg/sec single), incorporating additional validation and safety checks.

## Dependencies

The project uses:
- **sip2json**: Core SIP parsing library (supports configurable versioning via `-DSIP2JSON_VERSION=<version>`)
- **nlohmann/json**: JSON manipulation
- **nlohmann/json**: JSON manipulation
- **Google Test** (v1.17.0): Testing framework

Dependencies are automatically fetched via CPM (C++ Package Manager).

## CI/CD

The project uses Azure Pipelines for continuous integration with:
- Multi-platform builds (Windows, Linux, macOS)
- Multiple compiler configurations (MSVC, GCC, Clang)
- Code coverage reporting (Linux/macOS with LLVM)
- Automated test execution and reporting

## Usage

### Basic Message Creation

```cpp
#include "siddiqsoft/sip2json.hpp"

// Create a REGISTER request
siddiqsoft::sipmessage registerMsg("REGISTER", "sip:example.com", 
                                   siddiqsoft::createCallId(), 1);
registerMsg.setHeader("To", "sip:user@example.com")
           .setHeader("Contact", "sip:user@example.com");

// Serialize to SIP format
auto sipBuffer = siddiqsoft::sip2json::serialize(registerMsg);
```

### Parsing SIP Messages

```cpp
// Parse from buffer
auto buffer = /* SIP message bytes */;
auto it = buffer.begin();
siddiqsoft::sipmessage parsedMsg = siddiqsoft::sip2json::parseFromBuffer(it, buffer.end());

// Access message properties
std::string method = parsedMsg.getMethod();
std::string callId = parsedMsg.getCallID();
int contentLength = parsedMsg.getContentLength();
```

### Async Parsing

```cpp
// Parse multiple messages with callbacks
siddiqsoft::sip2json::parseAsync(
    buffer,
    [](siddiqsoft::sipmessage&& msg) {
        // Handle successfully parsed message
        std::cout << "Parsed: " << msg.getMethod() << std::endl;
    },
    [](const siddiqsoft::sip2json_exception& e, auto&, auto&) {
        // Handle parsing error
        std::cerr << "Error: " << e.what() << std::endl;
    }
);
```

## Code Coverage

Coverage information is available for Linux and macOS builds using the LLVM toolchain.

### Generating Coverage Reports

```bash
# Generate HTML coverage report
llvm-cov show -format=html --ignore-filename-regex=tests/ \
    tests/sip2json_client_test --instr-profile=tests/merge.out \
    -o tests/results/coverage.info

# Generate LCOV format
llvm-cov export tests/sip2json_client_test \
    --instr-profile=tests/merge.out -format=lcov > tests/results/coverage.lcov

# Using gcovr
gcovr -e **/_deps/ -e tests/ --cobertura-pretty --cobertura coverage.xml
```

## License

Copyright 2024 Abdulkareem Siddiq. All rights reserved.

See LICENSE file for details.

<p align="right">
&copy; 2024 Abdulkareem Siddiq. All rights reserved.
</p>
