#!/usr/bin/env python3
"""
generate_dependencies_md.py

Parses CMakeLists.txt files in the repository to extract project dependencies
and generates dependencies.md with a Mermaid dependency graph and breakdown table.
"""

import os
import re
import sys
import argparse
from pathlib import Path

# Friendly name overrides for known repositories/packages
DISPLAY_NAMES = {
    "compile-time-regular-expressions": "ctre",
    "json": "nlohmann_json",
    "googletest": "gtest",
}


def parse_cmake_file(filepath):
    """
    Parses a CMakeLists.txt file to extract find_package and cpmaddpackage dependencies.
    """
    filepath = Path(filepath)
    if not filepath.exists():
        return []

    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()

    dependencies = []
    lines = content.splitlines()
    current_scope = "Core"
    is_test_file = "tests" in str(filepath)

    for line in lines:
        stripped = line.strip()

        # Update scope tracking based on CMake conditional blocks
        if "MATCHES [Mm][Ss][Vv][Cc]" in stripped:
            current_scope = "Windows"
        elif (
            "MATCHES [Cc][Ll][Aa][Nn][Gg]" in stripped
            or "MATCHES [Gg][Nn][Uu]" in stripped
            or "MATCHES [Aa][Pp][Pp][Ll][Ee]" in stripped
        ):
            current_scope = "Linux/macOS"
        elif stripped.startswith("endif()") or stripped.startswith("elseif("):
            if "MATCHES" not in stripped:
                current_scope = "Core"

        scope = "Test" if is_test_file else current_scope

        # Match cpmaddpackage("gh:owner/repo#version") or CPMAddPackage(...)
        cpm_match = re.search(
            r'(?:cpmaddpackage|CPMAddPackage)\s*\(\s*["\'](?:gh:)?([^"\'#]+)#([^"\'\)]+)["\']\s*\)',
            stripped,
            re.IGNORECASE,
        )
        if cpm_match:
            repo_spec = cpm_match.group(1).strip()
            raw_version = cpm_match.group(2).strip()
            raw_name = repo_spec.split("/")[-1]
            name = DISPLAY_NAMES.get(raw_name, raw_name)
            dependencies.append(
                {
                    "name": name,
                    "repo": repo_spec,
                    "version": raw_version,
                    "type": "CPM",
                    "scope": scope,
                }
            )
            continue

        # Match find_package(PackageName Version ...)
        fp_match = re.search(
            r"find_package\s*\(\s*([A-Za-z0-9_]+)\s+([0-9\.]+)?", stripped
        )
        if fp_match:
            pkg_name = fp_match.group(1)
            pkg_ver = fp_match.group(2) or "REQUIRED"
            name = DISPLAY_NAMES.get(pkg_name, pkg_name)
            if not any(d["name"] == name for d in dependencies):
                dependencies.append(
                    {
                        "name": name,
                        "repo": f"System / {pkg_name}",
                        "version": f">= {pkg_ver}"
                        if pkg_ver != "REQUIRED"
                        else "System",
                        "type": "find_package",
                        "scope": scope if scope != "Core" else "Linux/macOS",
                    }
                )

    return dependencies


