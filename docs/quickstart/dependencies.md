# Project Dependencies

This document is automatically generated from `CMakeLists.txt` files for `sip2json`.

## Dependency Diagram

```mermaid
graph TD
    sip2json["sip2json::sip2json"]

    subgraph Core["Core Dependencies (via CPM)"]
        NLOHMANNJSON["nlohmann_json v3.12.0"]
    end

    sip2json --> NLOHMANNJSON
```

## Dependency Breakdown

| Dependency | Repository / Target | Version | Type | Scope / Platform |
| :--- | :--- | :--- | :--- | :--- |
| **nlohmann_json** | [`nlohmann/json`](https://github.com/nlohmann/json) | v3.12.0 | `CPM` | All Platforms (`INTERFACE`) |
