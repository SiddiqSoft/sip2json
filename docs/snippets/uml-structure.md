```mermaid
flowchart TD
    subgraph PublicAPI["Public API Layer (include/siddiqsoft/)"]
        S2J["siddiqsoft::sip2json<br/><i>Static Parser & Serializer Facade</i>"]
        SMSG["siddiqsoft::sipmessage<br/><i>SIP Message Container DTO</i>"]
    end

    subgraph InternalEngines["Parser & Serializer Engines (private/)"]
        Parser["sip2json_parser<br/><i>Zero-Copy Token & Delimiter Scanner</i>"]
        Serial["sip2json_serializer<br/><i>RFC 3261 Wire Serializer</i>"]
        SDP["sip2json_sdp<br/><i>RFC 4566 / 8866 SDP Body Engine</i>"]
    end

    subgraph ProtocolDict["Dictionaries & Token Tables (private/)"]
        HKS["sip2json_header_keys<br/><i>64-bit constexpr FNV-1a Dispatch</i>"]
        Const["sip2json_constants<br/><i>Method Literals & Delimiters</i>"]
        Resp["sip2json_response_codes<br/><i>Numeric Status Code Tables</i>"]
        DT["sip2json_datetime<br/><i>ISO 8601 & RFC 1123 Generators</i>"]
    end

    subgraph Diagnostics["Diagnostics & Utilities (private/)"]
        Exc["sip2json_exception<br/><i>Diagnostics & sip2jsonErrors</i>"]
        Utils["sip2json_utils<br/><i>String View & Memory Utilities</i>"]
    end

    subgraph External["External Base DOM"]
        JSON["nlohmann::json<br/><i>JSON Document Base Class</i>"]
        StdErr["std::runtime_error<br/><i>C++ Standard Exception</i>"]
    end

    S2J -->|produces / consumes| SMSG
    SMSG -->|inherits| JSON
    Exc -->|inherits| StdErr

    S2J -->|delegates streaming| Parser
    S2J -->|delegates serialization| Serial
    Parser -->|delegates body payload| SDP
    Serial -->|formats payload| SDP
    Parser -->|canonical lookup| HKS
    Parser -->|scans delimiters| Utils
    Parser -->|throws framing errors| Exc
    SMSG -->|queries aliases| HKS
    SMSG -->|references codes| Resp
    SMSG -->|timestamps| DT
    Serial -->|delimiters & tokens| Const

    classDef apiLayer fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;
    classDef engineLayer fill:rgba(15,118,110,0.08),stroke:#0f766e,stroke-width:2px;
    classDef dictLayer fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;
    classDef diagLayer fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;
    classDef extLayer fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;

    class S2J,SMSG apiLayer;
    class Parser,Serial,SDP engineLayer;
    class HKS,Const,Resp,DT dictLayer;
    class Exc,Utils diagLayer;
    class JSON,StdErr extLayer;

    click S2J "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sip2json.hpp" "Source: sip2json.hpp"
    click SMSG "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sipmessage.hpp" "Source: sipmessage.hpp"
    click Parser "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_parser.hpp" "Source: sip2json_parser.hpp"
    click Serial "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_serializer.hpp" "Source: sip2json_serializer.hpp"
    click SDP "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_sdp.hpp" "Source: sip2json_sdp.hpp"
    click HKS "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_header_keys.hpp" "Source: sip2json_header_keys.hpp"
    click Exc "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: sip2json_exception.hpp"
```
