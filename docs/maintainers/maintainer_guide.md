# Maintainer Guide

Codebase architecture, development guidelines, formatting standards, and maintainer documentation index for `siddiqsoft::sip2json`.

---

## Documentation Index

The maintainer documentation is organized into modular topic guides:

| Topic Guide | Description |
| :--- | :--- |
| [**CI/CD Pipelines**](pipelines.md) | Azure Pipelines architecture, build matrix, platform triggers, and parameters |
| [**CMake Presets**](cmake_presets.md) | Decoupled presets hierarchy, `project-base.json`, and preset reference |
| [**Development Workflow**](workflow.md) | Local building, testing, standalone validation subproject, and macOS toolchain |
| [**Build Agent Requirements**](build_agents.md) | Prerequisites and configuration for macOS, Linux, and Windows self-hosted agents |
| [**Release & Publication**](releases.md) | GitVersion, SemVer tagging, GitHub Releases, and NuGet package publishing |
| [**Documentation Architecture**](documentation.md) | MkDocs Material, Doxygen XML, custom CSS tokens, hooks, and local preview |

---

## Codebase Architecture & UML Class Diagram

<!-- UML_CLASS_DIAGRAM_START -->
The following UML class diagram illustrates the primary classes, relationships, and exception hierarchy in `siddiqsoft::sip2json`. The diagram is auto-generated from the C++ source AST via Doxygen XML. Each node in the diagram links directly to its source header file on GitHub.

```mermaid
classDiagram
    direction TB

    classDef coreClass fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;
    classDef utilityClass fill:rgba(15,118,110,0.08),stroke:#0f766e,stroke-width:2px;
    classDef exceptionClass fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;
    classDef enumClass fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;
    classDef externalClass fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;
    classDef highlightClass fill:rgba(2,132,199,0.18),stroke:#0284c7,stroke-width:3px;

    class json["nlohmann::json"] {
        <<external DOM>>
    }
    class json:::externalClass

    class runtime_error["std::runtime_error"] {
        <<external exception>>
    }
    class runtime_error:::externalClass

    class sip2json["siddiqsoft::sip2json"] {
        <<final utility>>
        +parseAsync(string_view& buffer, onMsg, onErr)$ size_t
        +parse(string_view& buffer)$ vector~sipmessage~
        +parseFromBuffer(string_view& buffer)$ sipmessage
        +serialize(sipmessage& msg)$ string
    }
    class sip2json:::utilityClass

    class sipmessage["siddiqsoft::sipmessage"] {
        +sipmessage()
        +sipmessage(string_view method, string_view uri, string_view callId, uint32_t cseq)
        +sipmessage(const json& src)
        +getMethodView() string_view
        +getUriView() string_view
        +getCallIDView() string_view
        +getStatusCode() uint32_t
        +getReasonView() string_view
        +getHeader(string_view key) string
        +setHeader(string_view key, string_view val) sipmessage&
        +hasHeader(string_view key) bool
        +getContentTypeView() string_view
    }
    class sipmessage:::coreClass

    class HeaderKeySet["siddiqsoft::HeaderKeySet"] {
        +string_view canonicalKey
        +string_view canonicalUpper
        +char compactAlias
        +uint64_t hash
        +HeaderKeySet(string_view key)
        +canonical() string_view
        +lower() string_view
    }
    class HeaderKeySet:::coreClass

    class SIPMessageType["siddiqsoft::SIPMessageType"] {
        <<enumeration>>
        Request = 1
        Response = 2
    }
    class SIPMessageType:::enumClass

    class sip2jsonErrors["siddiqsoft::sip2jsonErrors"] {
        <<enumeration>>
        unknown = -1
        success = 0
        invalid_document = 1
        empty_message = 2
        invalid_startline = 3
        incomplete_buffer_for_header = 4
        incomplete_buffer_for_content = 5
        incomplete_buffer_for_parse = 6
        missing_required_element = 7
        unsupported_contenttype = 8
    }
    class sip2jsonErrors:::enumClass

    class sip2json_exception["siddiqsoft::sip2json_exception"] {
        +sip2jsonErrors errCode
        +sip2json_exception(string message, sip2jsonErrors code)
        +what() const char*
    }
    class sip2json_exception:::exceptionClass

    class empty_message_error["siddiqsoft::empty_message_error"]
    class empty_message_error:::exceptionClass
    class incomplete_buffer_for_content_error["siddiqsoft::incomplete_buffer_for_content_error"]
    class incomplete_buffer_for_content_error:::exceptionClass
    class incomplete_buffer_for_header_error["siddiqsoft::incomplete_buffer_for_header_error"]
    class incomplete_buffer_for_header_error:::exceptionClass
    class incomplete_buffer_for_parse_error["siddiqsoft::incomplete_buffer_for_parse_error"]
    class incomplete_buffer_for_parse_error:::exceptionClass
    class invalid_document_error["siddiqsoft::invalid_document_error"]
    class invalid_document_error:::exceptionClass
    class invalid_startline_error["siddiqsoft::invalid_startline_error"]
    class invalid_startline_error:::exceptionClass
    class missing_required_element["siddiqsoft::missing_required_element"]
    class missing_required_element:::exceptionClass
    class unsupported_contenttype_error["siddiqsoft::unsupported_contenttype_error"]
    class unsupported_contenttype_error:::exceptionClass

    json <|-- sipmessage : public inheritance
    runtime_error <|-- sip2json_exception : public inheritance
    sip2json_exception <|-- empty_message_error
    sip2json_exception <|-- incomplete_buffer_for_content_error
    sip2json_exception <|-- incomplete_buffer_for_header_error
    sip2json_exception <|-- incomplete_buffer_for_parse_error
    sip2json_exception <|-- invalid_document_error
    sip2json_exception <|-- invalid_startline_error
    sip2json_exception <|-- missing_required_element
    sip2json_exception <|-- unsupported_contenttype_error

    sip2json ..> sipmessage : produces / consumes
    sip2json ..> sip2json_exception : throws
    sipmessage ..> SIPMessageType : classifies
    sipmessage ..> HeaderKeySet : uses
    sip2json_exception ..> sip2jsonErrors : contains

    link sip2json "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sip2json.hpp" "Source: include/siddiqsoft/sip2json.hpp"
    link sipmessage "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sipmessage.hpp" "Source: include/siddiqsoft/sipmessage.hpp"
    link HeaderKeySet "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_header_keys.hpp" "Source: include/siddiqsoft/private/sip2json_header_keys.hpp"
    link SIPMessageType "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sipmessage.hpp" "Source: include/siddiqsoft/sipmessage.hpp"
    link sip2jsonErrors "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link sip2json_exception "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link empty_message_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link incomplete_buffer_for_content_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link incomplete_buffer_for_header_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link incomplete_buffer_for_parse_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link invalid_document_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link invalid_startline_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link missing_required_element "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link unsupported_contenttype_error "https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp" "Source: include/siddiqsoft/private/sip2json_exception.hpp"
    link json "https://github.com/nlohmann/json" "External: nlohmann/json"
    link runtime_error "https://en.cppreference.com/w/cpp/error/runtime_error" "Standard Library: std::runtime_error"
```

