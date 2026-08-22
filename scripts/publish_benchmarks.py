#!/usr/bin/env python3
"""
Publish Benchmarks Script
Automatically locates benchmark outputs across CI build matrix runners (Windows x64/arm64, Linux x64/arm64, macOS),
compiles multi-platform performance reports, generates interactive HTML charts, and updates docs/features/benchmarks.md.

Usage:
    python3 scripts/publish_benchmarks.py [--root REPO_ROOT] [--skip-build] [--skip-exec]
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

def run_cmd(cmd, cwd=None):
    """Helper to run command and log output."""
    print(f"[publish_benchmarks] Running: {' '.join(cmd)}", flush=True)
    res = subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)
    if res.returncode != 0:
        print(f"[publish_benchmarks] Command failed with exit code {res.returncode}:\n{res.stderr}", flush=True)
        return False
    return True

def parse_platform_from_filename(filename_str: str) -> tuple:
    """Extract OS, Arch, and Compiler from benchmark JSON filename if available."""
    # Pattern: benchmark_results_OS_ARCH_COMPILER.json
    name = Path(filename_str).stem
    parts = name.replace("benchmark_results_", "").split("_")
    os_name = parts[0] if len(parts) > 0 else "Host"
    arch_name = parts[1] if len(parts) > 1 else "native"
    compiler_name = parts[2] if len(parts) > 2 else "Clang/GCC/MSVC"
    return os_name, arch_name, compiler_name

def update_benchmarks_doc(repo_root: Path, platform_results: list):
    """Dynamically update docs/features/benchmarks.md between PIPELINE_BENCHMARKS markers."""
    doc_path = repo_root / "docs" / "features" / "benchmarks.md"
    if not doc_path.exists():
        print(f"[publish_benchmarks] Warning: {doc_path} not found.", flush=True)
        return

    content = doc_path.read_text(encoding="utf-8")
    start_marker = "<!-- PIPELINE_BENCHMARKS_START -->"
    end_marker = "<!-- PIPELINE_BENCHMARKS_END -->"

    if start_marker not in content or end_marker not in content:
        print(f"[publish_benchmarks] Warning: Pipeline benchmark markers not found in {doc_path}.", flush=True)
        return

    # Build Markdown table
    table_lines = [
        start_marker,
        "## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix",
        "",
        "*Empirical build pipeline measurements collected across matrix runners:*",
        "",
        "| Operating System | Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |",
        "| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |"
    ]

    if platform_results:
        for res in platform_results:
            os_name = res.get("os", "Linux")
            arch = res.get("arch", "x64")
            compiler = res.get("compiler", "Clang")
            async_tput = res.get("async_tput", "N/A")
            bandwidth = res.get("bandwidth", "N/A")
            async_lat = res.get("async_lat", "N/A")
            single_tput = res.get("single_tput", "N/A")
            single_lat = res.get("single_lat", "N/A")

            table_lines.append(
                f"| **{os_name}** | **{arch}** | {compiler} | **{async_tput}** | **{bandwidth}** | **{async_lat}** | **{single_tput}** | **{single_lat}** |"
            )
    else:
        table_lines.append("| *Awaiting Pipeline Run* | *x64 / arm64* | CI Runners | *Collected on CI* | *Collected on CI* | *Collected on CI* | *Collected on CI* | *Collected on CI* |")

    table_lines.append(end_marker)
    new_section = "\n".join(table_lines)

    # Replace content between markers
    pattern = re.compile(f"{re.escape(start_marker)}.*?{re.escape(end_marker)}", re.DOTALL)
    updated_content = pattern.sub(new_section, content)

    doc_path.write_text(updated_content, encoding="utf-8")
    print(f"[publish_benchmarks] Updated {doc_path} with pipeline benchmark metrics.", flush=True)

def main():
    parser = argparse.ArgumentParser(description="Publish sip2json benchmark reports")
    parser.add_argument("--root", type=str, help="Repository root path")
    parser.add_argument("--skip-build", action="store_true", help="Skip building benchmark binary if executable already exists")
    parser.add_argument("--skip-exec", action="store_true", help="Skip running benchmark executable if json output already exists")
    args = parser.parse_args()

    repo_root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parent.parent
    benchmarks_dir = repo_root / "benchmarks"
    docs_assets_dir = repo_root / "docs" / "assets"
    docs_assets_dir.mkdir(parents=True, exist_ok=True)

    # 1. Search for benchmark JSON artifacts collected from build matrix jobs
    json_search_dirs = [
        benchmarks_dir / "artifacts",
        benchmarks_dir / "results",
        repo_root / "build",
        benchmarks_dir
    ]

    json_files = []
    for sdir in json_search_dirs:
        if sdir.exists():
            for p in sdir.glob("**/*.json"):
                if "benchmark" in p.name.lower():
                    json_files.append(p)

    platform_results = []
    for jfile in json_files:
        try:
            data = json.loads(jfile.read_text(encoding="utf-8"))
            os_name, arch, compiler = parse_platform_from_filename(jfile.name)
            
            # Extract key metrics if present
            b_list = data.get("benchmarks", [])
            async_tput = "N/A"
            bandwidth = "N/A"
            async_lat = "N/A"
            single_tput = "N/A"
            single_lat = "N/A"

            for b in b_list:
                bname = b.get("name", "")
                if "parseAsync" in bname or "Callback" in bname:
                    items_sec = b.get("items_per_second", 0)
                    rtime = b.get("real_time", 0)
                    tunit = b.get("time_unit", "us")
                    if items_sec > 0:
                        async_tput = f"{items_sec:,.2f} msg/s"
                        bandwidth = f"{(items_sec * 2600) / 1024 / 1024:.2f} MB/s"
                    if rtime > 0:
                        async_lat = f"{rtime:.2f} {tunit}"
                elif "parseFromBuffer" in bname or "Single" in bname:
                    items_sec = b.get("items_per_second", 0)
                    rtime = b.get("real_time", 0)
                    tunit = b.get("time_unit", "us")
                    if items_sec > 0:
                        single_tput = f"{items_sec:,.2f} msg/s"
                    if rtime > 0:
                        single_lat = f"{rtime:.2f} {tunit}"

            platform_results.append({
                "os": os_name,
                "arch": arch,
                "compiler": compiler,
                "async_tput": async_tput,
                "bandwidth": bandwidth,
                "async_lat": async_lat,
                "single_tput": single_tput,
                "single_lat": single_lat
            })
        except Exception as ex:
            print(f"[publish_benchmarks] Could not parse JSON file {jfile}: {ex}", flush=True)

    # 2. Update docs/features/benchmarks.md with collected platform results
    update_benchmarks_doc(repo_root, platform_results)

    # 3. Locate primary benchmark executable or run generator if JSON exists
    json_out_path = benchmarks_dir / "benchmark_results.json"
    generator_script = benchmarks_dir / "benchmark_report_generator.py"
    html_report = benchmarks_dir / "benchmark_report.html"
    published_html = docs_assets_dir / "benchmark_report.html"

    if json_out_path.exists() and generator_script.exists():
        gen_cmd = [sys.executable, str(generator_script), str(json_out_path), str(benchmarks_dir)]
        if run_cmd(gen_cmd):
            print(f"[publish_benchmarks] Generated benchmark reports in {benchmarks_dir}", flush=True)

    if html_report.exists():
        shutil.copy2(html_report, published_html)
        print(f"[publish_benchmarks] Published interactive HTML report to: {published_html}", flush=True)

if __name__ == "__main__":
    main()
