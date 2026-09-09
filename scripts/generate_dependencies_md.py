#!/usr/bin/env python3
"""
generate_dependencies_md.py

Parses CMakeLists.txt files in the repository to extract project dependencies
and generates docs/quickstart/dependencies.md with a Mermaid dependency graph
and breakdown table.
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
    "benchmark": "benchmark",
}


def extract_dependencies(root_path: Path):
    """
    Parses CMakeLists.txt files across the repository (root, benchmarks, tests)
    to extract find_package and CPMAddPackage dependencies.
    """
    deps = {}
    cmake_files = [root_path / "CMakeLists.txt"]

    bench_dir = root_path / "benchmarks"
    if bench_dir.exists():
        cmake_files.extend(bench_dir.rglob("CMakeLists.txt"))

    tests_dir = root_path / "tests"
    if tests_dir.exists():
        cmake_files.extend(tests_dir.rglob("CMakeLists.txt"))

    for cf in cmake_files:
        if not cf.exists():
            continue

        rel_path = cf.relative_to(root_path)
        content = cf.read_text(encoding="utf-8")

        # Determine default scope based on directory
        rel_str = str(rel_path)
        if rel_str.startswith("tests"):
            default_scope = "Test"
        elif rel_str.startswith("benchmarks"):
            default_scope = "Benchmark"
        else:
            default_scope = "Core"

        # 1. Shorthand: CPMAddPackage("gh:owner/repo#version") or ("owner/repo#version")
        shorthand_matches = re.findall(
            r'(?:cpmaddpackage|CPMAddPackage)\s*\(\s*["\'](?:gh:)?([^"\'#]+)#([^"\'\)]+)["\']\s*\)',
            content,
            re.IGNORECASE,
        )
        for repo_spec, raw_version in shorthand_matches:
            repo_spec = repo_spec.strip()
            raw_version = raw_version.strip()
            raw_name = repo_spec.split("/")[-1]
            if raw_name.lower() == "sip2json":
                continue
            name = DISPLAY_NAMES.get(raw_name, raw_name)
            scope = "Benchmark" if name == "benchmark" else default_scope
            if name not in deps or deps[name]["scope"] != "Core":
                deps[name] = {
                    "name": name,
                    "repo": repo_spec,
                    "version": raw_version,
                    "type": "CPM",
                    "scope": scope,
                }

        # 2. Multi-line: CPMAddPackage(NAME <name> GITHUB_REPOSITORY <repo> GIT_TAG <ver> ...)
        multiline_matches = re.findall(
            r'(?:cpmaddpackage|CPMAddPackage)\s*\(\s*NAME\s+([A-Za-z0-9_]+)\s+GITHUB_REPOSITORY\s+([^\s\)]+)\s+(?:GIT_TAG|VERSION)\s+([^\s\)]+)',
            content,
            re.IGNORECASE,
        )
        for name_tok, repo_spec, raw_version in multiline_matches:
            repo_spec = repo_spec.strip()
            raw_version = raw_version.strip()
            raw_name = repo_spec.split("/")[-1]
            if raw_name.lower() == "sip2json":
                continue
            name = DISPLAY_NAMES.get(name_tok, DISPLAY_NAMES.get(raw_name, name_tok))
            scope = "Benchmark" if name == "benchmark" else default_scope
            if name not in deps or deps[name]["scope"] != "Core":
                deps[name] = {
                    "name": name,
                    "repo": repo_spec,
                    "version": raw_version,
                    "type": "CPM",
                    "scope": scope,
                }

        # 3. Match find_package(PackageName Version ...)
        for line in content.splitlines():
            stripped = line.strip()
            fp_match = re.search(
                r"find_package\s*\(\s*([A-Za-z0-9_]+)\s+([0-9\.]+)?", stripped
            )
            if fp_match:
                pkg_name = fp_match.group(1)
                pkg_ver = fp_match.group(2) or "REQUIRED"
                if pkg_name.lower() in ("cpm", "git", "threads"):
                    continue
                name = DISPLAY_NAMES.get(pkg_name, pkg_name)
                if name not in deps:
                    deps[name] = {
                        "name": name,
                        "repo": f"System / {pkg_name}",
                        "version": f">= {pkg_ver}" if pkg_ver != "REQUIRED" else "System",
                        "type": "find_package",
                        "scope": default_scope,
                    }

    return list(deps.values())


def generate_markdown(dependencies, project_name="sip2json"):
    """
    Generates markdown content containing the Mermaid diagram and dependency breakdown table.
    """
    core_deps = [d for d in dependencies if d["scope"] == "Core"]
    test_deps = [d for d in dependencies if d["scope"] == "Test"]
    bench_deps = [d for d in dependencies if d["scope"] == "Benchmark"]
    platform_deps = [
        d for d in dependencies if d["scope"] in ("Windows", "Linux/macOS")
    ]

    lines = []
    lines.append("# Project Dependencies")
    lines.append("")
    lines.append(
        f"This document is automatically generated from `CMakeLists.txt` files for `{project_name}`."
    )
    lines.append("")
    lines.append("## Dependency Diagram")
    lines.append("")
    lines.append("```mermaid")
    lines.append("graph TD")
    lines.append(f'    {project_name}["{project_name}::{project_name}"]')
    lines.append("")

    if core_deps:
        lines.append('    subgraph Core["Core Dependencies (via CPM)"]')
        for d in core_deps:
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            lines.append(f'        {node_id}["{d["name"]} {d["version"]}"]')
        lines.append("    end")
        lines.append("")

    if test_deps or bench_deps:
        lines.append('    subgraph TestBench["Test & Benchmark Dependencies (Conditional)"]')
        for d in test_deps:
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            lines.append(f'        {node_id}["{d["name"]} {d["version"]}"]')
        for d in bench_deps:
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            lines.append(f'        {node_id}["{d["name"]} {d["version"]}"]')
        lines.append("    end")
        lines.append("")

    if platform_deps:
        lines.append('    subgraph Platform["Platform-Specific Dependencies"]')
        for d in platform_deps:
            node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
            lines.append(f'        {node_id}["{d["name"]} {d["version"]}"]')
        lines.append("    end")
        lines.append("")

    # Diagram connections
    for d in core_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(f"    {project_name} --> {node_id}")

    for d in test_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(f"    {project_name} -.->|BUILD_TESTS=ON| {node_id}")

    for d in bench_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(f"    {project_name} -.->|BUILD_BENCHMARKS=ON| {node_id}")

    for d in platform_deps:
        node_id = re.sub(r"[^A-Za-z0-9]", "", d["name"]).upper()
        lines.append(f"    {project_name} --> {node_id}")

    lines.append("```")
    lines.append("")
    lines.append("## Dependency Breakdown")
    lines.append("")
    lines.append(
        "| Dependency | Repository / Target | Version | Type | Scope |"
    )
    lines.append(
        "| :--- | :--- | :--- | :--- | :--- |"
    )

    all_deps = core_deps + test_deps + bench_deps + platform_deps
    for d in all_deps:
        if d["repo"].startswith("System /"):
            repo_str = f'`{d["repo"]}`'
        else:
            repo_str = f'[`{d["repo"]}`](https://github.com/{d["repo"]})'

        scope_str = {
            "Core": "All Platforms (`INTERFACE`)",
            "Test": "Tests only (`sip2json_BUILD_TESTS=ON`)",
            "Benchmark": "Benchmarks only (`sip2json_BUILD_BENCHMARKS=ON`)",
            "Windows": "Windows (MSVC)",
            "Linux/macOS": "Linux / macOS",
        }.get(d["scope"], d["scope"])

        lines.append(
            f'| **{d["name"]}** | {repo_str} | {d["version"]} | `{d["type"]}` | {scope_str} |'
        )

    lines.append("")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Extract CMake dependencies into markdown documentation with Mermaid diagram."
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
        default="docs/quickstart/dependencies.md",
        help="Path to output markdown file (default: docs/quickstart/dependencies.md)",
    )

    args = parser.parse_args()

    root_path = Path(args.root).resolve()
    all_deps = extract_dependencies(root_path)
    markdown_content = generate_markdown(all_deps, project_name="sip2json")

    out_path = Path(args.output)
    full_out_path = root_path / out_path if not out_path.is_absolute() else out_path
    full_out_path.parent.mkdir(parents=True, exist_ok=True)

    # Avoid rewriting if content is identical (prevents infinite reload loops)
    if full_out_path.exists():
        existing_content = full_out_path.read_text(encoding="utf-8")
        if existing_content == markdown_content:
            print(f"[generate_dependencies_md] Up to date: {full_out_path}")
            return

    with open(full_out_path, "w", encoding="utf-8") as f:
        f.write(markdown_content)
    print(f"[generate_dependencies_md] Wrote dependency documentation to: {full_out_path}")


if __name__ == "__main__":
    main()