### Source Code Mapping

| Component / Class | Header File | Source Link | Purpose & Architectural Role |
| :--- | :--- | :--- | :--- |
| [`siddiqsoft::sip2json`](../api/sip2json.md) | `include/siddiqsoft/sip2json.hpp` | [`sip2json.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sip2json.hpp) | Top-level static parser, stream deserializer, and wire serializer utility |
| [`siddiqsoft::sipmessage`](../api/sipmessage.md) | `include/siddiqsoft/sipmessage.hpp` | [`sipmessage.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sipmessage.hpp) | Core message container inheriting from `nlohmann::json` with zero-copy view accessors |
| [`siddiqsoft::HeaderKeySet`](../api/constants.md) | `include/siddiqsoft/private/sip2json_header_keys.hpp` | [`sip2json_header_keys.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_header_keys.hpp) | Canonical SIP header normalization, compact alias mapping, and compile-time hashing |
| [`siddiqsoft::SIPMessageType`](../api/sipmessage.md) | `include/siddiqsoft/sipmessage.hpp` | [`sipmessage.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/sipmessage.hpp) | Protocol message discriminator (`Request = 1`, `Response = 2`) |
| [`siddiqsoft::sip2json_exception`](../api/errors.md#exception-class-hierarchy) | `include/siddiqsoft/private/sip2json_exception.hpp` | [`sip2json_exception.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp) | Base exception class inheriting from `std::runtime_error` with `sip2jsonErrors` payload |
| [`siddiqsoft::sip2jsonErrors`](../api/errors.md#sip2jsonerrors-enumeration) | `include/siddiqsoft/private/sip2json_exception.hpp` | [`sip2json_exception.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp) | Diagnostic error code enumeration for parser and syntax failures |
| Derived Exceptions | `include/siddiqsoft/private/sip2json_exception.hpp` | [`sip2json_exception.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_exception.hpp) | Specialized exception hierarchy (`invalid_document_error`, `empty_message_error`, etc.) |
| Parser Engine (`raw_view`) | `include/siddiqsoft/private/sip2json_parser.hpp` | [`sip2json_parser.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_parser.hpp) | High-throughput streaming parser, zero-copy buffer slicing, CRLF boundary scanning |
| Wire Serializer | `include/siddiqsoft/private/sip2json_serializer.hpp` | [`sip2json_serializer.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_serializer.hpp) | RFC 3261 compliant text wire serializer |
| SDP Body Parser | `include/siddiqsoft/private/sip2json_sdp.hpp` | [`sip2json_sdp.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_sdp.hpp) | RFC 4566 Session Description Protocol parser and structured JSON serialization |
| Response Codes | `include/siddiqsoft/private/sip2json_response_codes.hpp` | [`sip2json_response_codes.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_response_codes.hpp) | SIP status codes, reason phrases, and classification ranges (1xx-6xx) |
| Protocol Constants | `include/siddiqsoft/private/sip2json_constants.hpp` | [`sip2json_constants.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_constants.hpp) | SIP grammar tokens, method strings, whitespace matchers, CRLF constants |
| DateTime Parser | `include/siddiqsoft/private/sip2json_datetime.hpp` | [`sip2json_datetime.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_datetime.hpp) | RFC 3261 / RFC 1123 HTTP-date timestamp parser and serializer |
| Utility Functions | `include/siddiqsoft/private/sip2json_utils.hpp` | [`sip2json_utils.hpp`](https://github.com/SiddiqSoft/sip2json/blob/master/include/siddiqsoft/private/sip2json_utils.hpp) | Internal whitespace trimming, view slicing, string conversion utilities |
<!-- UML_CLASS_DIAGRAM_END -->

---

## Source Code Formatting (Clang-Format)

All C++ source code (`include/`, `tests/`, and `benchmarks/`) adheres to the formatting rules defined in [`.clang-format`](https://github.com/SiddiqSoft/sip2json/blob/master/.clang-format) at the repository root.

The configuration is based on the **WebKit** style with modern C++20 conventions:
* **Column Limit**: 132 characters
* **Indentation**: 4 spaces (tabs are never used)
* **Brace Style**: WebKit (braces break before functions, classes, and catch/else blocks)
* **Pointer Alignment**: Left (`const sipmessage& msg`, `std::string_view* ptr`)
* **Standard**: C++20

---

### How to Bulk Clang-Format the Source Code

Maintainers have multiple ways to format the entire codebase in bulk:

#### Option 1: Via CMake Build Target (Recommended)

When configured locally, CMake generates a dedicated `format` (and `clang-format`) target:

```bash
# Format using an active preset build directory:
cmake --build --preset Apple-Clang-Debug --target format

# Or format directly against any existing build folder:
cmake --build build/Apple-Clang-Debug --target format
```

> [!TIP]
> **Automatic Debug Formatting**: On non-CI local builds, CMake enables `sip2json_ENABLE_CLANG_FORMAT=ON` by default in `Debug` configuration. Formatting runs automatically prior to every local Debug compilation whenever a compatible `clang-format` executable is detected.

---

#### Option 2: Bulk Command-Line via Find (macOS & Linux)

To reformat all C++ header and source files across `include/`, `tests/`, and `benchmarks/` in one command:

```bash
find include tests benchmarks -type f \( -name "*.hpp" -o -name "*.cpp" -o -name "*.h" \) -exec clang-format -i --style=file {} +
```

To verify formatting without modifying files, pass `--dry-run --Werror`:

```bash
find include tests benchmarks -type f \( -name "*.hpp" -o -name "*.cpp" -o -name "*.h" \) -exec clang-format --dry-run --Werror --style=file {} +
```

---

#### Option 3: Bulk Command-Line via Git (Cross-Platform)

Format all tracked C++ files in the repository using `git ls-files`:

=== "macOS & Linux (bash / zsh)"
    ```bash
    git ls-files '*.hpp' '*.cpp' '*.h' | xargs clang-format -i --style=file
    ```

=== "Windows (PowerShell)"
    ```powershell
    git ls-files '*.hpp', '*.cpp', '*.h' | ForEach-Object { clang-format -i --style=file $_ }
    ```

---

#### Option 4: Bulk PowerShell Script (Windows)

On Windows systems without Git bash, run the following PowerShell one-liner:

```powershell
Get-ChildItem -Path include, tests, benchmarks -Include *.hpp, *.cpp, *.h -Recurse | ForEach-Object {
    clang-format -i --style=file $_.FullName
}
```

---

#### Option 5: Editor & IDE Integration

* **Visual Studio 2022**: Visual Studio automatically detects `.clang-format` at the repository root. Press `Ctrl+K, Ctrl+D` to format the active document, or `Ctrl+K, Ctrl+F` to format a selection.
* **Visual Studio Code**: Ensure the [C/C++ Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) is installed. In `.vscode/settings.json`:
  ```json
  {
    "C_Cpp.clang_format_style": "file",
    "editor.formatOnSave": true
  }
  ```
* **CLion**: Navigate to **Settings > Editor > Code Style > C/C++** and verify **Enable ClangFormat** is checked. Press `Ctrl+Alt+L` (`Cmd+Alt+L` on macOS) to format.

---

## Maintainer Pre-Flight Checklist

Before submitting a pull request or pushing to `master`:

1. **Format Code**: Run the bulk clang-format command or `cmake --build <preset> --target format`.
2. **Build Cleanly**: Compile with zero compiler warnings using your platform preset:
   ```bash
   cmake --build --preset <preset-name>
   ```
3. **Execute All Tests**: Ensure 100% of tests pass:
   ```bash
   ctest --preset <preset-name> --output-on-failure
   ```
4. **Validate Documentation**: Verify zero broken links or markdown syntax errors:
   ```bash
   mkdocs build --strict
   ```
5. **Major Version Updates**: When introducing breaking API changes or preparing a major version release, manually update the `next-version:` entry in [`GitVersion.yml`](https://github.com/SiddiqSoft/sip2json/blob/master/GitVersion.yml) (e.g. `next-version: 4.0.0`) so GitVersion establishes the new major version baseline.
