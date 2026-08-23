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
    name = Path(filename_str).stem
    if name.startswith("benchmark_results_"):
        parts = name.replace("benchmark_results_", "").split("_")
        os_name = parts[0] if len(parts) > 0 else "Linux"
        arch_name = parts[1] if len(parts) > 1 else "x64"
        compiler_name = parts[2] if len(parts) > 2 else "Clang/MSVC"
        return os_name, arch_name, compiler_name
    return "Linux", "x64", "Clang"

def check_platform_completeness(platform_results: list, required_str: str) -> tuple:
    """Check if all required (OS, Arch) combinations are present in platform_results."""
    if not required_str:
        return True, []
    
    req_tuples = []
    for pair in required_str.split(","):
        pair = pair.strip()
        if ":" in pair:
            os_p, arch_p = pair.split(":", 1)
            req_tuples.append((os_p.strip().lower(), arch_p.strip().lower()))

    found_set = {
        (res.get("os", "").lower(), res.get("arch", "").lower())
        for res in platform_results
    }

    missing = []
    for req_os, req_arch in req_tuples:
        if (req_os, req_arch) not in found_set:
            missing.append(f"{req_os.capitalize()}-{req_arch}")

    is_complete = len(missing) == 0
    return is_complete, missing

def update_benchmarks_doc(repo_root: Path, platform_results: list, require_all: bool = False, required_str: str = ""):
    """Dynamically update docs/features/benchmarks.md between PIPELINE_BENCHMARKS markers."""
    doc_path = repo_root / "docs" / "features" / "benchmarks.md"
    if not doc_path.exists():
        print(f"[publish_benchmarks] Warning: {doc_path} not found.", flush=True)
        return

    # Check completeness if require_all is specified
    is_complete, missing = check_platform_completeness(platform_results, required_str)
    if require_all and not is_complete:
        print(f"[publish_benchmarks] Benchmark collection is INCOMPLETE!", flush=True)
        print(f"[publish_benchmarks] Missing required matrix targets: {', '.join(missing)}.", flush=True)
        print(f"[publish_benchmarks] SKIPPING update to {doc_path} to avoid publishing incomplete data.", flush=True)
        return

    content = doc_path.read_text(encoding="utf-8")
    start_marker = "<!-- PIPELINE_BENCHMARKS_START -->"
    end_marker = "<!-- PIPELINE_BENCHMARKS_END -->"

    if start_marker not in content or end_marker not in content:
        print(f"[publish_benchmarks] Warning: Pipeline benchmark markers not found in {doc_path}.", flush=True)
        return

    # Group results by Operating System platform
    grouped_results = {}
    for res in platform_results:
        os_key = res.get("os", "Linux").capitalize()
        if os_key not in grouped_results:
            grouped_results[os_key] = []
        grouped_results[os_key].append(res)

    os_order = ["Linux", "Windows", "macOS"]
    sorted_os_keys = sorted(grouped_results.keys(), key=lambda x: os_order.index(x) if x in os_order else 99)

    table_lines = [
        start_marker,
        "## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix",
        "",
        "*Empirical build pipeline measurements collected across matrix runners grouped by operating system platform:*",
        ""
    ]

    if platform_results:
        for os_key in sorted_os_keys:
            res_list = grouped_results[os_key]
            res_list.sort(key=lambda r: (r.get("arch", ""), r.get("compiler", "")))

            table_lines.append(f"### {os_key} Platform Benchmarks")
            table_lines.append("")
            table_lines.append("| Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |")
            table_lines.append("| :---: | :---: | :---: | :---: | :---: | :---: | :---: |")

            for res in res_list:
                arch = res.get("arch", "x64")
                compiler = res.get("compiler", "Clang")
                async_tput = res.get("async_tput", "N/A")
                bandwidth = res.get("bandwidth", "N/A")
                async_lat = res.get("async_lat", "N/A")
                single_tput = res.get("single_tput", "N/A")
                single_lat = res.get("single_lat", "N/A")

                table_lines.append(
                    f"| **{arch}** | {compiler} | **{async_tput}** | **{bandwidth}** | **{async_lat}** | **{single_tput}** | **{single_lat}** |"
                )
            table_lines.append("")
    else:
        table_lines.append("| Operating System | Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |")
        table_lines.append("| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |")
        table_lines.append("| *Awaiting Pipeline Run* | *x64 / arm64* | CI Runners | *Collected on CI* | *Collected on CI* | *Collected on CI* | *Collected on CI* | *Collected on CI* |")
        table_lines.append("")

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
    parser.add_argument("--require-all", action="store_true", help="Skip updating documentation if any required platform matrix target is missing")
    parser.add_argument("--required-platforms", type=str, default="linux:x64,linux:arm64,windows:x64,windows:arm64", help="Comma-separated list of required OS:ARCH matrix targets")
    args = parser.parse_args()

    repo_root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parent.parent
    benchmarks_dir = repo_root / "benchmarks"
    docs_assets_dir = repo_root / "docs" / "assets"
    docs_assets_dir.mkdir(parents=True, exist_ok=True)

    # 1. Search for benchmark JSON and TXT artifacts collected from build matrix jobs
    json_search_dirs = [
        benchmarks_dir / "artifacts",
        benchmarks_dir / "results",
        repo_root / "build",
        benchmarks_dir
    ]

    platform_results_map = {}

    for sdir in json_search_dirs:
        if not sdir.exists():
            continue

        # Process text summaries (stream_benchmark_summary.txt)
        for txt_file in sdir.glob("**/stream_benchmark_summary.txt"):
            try:
                os_name, arch, compiler = parse_platform_from_filename(txt_file.parent.name)
                if os_name == "Linux" and "Windows" in str(txt_file): os_name = "Windows"
                
                content = txt_file.read_text(encoding="utf-8", errors="ignore")
                key = (os_name, arch, compiler)
                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A"
                })

                stream_match = re.search(r"parseAsync.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Data Bandwidth\s*:\s*([\d,.]+)\s*MB/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if stream_match:
                    tput, bw, lat, unit = stream_match.groups()
                    res["async_tput"] = f"{float(tput.replace(',', '')):,.2f} msg/s"
                    res["bandwidth"] = f"{float(bw.replace(',', '')):.2f} MB/s"
                    res["async_lat"] = f"{float(lat.replace(',', '')):.2f} {unit}"

                single_match = re.search(r"parseFromBuffer.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if single_match:
                    tput, lat, unit = single_match.groups()
                    res["single_tput"] = f"{float(tput.replace(',', '')):,.2f} msg/s"
                    res["single_lat"] = f"{float(lat.replace(',', '')):.2f} {unit}"

                platform_results_map[key] = res
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse text summary {txt_file}: {ex}", flush=True)

        # Process Google Benchmark JSON files matching benchmark_results_*.json
        for jfile in sdir.glob("**/benchmark_results_*.json"):
            try:
                data = json.loads(jfile.read_text(encoding="utf-8"))
                os_name, arch, compiler = parse_platform_from_filename(jfile.name)
                key = (os_name, arch, compiler)
                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A"
                })

                b_list = data.get("benchmarks", [])
                for b in b_list:
                    bname = b.get("name", "").lower()
                    items_sec = b.get("items_per_second", 0)
                    rtime = b.get("real_time", 0)
                    tunit = b.get("time_unit", "us")

                    if "async" in bname or "callback" in bname:
                        if items_sec > 0:
                            res["async_tput"] = f"{items_sec:,.2f} msg/s"
                            res["bandwidth"] = f"{(items_sec * 2600) / 1024 / 1024:.2f} MB/s"
                        if rtime > 0:
                            res["async_lat"] = f"{rtime:.2f} {tunit}"
                    elif any(k in bname for k in ["single", "parsefrombuffer", "minimalresponse", "registerrequest", "invitewithsdp"]):
                        if items_sec > 0:
                            res["single_tput"] = f"{items_sec:,.2f} msg/s"
                        if rtime > 0:
                            res["single_lat"] = f"{rtime:.2f} {tunit}"

                platform_results_map[key] = res
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse JSON file {jfile}: {ex}", flush=True)

    platform_results = list(platform_results_map.values())

    # 2. Update docs/features/benchmarks.md with collected platform results
    update_benchmarks_doc(repo_root, platform_results, require_all=args.require_all, required_str=args.required_platforms)

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
