# Maintainer & Developer Handbook

Welcome to the **`sip2json` Maintainer & Developer Handbook**. This guide is designed for core developers, contributors, and maintainers taking ownership of the `sip2json` project.

---

## Repository Architecture & Directory Layout

`sip2json` is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor.

```
sip2json/
├── include/                     # Public and private header-only library implementation
│   └── siddiqsoft/
│       ├── sip2json.hpp         # Primary public header (parse, parseAsync, parseFromBuffer, serialize)
│       ├── sipmessage.hpp       # sipmessage class (subclasses nlohmann::json)
│       └── private/             # Zero-allocation internal helpers, serializers & CTRE regexes
│           ├── sip2json_header_keys.hpp      # 64-bit FNV-1a constexpr hash matching & jump table
│           ├── sip2json_parser.hpp           # Start-line & header parsing engine
│           ├── sip2json_serializer.hpp       # Wire format serialization engine
│           ├── sip2json_sdp.hpp              # SDP payload AST parser and serializer
│           ├── sip2json_exception.hpp        # Hierarchy of parser exceptions
│           ├── sip2json_response_codes.hpp   # RFC 3261 response codes & reason phrases
│           └── sip2json_utils.hpp            # In-register lowercasing, date/time, hex tools
├── tests/                       # Comprehensive test suites (321 tests, 100% passing)
│   ├── compliance/              # RFC 3261, RFC 4475 (50 torture cases), SDP RFC 4566/8866 suites
│   ├── regression/              # Production defect regressions & stream re-assembly tests
│   ├── coverage/                # Branch & statement coverage tests
│   ├── security/                # CRLF injection, integer overflow, memory safety tests
│   ├── vulnerability/           # CVE & fuzzing attack simulation tests
│   ├── validation/              # Real-world stream fixtures (36 files, ~1.5 MB)
│   └── benchmark/               # C++ benchmark harnesses (single-thread & Google Benchmark)
├── docs/                        # MkDocs documentation source (Material theme)
│   ├── hooks.py                 # MkDocs build hook (version resolution & dynamic doc generation)
│   └── ...                      # Markdown guides, architecture studies, and API references
├── scripts/                     # Maintainer automation scripts
│   ├── prep_windows_machine.ps1 # Administrator setup for Windows (Registry long paths, Git config)
│   ├── init_msvc_env.bat        # Command-line MSVC developer environment initialization
│   ├── init_msvc_env.ps1        # PowerShell MSVC environment initialization
│   ├── publish_benchmarks.py    # Cross-platform CI benchmark aggregator and markdown injector
│   └── generate_dependencies_md.py # Dynamic CMake dependency diagram generator
├── .azure/                      # Azure DevOps CI/CD pipeline stage and job templates
│   ├── az-build-linux.yml       # Linux CI build matrix (GCC & Clang, x64 & arm64)
│   ├── az-build-windows.yml     # Windows CI build matrix (MSVC, x64 & arm64)
│   ├── az-publish-docs.yml      # MkDocs documentation builder & GitHub Pages publisher
│   └── az-publish-github.yml    # GitHub Releases publisher
├── azure-pipelines.yml          # Main Azure Pipelines orchestration definition
├── CMakeLists.txt               # Main CMake build definition (CPM package management)
├── CMakePresets.json            # Cross-platform build and configure presets
├── GitVersion.yml               # Semantic version calculation configuration
└── mkdocs.yml                   # MkDocs Material theme configuration and navigation
```

---

## Maintainer Guide Index

Navigate the dedicated maintainer guides:

1. [**Developer Environment Setup**](setup.md): Step-by-step toolchain and prerequisite configuration for macOS (Apple Silicon & Intel), Linux (Ubuntu / Debian / RHEL), and Windows (MSVC 2022 + long paths).
2. [**Building, Testing & Benchmarking**](building.md): Using CMake Presets, executing the 321-test CTest suite, running performance benchmark harnesses, and diagnosing memory with AddressSanitizer.
3. [**Documentation & MkDocs**](documentation.md): Local preview server (`mkdocs serve`), strict verification builds, Python virtual environment, and dynamic build hooks (`docs/hooks.py`).
4. [**Azure Pipelines CI/CD & Secrets**](pipelines.md): Self-hosted agent requirements, multi-platform build matrix, release gates, GitHub Pages deployment, and required Azure DevOps secrets (`GITHUB_TOKEN`, `GITHUB_USER`).
