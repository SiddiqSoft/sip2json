# Project Dependencies & Architecture Hierarchy

`sip2json` is a lightweight, zero-binary-bloat, **header-only Modern C++23 library**. Dependencies are managed declaratively via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) and cached automatically across local and CI builds.

## Dependency Architecture Diagram

```mermaid
graph TD
    App["🚀 <b>Client Application / Service</b><br/><i>(VoIP Proxy, WebRTC Gateway, Media Server, Analytics)</i>"]:::appClass

    sip2json["📦 <b>sip2json::sip2json</b><br/><i>Modern C++23 Header-Only SIP Parser & Serializer</i>"]:::projectClass

    subgraph CoreGroup ["⚡ Core Header-Only"]
        NLOHMANNJSON["<b>nlohmann_json</b> <code>v3.12.0</code><br/><i>JSON Model & Deserialization</i>"]:::coreClass1
        CTRE["<b>ctre</b> <code>v3.11.0</code><br/><i>Compile-Time Regular Expressions</i>"]:::coreClass2
    end

    App -->|"<code>#include &lt;siddiqsoft/sip2json.hpp&gt;</code>"| sip2json
    sip2json -->|"<code>INTERFACE link</code>"| NLOHMANNJSON
    sip2json -->|"<code>INTERFACE link</code>"| CTRE

    classDef appClass fill:#2E7D32,stroke:#1B5E20,stroke-width:2px,color:#FFFFFF,font-weight:bold;
    classDef projectClass fill:#1565C0,stroke:#0D47A1,stroke-width:3px,color:#FFFFFF,font-weight:bold;
    classDef coreClass1 fill:#6A1B9A,stroke:#4A148C,stroke-width:2px,color:#FFFFFF;
    classDef coreClass2 fill:#00695C,stroke:#004D40,stroke-width:2px,color:#FFFFFF;
    classDef platformClass fill:#0277BD,stroke:#01579B,stroke-width:2px,color:#FFFFFF;
    classDef testClass fill:#E65100,stroke:#BF360C,stroke-width:2px,color:#FFFFFF;
```

---

## Detailed Dependency Breakdown

As of Version `{ version }`

| Dependency | Version | CPM Scope / Target | Description |
| :--------- | :-----: | :------------- | :--- |
| [**nlohmann_json**](https://github.com/nlohmann/json) | `v3.12.0` | All Platforms (`INTERFACE`) | First-class JSON object model and DOM serialization |
| [**ctre**](https://github.com/hanickadot/compile-time-regular-expressions) | `v3.11.0` | All Platforms (`INTERFACE`) | Fast compile-time regular expression evaluation engine |