def generate_markdown(dependencies, project_name="sip2json"):
    """
    Generates markdown content containing a rich, colorful Mermaid diagram and dependency breakdown table.
    """
    platform_deps = [
        d for d in dependencies if d["scope"] in ("Windows", "Linux/macOS")
    ]
    core_deps = [d for d in dependencies if d["scope"] == "Core"]
    test_deps = [d for d in dependencies if d["scope"] == "Test"]

    lines = []
    lines.append("# Project Dependencies & Architecture Hierarchy")
    lines.append("")
    lines.append(
        f"`{project_name}` is a lightweight, zero-binary-bloat, **header-only Modern C++23 library**. "
        "Dependencies are managed declaratively via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) "
        "and cached automatically across local and CI builds."
    )
    lines.append("")
    lines.append("## Dependency Architecture Diagram")
    lines.append("")
    lines.append("```mermaid")
    lines.append("graph TD")
    lines.append('    App["🚀 <b>Client Application / Service</b><br/><i>(VoIP Proxy, WebRTC Gateway, Media Server, Analytics)</i>"]:::appClass')
    lines.append("")
    lines.append(f'    {project_name}["📦 <b>{project_name}::{project_name}</b><br/><i>Modern C++23 Header-Only SIP Parser & Serializer</i>"]:::projectClass')
    lines.append("")

    if core_deps:
        lines.append('    subgraph CoreGroup ["⚡ Core Header-Only Dependencies (Required via CPM)"]')
        for i, d in enumerate(core_deps):
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            role_desc = "JSON Model & Deserialization" if "json" in d["name"].lower() else "Compile-Time Regular Expressions"
            color_class = f"coreClass{i+1}"
            lines.append(f'        {node_id}["<b>{d["name"]}</b> <code>{d["version"]}</code><br/><i>{role_desc}</i>"]:::{color_class}')
        lines.append("    end")
        lines.append("")

    if platform_deps:
        lines.append('    subgraph PlatformGroup ["🖥️ Platform-Specific Dependencies"]')
        for d in platform_deps:
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            scope_desc = "Windows / MSVC" if d["scope"] == "Windows" else "Linux / macOS"
            lines.append(f'        {node_id}["<b>{d["name"]}</b> <code>{d["version"]}</code><br/><i>{scope_desc}</i>"]:::platformClass')
        lines.append("    end")
        lines.append("")

    if test_deps:
        lines.append('    subgraph TestGroup ["🧪 Test & Benchmark Suite (Optional - Active when tests enabled)"]')
        for d in test_deps:
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            lines.append(f'        {node_id}["<b>{d["name"]}</b> <code>{d["version"]}</code><br/><i>Unit, Torture & SDP Certification Tests</i>"]:::testClass')
        lines.append("    end")
        lines.append("")

    # Connect diagram edges
    lines.append(f'    App -->|"<code>#include &lt;siddiqsoft/sip2json.hpp&gt;</code>"| {project_name}')
    for d in core_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(f'    {project_name} -->|"<code>INTERFACE link</code>"| {node_id}')

    for d in platform_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(f'    {project_name} -->|"<code>Platform dependency</code>"| {node_id}')

    for d in test_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(
            f'    {project_name} -.->|"<code>sip2json_BUILD_TESTS=ON</code>"| {node_id}'
        )

    lines.append("")
    lines.append("    classDef appClass fill:#2E7D32,stroke:#1B5E20,stroke-width:2px,color:#FFFFFF,font-weight:bold;")
    lines.append("    classDef projectClass fill:#1565C0,stroke:#0D47A1,stroke-width:3px,color:#FFFFFF,font-weight:bold;")
    lines.append("    classDef coreClass1 fill:#6A1B9A,stroke:#4A148C,stroke-width:2px,color:#FFFFFF;")
    lines.append("    classDef coreClass2 fill:#00695C,stroke:#004D40,stroke-width:2px,color:#FFFFFF;")
    lines.append("    classDef platformClass fill:#0277BD,stroke:#01579B,stroke-width:2px,color:#FFFFFF;")
    lines.append("    classDef testClass fill:#E65100,stroke:#BF360C,stroke-width:2px,color:#FFFFFF;")
    lines.append("```")
    lines.append("")
    lines.append("---")
    lines.append("")
    lines.append("## Detailed Dependency Breakdown")
    lines.append("")
    lines.append(
        "| Dependency | Repository / Source | Version | Integration Method | Scope / Target | Description |"
    )
    lines.append(
        "| :--- | :--- | :---: | :---: | :--- | :--- |"
    )

    all_deps = core_deps + platform_deps + test_deps
    for d in all_deps:
        if d["repo"].startswith("System /"):
            repo_str = f'`{d["repo"]}`'
        else:
            repo_str = f'[`{d["repo"]}`](https://github.com/{d["repo"]})'

        scope_str = {
            "Windows": "Windows (MSVC)",
            "Linux/macOS": "Linux / macOS (GCC, Clang)",
            "Core": "All Platforms (`INTERFACE`)",
            "Test": "Test Suite (`sip2json_BUILD_TESTS=ON`)",
        }.get(d["scope"], d["scope"])

        desc = {
            "nlohmann_json": "First-class JSON object model and DOM serialization",
            "ctre": "Fast compile-time regular expression evaluation engine",
            "gtest": "GoogleTest runner for compliance and torture suites",
        }.get(d["name"], "Component library dependency")

        lines.append(
            f'| **{d["name"]}** | {repo_str} | `{d["version"]}` | `{d["type"]}` | {scope_str} | {desc} |'
        )

    lines.append("")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Extract CMake dependencies into dependencies.md with Mermaid diagram."
    )
    parser.add_argument(
        "--root",
        type=str,
        default=".",
        help="Root directory of the project (default: current directory)",
    )
    parser.add_argument(
        "--output",
        type=str,
        default="dependencies.md",
        help="Path to output markdown file (default: dependencies.md)",
    )
    parser.add_argument(
        "--also-output",
        type=str,
        default="docs/integration/dependencies.md",
        help="Additional file path to write to (default: docs/integration/dependencies.md)",
    )

    args = parser.parse_args()

    root_path = Path(args.root).resolve()
    cmake_files = [
        root_path / "CMakeLists.txt",
        root_path / "tests" / "CMakeLists.txt",
    ]

    all_deps = []
    for cm_file in cmake_files:
        if cm_file.exists():
            deps = parse_cmake_file(cm_file)
            all_deps.extend(deps)

    markdown_content = generate_markdown(all_deps, project_name="sip2json")

    outputs = [Path(args.output)]
    if args.also_output:
        outputs.append(Path(args.also_output))

    for out_path in outputs:
        full_out_path = root_path / out_path if not out_path.is_absolute() else out_path
        full_out_path.parent.mkdir(parents=True, exist_ok=True)
        with open(full_out_path, "w", encoding="utf-8") as f:
            f.write(markdown_content)
        print(f"[generate_dependencies_md] Wrote dependency documentation to: {full_out_path}")


if __name__ == "__main__":
    main()
