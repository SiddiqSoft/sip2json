# Maintainer & Developer Handbook

Welcome to the **`sip2json` Maintainer & Developer Handbook**. This guide is designed for core developers, contributors, and maintainers taking ownership of the `sip2json` project.

---

## Repository Architecture & Directory Layout

`sip2json` is a header-only Modern C++23 SIP protocol parser and serializer library designed with `nlohmann::json` as a first-class API metaphor.

| Directory / File | Scope & Type | Purpose & Key Artifacts |
| :--- | :--- | :--- |
| **`include/siddiqsoft/`** | Public API Headers | Primary public interface: [`sip2json.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/sip2json.hpp) (parser & serializer entry points) and [`sipmessage.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/sipmessage.hpp) (`nlohmann::json` subclass). |
| **`include/siddiqsoft/private/`** | Internal Engine | High-performance header parsing ([`sip2json_parser.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/private/sip2json_parser.hpp)), constexpr 64-bit FNV-1a matching ([`sip2json_header_keys.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/private/sip2json_header_keys.hpp)), SDP parser ([`sip2json_sdp.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/private/sip2json_sdp.hpp)), wire serializer ([`sip2json_serializer.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/private/sip2json_serializer.hpp)), exceptions ([`sip2json_exception.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/private/sip2json_exception.hpp)), and response codes ([`sip2json_response_codes.hpp`](file:///Users/maas/source/repos/siddiqsoft/sip2json/include/siddiqsoft/private/sip2json_response_codes.hpp)). |
| **`tests/`** | Test Suites (324 Tests) | Categorized testing architecture: `compliance/` (RFC 3261 & RFC 4475), `regression/` (bug fixes & memory safety), `coverage/` (statement & branch coverage), `security/` (CRLF injection & memory safety), `vulnerability/` (fuzzing & overflow resilience), `validation/` (36 sample stream captures), and `benchmark/` (performance harnesses). |
| **`docs/`** | Documentation Source | MkDocs Material site source, theme overrides, API references, architecture deep-dives, and [`docs/hooks.py`](file:///Users/maas/source/repos/siddiqsoft/sip2json/docs/hooks.py) build lifecycle hooks. |
| **`scripts/`** | Developer Automation | Cross-platform build helpers: [`publish_benchmarks.py`](file:///Users/maas/source/repos/siddiqsoft/sip2json/scripts/publish_benchmarks.py) (matrix aggregator), [`generate_dependencies_md.py`](file:///Users/maas/source/repos/siddiqsoft/sip2json/scripts/generate_dependencies_md.py), [`prep_windows_machine.ps1`](file:///Users/maas/source/repos/siddiqsoft/sip2json/scripts/prep_windows_machine.ps1), and [`init_msvc_env.ps1`](file:///Users/maas/source/repos/siddiqsoft/sip2json/scripts/init_msvc_env.ps1). |
| **`.azure/`** | CI/CD Pipelines | Modular Azure DevOps pipeline definitions: [`az-build-linux.yml`](file:///Users/maas/source/repos/siddiqsoft/sip2json/.azure/az-build-linux.yml), [`az-build-windows.yml`](file:///Users/maas/source/repos/siddiqsoft/sip2json/.azure/az-build-windows.yml), [`az-publish-docs.yml`](file:///Users/maas/source/repos/siddiqsoft/sip2json/.azure/az-publish-docs.yml), and [`az-publish-github.yml`](file:///Users/maas/source/repos/siddiqsoft/sip2json/.azure/az-publish-github.yml). |
| **`CMakeLists.txt`** | Build Definition | CMake 3.31+ interface library target definition with CPM dependency management. |
| **`CMakePresets.json`** | Build Presets | Cross-platform configure, build, and test presets for macOS (Xcode/LLVM), Linux (GCC/Clang), and Windows (MSVC x64/ARM64). |
| **`GitVersion.yml`** | Versioning Policy | Semantic version calculation rules based on Git commit history and release branch names. |
| **`mkdocs.yml`** | Site Configuration | Material for MkDocs navigation hierarchy, theme extensions, search, and integrated Table of Contents (`toc.integrate`). |

---

## Maintainer Guide Index

Navigate the dedicated maintainer guides:

1. [**Developer Environment Setup**](setup.md): Step-by-step toolchain and prerequisite configuration for macOS (Apple Silicon & Intel), Linux (Red Hat 10.2 / Fedora / Debian), and Windows (MSVC 2022 + long paths).
2. [**Building, Testing & Benchmarking**](building.md): Using CMake Presets, executing the 324-test CTest suite, running performance benchmark harnesses, and diagnosing memory with AddressSanitizer.
3. [**Documentation & MkDocs**](documentation.md): Local preview server (`mkdocs serve`), strict verification builds, Python virtual environment, and dynamic build hooks (`docs/hooks.py`).
4. [**Azure Pipelines CI/CD & Secrets**](pipelines.md): Self-hosted agent requirements, multi-platform build matrix, release gates, GitHub Pages deployment, and required Azure DevOps secrets (`GITHUB_TOKEN`, `GITHUB_USER`).
