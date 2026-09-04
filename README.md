# sip2json: A Focused SIP Parser for Modern C++

<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/siddiqsoftware.sip2json?branchName=master)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![NuGet Version](https://img.shields.io/nuget/v/SiddiqSoft.sip2json?logo=nuget)](https://www.nuget.org/packages/SiddiqSoft.sip2json/)
[![NuGet Downloads](https://img.shields.io/nuget/dt/SiddiqSoft.sip2json?logo=nuget)](https://www.nuget.org/packages/SiddiqSoft.sip2json/)
[![Tests](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/21/master.svg)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=21&branchName=master)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/20)
[![License BSD-3](https://img.shields.io/badge/License-BSD--3--Clause-blue)](LICENSE)

[![IETF RFC 3261](https://img.shields.io/badge/IETF-RFC%203261%20(SIP)-003366)](https://datatracker.ietf.org/doc/html/rfc3261)
[![IETF RFC 4475](https://img.shields.io/badge/IETF-RFC%204475%20(Torture%2050/50)-2ea44f)](https://datatracker.ietf.org/doc/html/rfc4475)
[![IETF RFC 8866](https://img.shields.io/badge/IETF-RFC%208866%20(SDP)-003366)](https://datatracker.ietf.org/doc/html/rfc8866)
[![W3C WebRTC SDP](https://img.shields.io/badge/W3C-WebRTC%20SDP-1488C6)](https://datatracker.ietf.org/doc/html/rfc8829)
<!-- end badges -->

**`sip2json`** is a header-only Modern C++20 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor for seamlessly converting SIP protocol messages to/from JSON for NoSQL databases and distributed event processing.

---

## Documentation

**[siddiqsoft.github.io/sip2json](https://siddiqsoft.github.io/sip2json/)**

* [**Quick Start & Integration**](https://siddiqsoft.github.io/sip2json/integration/)
* [**Asynchronous Stream Parsing**](https://siddiqsoft.github.io/sip2json/features/async/)
* [**Performance & Benchmarks**](https://siddiqsoft.github.io/sip2json/features/benchmarks/)
* [**JSON Schema Metaphor**](https://siddiqsoft.github.io/sip2json/features/json_schema/)
* [**API Reference**](https://siddiqsoft.github.io/sip2json/api/)

---

## Quick Example

=== "Stream Parsing"

    ```cpp
    #include <iostream>
    #include "siddiqsoft/sip2json.hpp"

    using namespace siddiqsoft;

    void onNetworkDataReceived(std::string& tcpReadBuffer)
    {
        // Asynchronously parse multiple SIP frames from buffer (erasing consumed bytes)
        sip2json::parseAsync(
            tcpReadBuffer,
            [](sipmessage&& msg) {
                std::cout << "Parsed " << msg.getMethod() << " Call-ID: " << msg.getCallID() << "\n";
                nlohmann::json doc = msg; // First-class JSON metaphor
            },
            [](const sip2json_exception& ex, auto& start, const auto& end) {
                std::cerr << "Parser warning: " << ex.what() << "\n";
            }
        );
    }
    ```

=== "Push to RabbitMQ"

    ```cpp
    #include "siddiqsoft/sip2json.hpp"
    #include <SimpleAmqpClient/SimpleAmqpClient.h>

    using namespace siddiqsoft;

    // Stream incoming SIP frames directly into a RabbitMQ exchange as JSON
    auto channel = AmqpClient::Channel::Create("localhost");

    sip2json::parseAsync(tcpReadBuffer, [&](sipmessage&& msg) {
        nlohmann::json doc = msg;
        auto body = AmqpClient::BasicMessage::Create(doc.dump());
        channel->BasicPublish("sip_events", std::string(msg.getMethod()), body);
    });
    ```

=== "Log to DuckDB"

    ```cpp
    #include "siddiqsoft/sip2json.hpp"
    #include <duckdb.hpp>

    using namespace siddiqsoft;

    // Stream incoming SIP traffic directly into DuckDB for columnar analytics
    duckdb::DuckDB db("sip_analytics.db");
    duckdb::Connection con(db);
    con.Query("CREATE TABLE IF NOT EXISTS sip_traffic (method VARCHAR, call_id VARCHAR, payload JSON);");

    duckdb::Appender appender(con, "sip_traffic");
    sip2json::parseAsync(tcpReadBuffer, [&](sipmessage&& msg) {
        nlohmann::json doc = msg;
        appender.AppendRow(std::string(msg.getMethod()), std::string(msg.getCallID()), doc.dump());
    });
    appender.Flush();
    ```
    !!! note "NOTE"
        Use threadpool or async versions of specific SDK to maximize performance.
---


## Standards Compliance

`sip2json` is verified against official IETF specifications across dedicated automated test suites:

- **[RFC 3261](https://datatracker.ietf.org/doc/html/rfc3261)** (Core SIP): [`tests/compliance/src/rfc3261_compliance_tests.cpp`](tests/compliance/src/rfc3261_compliance_tests.cpp)
- **[RFC 4475](https://datatracker.ietf.org/doc/html/rfc4475)** (Torture Test Suite — 50/50): [`tests/compliance/src/rfc4475_torture_tests.cpp`](tests/compliance/src/rfc4475_torture_tests.cpp)
- **[RFC 3262](https://datatracker.ietf.org/doc/html/rfc3262)**, **[RFC 3515](https://datatracker.ietf.org/doc/html/rfc3515)**, **[RFC 3903](https://datatracker.ietf.org/doc/html/rfc3903)**, **[RFC 6665](https://datatracker.ietf.org/doc/html/rfc6665)** (SIP Extensions): [`tests/compliance/src/sip_certification_suite.cpp`](tests/compliance/src/sip_certification_suite.cpp)
- **[RFC 8866](https://datatracker.ietf.org/doc/html/rfc8866)** / **[RFC 4566](https://datatracker.ietf.org/doc/html/rfc4566)**, **[RFC 3264](https://datatracker.ietf.org/doc/html/rfc3264)**, **[RFC 8829](https://datatracker.ietf.org/doc/html/rfc8829)** / **[RFC 8839](https://datatracker.ietf.org/doc/html/rfc8839)** (SDP & WebRTC): [`tests/compliance/src/sdp_compliance_tests.cpp`](tests/compliance/src/sdp_compliance_tests.cpp)

For full coverage matrices, section mappings, and torture test details, see the [**Standards Compliance Guide**](https://siddiqsoft.github.io/sip2json/architecture/compliance/) on our documentation site.

!!! note "Performance"
    Up to **~40,000 msg/sec** parsing throughput with sub-microsecond latency (+97.5% gain over v2.x) — see our [**Performance & Benchmarks Guide**](https://siddiqsoft.github.io/sip2json/architecture/benchmarks/).

---

## License

Licensed under the [BSD 3-Clause License](LICENSE).