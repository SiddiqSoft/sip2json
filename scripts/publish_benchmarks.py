#!/usr/bin/env python3
"""
Publish Benchmarks Script
Dynamically discovers benchmark results and host environment metadata emitted by CI build matrix runners,
compiles multi-platform performance reports, and updates docs/architecture/benchmarks.md strictly from build data.

Usage:
    python3 scripts/publish_benchmarks.py [--root REPO_ROOT] [--skip-build] [--skip-exec]
"""

import argparse
import json
import os
import platform
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


def format_latency(val_float: float, unit: str = "us") -> str:
    """Convert latency into human-readable microseconds or milliseconds uniformly."""
    unit_lower = unit.lower().strip()
    if unit_lower in ("ns", "nanoseconds", "nanosecond"):
        us = val_float / 1000.0
    elif unit_lower in ("ms", "milliseconds", "millisecond"):
        us = val_float * 1000.0
    elif unit_lower in ("s", "sec", "seconds", "second"):
        us = val_float * 1000000.0
    else:  # us / µs
        us = val_float

    if us < 1000.0:
        return f"{us:.2f} µs"
    elif us < 1000000.0:
        return f"{us / 1000.0:.2f} ms"
    else:
        return f"{us / 1000000.0:.2f} s"


def normalize_os(os_raw: str) -> str:
    """Normalize OS identifier across various platforms and agent outputs."""
    s = (os_raw or "").lower().strip()
    if any(k in s for k in ("darwin", "apple", "macos", "osx")):
        return "macOS"
    if any(k in s for k in ("windows", "win", "win32", "win64", "windows_nt")):
        return "Windows"
    if any(k in s for k in ("linux", "rhel", "redhat", "centos", "fedora", "ubuntu")):
        return "Linux"
    return os_raw.capitalize() if os_raw else "Linux"


def normalize_arch(arch_raw: str) -> str:
    """Normalize architecture string to arm64, x64, or x86."""
    s = (arch_raw or "").lower().strip()
    if s in ("arm64", "aarch64"):
        return "arm64"
    if s in ("x64", "x86_64", "amd64"):
        return "x64"
    if s in ("x86", "i386", "i686"):
        return "x86"
    return s


def normalize_compiler(compiler_raw: str, os_name: str) -> str:
    """Normalize compiler label to AppleClang, MSVC, GCC, or Clang."""
    comp = (compiler_raw or "").strip()
    comp_low = comp.lower()
    os_norm = normalize_os(os_name)
    if os_norm == "macOS":
        return "AppleClang"
    if "msvc" in comp_low or "cl.exe" in comp_low or comp_low == "cl":
        return "MSVC"
    if "gcc" in comp_low or "g++" in comp_low:
        return "GCC"
    if "clang" in comp_low:
        return "Clang"
    return comp or ("AppleClang" if os_norm == "macOS" else ("MSVC" if os_norm == "Windows" else "GCC"))


def format_platform_arch_label(os_name: str, arch: str, os_release: str = "") -> str:
    """Derive clean platform and architecture label using actual host OS release if available."""
    arch_norm = normalize_arch(arch)
    os_norm = normalize_os(os_name)

    if os_release:
        clean_rel = os_release.strip()
        if os_norm == "macOS" and "apple" not in clean_rel.lower():
            clean_rel = f"Apple {clean_rel}"
        return f"{clean_rel} ({arch_norm})"

    if os_norm == "macOS":
        return f"Apple macOS ({arch_norm})"
    elif os_norm == "Windows":
        return f"Microsoft Windows ({arch_norm})"
    elif os_norm == "Linux":
        return f"Red Hat Enterprise Linux ({arch_norm})"
    return f"{os_name} ({arch_norm})"


def parse_platform_from_path(file_path: Path) -> tuple:
    """Extract OS, Arch, and Compiler from file path or parent directories."""
    path_str = str(file_path).replace("\\", "/")

    m = re.search(r"benchmark[-_]results[-_]([A-Za-z]+)[-_](arm64|x64|x86)(?:[-_]([A-Za-z0-9]+))?", path_str, re.IGNORECASE)
    if m:
        os_raw = m.group(1)
        os_name = normalize_os(os_raw)
        arch_name = normalize_arch(m.group(2))
        comp_raw = m.group(3) or ("AppleClang" if os_name == "macOS" else ("MSVC" if os_name == "Windows" else "GCC"))
        compiler_name = normalize_compiler(comp_raw, os_name)
        return os_name, arch_name, compiler_name

    m = re.search(r"build/([A-Za-z0-9_-]+)", path_str, re.IGNORECASE)
    if m:
        preset = m.group(1).lower()
        if any(k in preset for k in ("apple", "darwin", "macos")):
            arch = "arm64" if "arm64" in preset else ("x64" if "x64" in preset else ("arm64" if platform.machine() in ("arm64", "aarch64") else "x64"))
            return "macOS", arch, "AppleClang"
        elif any(k in preset for k in ("windows", "win")):
            arch = "arm64" if "arm64" in preset else "x64"
            return "Windows", arch, "MSVC"
        elif "linux" in preset:
            arch = "arm64" if "arm64" in preset else "x64"
            compiler = "GCC" if "gcc" in preset else "Clang"
            return "Linux", arch, compiler

    return "Linux", "arm64", "GCC"


def check_platform_completeness(platform_results: list, required_str: str) -> tuple:
    """Check if all required (OS, Arch) combinations are present in platform_results."""
    if not required_str:
        return True, []

    req_tuples = []
    for pair in required_str.split(","):
        pair = pair.strip()
        if ":" in pair:
            os_p, arch_p = pair.split(":", 1)
            req_tuples.append((normalize_os(os_p).lower(), normalize_arch(arch_p).lower()))

    found_set = {
        (normalize_os(res.get("os", "")).lower(), normalize_arch(res.get("arch", "")).lower())
        for res in platform_results
    }

    missing = []
    for req_os, req_arch in req_tuples:
        if (req_os, req_arch) not in found_set:
            missing.append(f"{req_os.capitalize()}-{req_arch}")

    is_complete = len(missing) == 0
    return is_complete, missing


def build_charts_html(results: list) -> list:
    """Dynamically generate visual comparative performance charts from build pipeline measurements."""
    lines = [
        "---",
        "",
        "## 2. Visual Platform Performance Comparison",
        "",
        "Visual comparison of pipeline build results across platform runners.",
        "",
        "### Stream Parsing Throughput (`parseAsync` — Messages / Second — Higher is Better)",
        "",
        '<div class="modern-chart">'
    ]

    # Chart 1: Stream throughput
    valid_async = [r for r in results if r.get("async_tput_num", 0) > 0]
    if valid_async:
        max_async = max(r["async_tput_num"] for r in valid_async)
        for i, r in enumerate(sorted(valid_async, key=lambda x: x["async_tput_num"], reverse=True)):
            tput_num = r["async_tput_num"]
            width_pct = max(5, int((tput_num / max_async) * 100))
            is_fastest = (i == 0)
            pill = '<span class="pill pill-success">Fastest</span>' if is_fastest else ''
            bar_cls = "chart-bar chart-bar-primary" if is_fastest else ("chart-bar chart-bar-secondary" if i == 1 else ("chart-bar chart-bar-info" if i == 2 else "chart-bar chart-bar-muted"))
            title = r.get("platform_arch") or format_platform_arch_label(r.get("os", "Linux"), r.get("arch", "x64"), r.get("os_release", ""))

            lines.append('  <div class="chart-row">')
            lines.append('    <div class="chart-meta">')
            lines.append(f'      <span class="chart-title">{title} {pill}</span>')
            lines.append(f'      <span class="chart-val">{tput_num:,.0f} msg/s</span>')
            lines.append('    </div>')
            lines.append('    <div class="chart-track">')
            lines.append(f'      <div class="{bar_cls}" style="width: {width_pct}%;"></div>')
            lines.append('    </div>')
            lines.append('  </div>')
    lines.append('</div>')
    lines.append('')

    # Chart 2: Per-Message processing latency
    lines.append('### Per-Message Processing Latency (Microseconds — Lower is Better)')
    lines.append('')
    lines.append('<div class="modern-chart">')
    valid_lat = [r for r in results if r.get("async_lat_num", 0) > 0]
    if valid_lat:
        max_lat = max(r["async_lat_num"] for r in valid_lat)
        min_lat = min(r["async_lat_num"] for r in valid_lat)
        for i, r in enumerate(sorted(valid_lat, key=lambda x: x["async_lat_num"])):
            lat_num = r["async_lat_num"]
            width_pct = max(10, int((lat_num / max_lat) * 100))
            is_lowest = (lat_num == min_lat)
            pill = '<span class="pill pill-success">Lowest Latency</span>' if is_lowest else ''
            bar_cls = "chart-bar chart-bar-primary" if is_lowest else ("chart-bar chart-bar-secondary" if i == 1 else ("chart-bar chart-bar-info" if i == 2 else "chart-bar chart-bar-muted"))
            title = r.get("platform_arch") or format_platform_arch_label(r.get("os", "Linux"), r.get("arch", "x64"), r.get("os_release", ""))

            lines.append('  <div class="chart-row">')
            lines.append('    <div class="chart-meta">')
            lines.append(f'      <span class="chart-title">{title} {pill}</span>')
            lines.append(f'      <span class="chart-val">{lat_num:.2f} µs</span>')
            lines.append('    </div>')
            lines.append('    <div class="chart-track">')
            lines.append(f'      <div class="{bar_cls}" style="width: {width_pct}%;"></div>')
            lines.append('    </div>')
            lines.append('  </div>')
    lines.append('</div>')
    lines.append('')

    # Chart 3: Single-Message parsing throughput
    lines.append('### Single-Message Parsing Throughput (`parseFromBuffer` — Messages / Second — Higher is Better)')
    lines.append('')
    lines.append('<div class="modern-chart">')
    valid_single = [r for r in results if r.get("single_tput_num", 0) > 0]
    if valid_single:
        max_single = max(r["single_tput_num"] for r in valid_single)
        for i, r in enumerate(sorted(valid_single, key=lambda x: x["single_tput_num"], reverse=True)):
            tput_num = r["single_tput_num"]
            width_pct = max(5, int((tput_num / max_single) * 100))
            is_fastest = (i == 0)
            pill = '<span class="pill pill-success">Fastest</span>' if is_fastest else ''
            bar_cls = "chart-bar chart-bar-primary" if is_fastest else ("chart-bar chart-bar-secondary" if i == 1 else ("chart-bar chart-bar-info" if i == 2 else "chart-bar chart-bar-muted"))
            title = r.get("platform_arch") or format_platform_arch_label(r.get("os", "Linux"), r.get("arch", "x64"), r.get("os_release", ""))

            lines.append('  <div class="chart-row">')
            lines.append('    <div class="chart-meta">')
            lines.append(f'      <span class="chart-title">{title} {pill}</span>')
            lines.append(f'      <span class="chart-val">{tput_num:,.0f} msg/s</span>')
            lines.append('    </div>')
            lines.append('    <div class="chart-track">')
            lines.append(f'      <div class="{bar_cls}" style="width: {width_pct}%;"></div>')
            lines.append('    </div>')
            lines.append('  </div>')
    lines.append('</div>')
    lines.append('')

    return lines


def update_benchmarks_doc(repo_root: Path, platform_results: list, require_all: bool = False, required_str: str = ""):
    """Dynamically update docs/architecture/benchmarks.md strictly from pipeline build data."""
    doc_path = repo_root / "docs" / "architecture" / "benchmarks.md"
    if not doc_path.exists():
        print(f"[publish_benchmarks] Warning: {doc_path} not found.", flush=True)
        return

    # Check completeness only if explicitly required with target platforms
    is_complete, missing = check_platform_completeness(platform_results, required_str)
    if require_all and required_str and not is_complete:
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

    # Filter for results with valid throughput data
    valid_results = [
        r for r in platform_results
        if r.get("async_tput_num", 0) > 0 or r.get("single_tput_num", 0) > 0 or (r.get("async_tput", "N/A") != "N/A" and r.get("async_tput") != "0.00 msg/s")
    ]

    def sort_key(r):
        os_name = normalize_os(r.get("os", "")).lower()
        arch = normalize_arch(r.get("arch", "")).lower()
        pair = (os_name, arch)
        order = [
            ("macos", "arm64"),
            ("windows", "arm64"),
            ("linux", "arm64"),
            ("linux", "x64"),
            ("windows", "x64"),
            ("macos", "x64")
        ]
        return order.index(pair) if pair in order else 99

    sorted_results = sorted(valid_results, key=sort_key)

    table_lines = [
        start_marker,
        "## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix",
        ""
    ]

    if sorted_results:
        # Collect unique host runners from host_info metadata
        host_runners = []
        seen_runners = set()
        for r in sorted_results:
            h_info = r.get("host_info", "")
            os_label = r.get("os", "Host")
            if h_info and h_info not in seen_runners:
                seen_runners.add(h_info)
                host_runners.append(f"    - **{os_label} Runner**: {h_info}")

        table_lines.append('!!! note "Build Release & Runner Environment"')
        table_lines.append('    **Build Release & Version**: `{ version }`')
        if host_runners:
            table_lines.append('    **CI Matrix Host Runners (Derived from Pipeline)**:')
            table_lines.extend(host_runners)
        else:
            table_lines.append('    **CI Matrix Host Runners**: Derived dynamically from pipeline runners.')
        table_lines.append("")
        table_lines.append("*Build pipeline measurements collected across matrix runners:*")
        table_lines.append("")
        table_lines.append("| Platform & Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |")
        table_lines.append("| :--- | :---: | :---: | :---: | :---: | :---: | :---: |")

        for res in sorted_results:
            plat_arch = res.get("platform_arch") or format_platform_arch_label(res.get("os", "Linux"), res.get("arch", "x64"), res.get("os_release", ""))
            compiler = res.get("compiler", "Clang")
            async_tput = res.get("async_tput", "N/A")
            bandwidth = res.get("bandwidth", "N/A")
            async_lat = res.get("async_lat", "N/A")
            single_tput = res.get("single_tput", "N/A")
            single_lat = res.get("single_lat", "N/A")

            table_lines.append(
                f"| **{plat_arch}** | {compiler} | **{async_tput}** | **{bandwidth}** | **{async_lat}** | **{single_tput}** | **{single_lat}** |"
            )

        table_lines.append("")
        # Add visual comparative charts
        chart_lines = build_charts_html(sorted_results)
        table_lines.extend(chart_lines)

    else:
        # Awaiting pipeline run - no fabricated data
        table_lines.append('!!! info "Pipeline-Derived Performance Data"')
        table_lines.append('    Benchmark metrics and host runner environment details are compiled dynamically from CI/CD pipeline build matrix artifacts across our release matrix runners (Apple macOS, Red Hat Enterprise Linux, and Microsoft Windows). When release builds complete, live benchmark data will populate automatically.')
        table_lines.append("")
        table_lines.append("| Platform & Architecture | Compiler | Stream Throughput (`parseAsync`) | Bandwidth | Per-Msg Latency | Single Message (`parseFromBuffer`) | Single Latency |")
        table_lines.append("| :--- | :---: | :---: | :---: | :---: | :---: | :---: |")
        table_lines.append("| *Pipeline Build Pending* | *CI Matrix* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* | *Awaiting CI Run* |")
        table_lines.append("")
        table_lines.append("---")
        table_lines.append("")
        table_lines.append("## 2. Visual Platform Performance Comparison")
        table_lines.append("")
        table_lines.append('!!! info "Comparative Visual Performance Graphs Pending Build"')
        table_lines.append('    Visual comparative bar charts contrasting throughput and latency across Apple macOS, Red Hat Enterprise Linux (RHEL), and Microsoft Windows runners will render automatically when pipeline benchmark artifacts are compiled.')
        table_lines.append("")

    table_lines.append(end_marker)
    new_section = "\n".join(table_lines)

    # Replace content between markers
    pattern = re.compile(f"{re.escape(start_marker)}.*?{re.escape(end_marker)}", re.DOTALL)
    updated_content = pattern.sub(new_section, content)

    doc_path.write_text(updated_content, encoding="utf-8")
    print(f"[publish_benchmarks] Successfully updated {doc_path} with pipeline-derived benchmark metrics ({len(sorted_results)} platform entries).", flush=True)


def get_or_create_result(platform_results_map: dict, os_name: str, arch: str, compiler: str = "") -> dict:
    """Retrieve or initialize a platform runner entry cleanly keyed by normalized OS and Arch."""
    os_norm = normalize_os(os_name)
    arch_norm = normalize_arch(arch)
    comp_norm = normalize_compiler(compiler, os_norm) if compiler else ""
    key = (os_norm.lower(), arch_norm.lower())

    if key not in platform_results_map:
        default_comp = comp_norm or ("AppleClang" if os_norm == "macOS" else ("MSVC" if os_norm == "Windows" else "GCC"))
        platform_results_map[key] = {
            "os": os_norm,
            "arch": arch_norm,
            "compiler": default_comp,
            "os_release": "",
            "platform_arch": format_platform_arch_label(os_norm, arch_norm),
            "async_tput": "N/A",
            "bandwidth": "N/A",
            "async_lat": "N/A",
            "single_tput": "N/A",
            "single_lat": "N/A",
            "host_info": "",
            "async_tput_num": 0.0,
            "async_lat_num": 0.0,
            "single_tput_num": 0.0,
            "single_lat_num": 0.0,
        }

    entry = platform_results_map[key]
    if comp_norm:
        if entry["compiler"] in ("Clang", "GCC", "") or comp_norm == "AppleClang" or comp_norm == "MSVC":
            entry["compiler"] = comp_norm
    return entry


def main():
    parser = argparse.ArgumentParser(description="Publish sip2json benchmark reports")
    parser.add_argument("--root", type=str, help="Repository root path")
    parser.add_argument("--skip-build", action="store_true", help="Skip building benchmark binary if executable already exists")
    parser.add_argument("--skip-exec", action="store_true", help="Skip running benchmark executable if json output already exists")
    parser.add_argument("--require-all", action="store_true", help="Skip updating documentation if any required platform matrix target is missing")
    parser.add_argument("--required-platforms", type=str, default="", help="Comma-separated list of required OS:ARCH matrix targets (e.g. 'darwin:arm64,linux:arm64,windows:arm64')")
    args = parser.parse_args()

    repo_root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parent.parent
    benchmarks_dir = repo_root / "benchmarks"
    docs_assets_dir = repo_root / "docs" / "assets"
    docs_assets_dir.mkdir(parents=True, exist_ok=True)

    json_search_dirs = [
        benchmarks_dir / "artifacts",
        benchmarks_dir / "results",
        repo_root / "build",
        benchmarks_dir,
    ]

    # Include Azure DevOps standard artifact locations if available
    for env_var in ["SYSTEM_ARTIFACTSDIRECTORY", "BUILD_ARTIFACTSTAGINGDIRECTORY", "AGENT_BUILDDIRECTORY"]:
        env_val = os.environ.get(env_var)
        if env_val:
            p = Path(env_val)
            if p.exists() and p not in json_search_dirs:
                json_search_dirs.append(p)

    platform_results_map = {}

    print(f"[publish_benchmarks] Scanning for benchmark artifacts across {len(json_search_dirs)} directories...", flush=True)

    for sdir in json_search_dirs:
        if not sdir.exists():
            continue

        # 1. Process host_info.json files
        for host_file in sdir.glob("**/host_info.json"):
            try:
                hdata = json.loads(host_file.read_text(encoding="utf-8"))
                os_raw = hdata.get("platform") or hdata.get("os_name", "")
                arch_raw = hdata.get("arch", "x64")
                comp_raw = hdata.get("compiler", "")

                res = get_or_create_result(platform_results_map, os_raw, arch_raw, comp_raw)
                res["host_info"] = hdata.get("host_summary", "") or res["host_info"]
                res["os_release"] = hdata.get("os_release", "") or res["os_release"]
                res["platform_arch"] = format_platform_arch_label(res["os"], res["arch"], res["os_release"])
                if comp_raw:
                    res["compiler"] = normalize_compiler(comp_raw, res["os"])

                print(f"[publish_benchmarks] Discovered host_info: {res['platform_arch']} -> {res['host_info']} (from {host_file})", flush=True)
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse host_info.json {host_file}: {ex}", flush=True)

        # 2. Process stream_benchmark_results.json files
        for stream_json in sdir.glob("**/stream_benchmark_results.json"):
            try:
                sdata = json.loads(stream_json.read_text(encoding="utf-8"))
                os_name, arch, compiler = parse_platform_from_path(stream_json)
                res = get_or_create_result(platform_results_map, os_name, arch, compiler)

                if "stream_parse_async" in sdata:
                    spa = sdata["stream_parse_async"]
                    tput = float(spa.get("throughput_msg_per_sec", 0.0))
                    bw = float(spa.get("bandwidth_mb_per_sec", 0.0))
                    lat = float(spa.get("avg_latency_us", 0.0))
                    res["async_tput_num"] = tput
                    res["async_lat_num"] = lat
                    res["async_tput"] = f"{tput:,.2f} msg/s"
                    res["bandwidth"] = f"{bw:.2f} MB/s"
                    res["async_lat"] = format_latency(lat, "us")

                if "single_message_parse" in sdata:
                    smp = sdata["single_message_parse"]
                    tput = float(smp.get("throughput_msg_per_sec", 0.0))
                    lat = float(smp.get("avg_latency_us", 0.0))
                    res["single_tput_num"] = tput
                    res["single_lat_num"] = lat
                    res["single_tput"] = f"{tput:,.2f} msg/s"
                    res["single_lat"] = format_latency(lat, "us")

                res["platform_arch"] = format_platform_arch_label(res["os"], res["arch"], res.get("os_release", ""))
                print(f"[publish_benchmarks] Discovered stream benchmarks: {res['platform_arch']} -> {res['async_tput']} (from {stream_json})", flush=True)
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse stream json {stream_json}: {ex}", flush=True)

        # 3. Process text summaries (stream_benchmark_summary.txt)
        for txt_file in sdir.glob("**/stream_benchmark_summary.txt"):
            try:
                os_name, arch, compiler = parse_platform_from_path(txt_file)
                content = txt_file.read_text(encoding="utf-8", errors="ignore")
                res = get_or_create_result(platform_results_map, os_name, arch, compiler)

                stream_match = re.search(r"parseAsync.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Data Bandwidth\s*:\s*([\d,.]+)\s*MB/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if stream_match and res["async_tput_num"] == 0:
                    tput, bw, lat, unit = stream_match.groups()
                    tput_f = float(tput.replace(',', ''))
                    lat_f = float(lat.replace(',', ''))
                    res["async_tput_num"] = tput_f
                    res["async_lat_num"] = lat_f
                    res["async_tput"] = f"{tput_f:,.2f} msg/s"
                    res["bandwidth"] = f"{float(bw.replace(',', '')):.2f} MB/s"
                    res["async_lat"] = format_latency(lat_f, unit)

                single_match = re.search(r"parseFromBuffer.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if single_match and res["single_tput_num"] == 0:
                    tput, lat, unit = single_match.groups()
                    tput_f = float(tput.replace(',', ''))
                    lat_f = float(lat.replace(',', ''))
                    res["single_tput_num"] = tput_f
                    res["single_lat_num"] = lat_f
                    res["single_tput"] = f"{tput_f:,.2f} msg/s"
                    res["single_lat"] = format_latency(lat_f, unit)

                res["platform_arch"] = format_platform_arch_label(res["os"], res["arch"], res.get("os_release", ""))
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse text summary {txt_file}: {ex}", flush=True)

        # 4. Process Google Benchmark JSON files matching benchmark_results_*.json
        for jfile in sdir.glob("**/benchmark_results_*.json"):
            try:
                data = json.loads(jfile.read_text(encoding="utf-8"))
                os_name, arch, compiler = parse_platform_from_path(jfile)
                res = get_or_create_result(platform_results_map, os_name, arch, compiler)

                if "context" in data and not res.get("host_info"):
                    ctx = data["context"]
                    res["host_info"] = f"{ctx.get('host_name', os_name)} ({ctx.get('num_cpus', '')} Cores, {ctx.get('mhz_per_cpu', '')} MHz)"

                b_list = data.get("benchmarks", [])
                for b in b_list:
                    bname = b.get("name", "").lower()
                    items_sec = float(b.get("items_per_second", 0.0))
                    rtime = float(b.get("real_time", 0.0))
                    tunit = b.get("time_unit", "us")

                    if "async" in bname or "callback" in bname:
                        if items_sec > 0 and res["async_tput_num"] == 0:
                            res["async_tput_num"] = items_sec
                            res["async_tput"] = f"{items_sec:,.2f} msg/s"
                            res["bandwidth"] = f"{(items_sec * 2600) / 1024 / 1024:.2f} MB/s"
                        if rtime > 0 and res["async_lat_num"] == 0:
                            res["async_lat_num"] = rtime
                            res["async_lat"] = format_latency(rtime, tunit)
                    elif any(k in bname for k in ["single", "parsefrombuffer", "minimalresponse", "registerrequest", "invitewithsdp"]):
                        if items_sec > 0 and res["single_tput_num"] == 0:
                            res["single_tput_num"] = items_sec
                            res["single_tput"] = f"{items_sec:,.2f} msg/s"
                        if rtime > 0 and res["single_lat_num"] == 0:
                            res["single_lat_num"] = rtime
                            res["single_lat"] = format_latency(rtime, tunit)

                res["platform_arch"] = format_platform_arch_label(res["os"], res["arch"], res.get("os_release", ""))
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse JSON file {jfile}: {ex}", flush=True)

    # 5. Check repo_root recursively for artifacts if nothing was discovered yet
    if not any(r.get("async_tput_num", 0) > 0 for r in platform_results_map.values()):
        print("[publish_benchmarks] Performing deep scan of workspace for benchmark artifacts...", flush=True)
        exclude_dirs = {".git", ".cpmcache", "site", "venv", "install"}
        for root, dirs, files in os.walk(repo_root):
            dirs[:] = [d for d in dirs if d not in exclude_dirs]
            p_root = Path(root)
            if "host_info.json" in files:
                try:
                    hdata = json.loads((p_root / "host_info.json").read_text(encoding="utf-8"))
                    os_raw = hdata.get("platform") or hdata.get("os_name", "")
                    arch_raw = hdata.get("arch", "x64")
                    comp_raw = hdata.get("compiler", "")
                    res = get_or_create_result(platform_results_map, os_raw, arch_raw, comp_raw)
                    res["host_info"] = hdata.get("host_summary", "") or res["host_info"]
                    res["os_release"] = hdata.get("os_release", "") or res["os_release"]
                    res["platform_arch"] = format_platform_arch_label(res["os"], res["arch"], res["os_release"])
                    if comp_raw:
                        res["compiler"] = normalize_compiler(comp_raw, res["os"])
                except Exception:
                    pass

            if "stream_benchmark_results.json" in files:
                try:
                    stream_json = p_root / "stream_benchmark_results.json"
                    sdata = json.loads(stream_json.read_text(encoding="utf-8"))
                    os_name, arch, compiler = parse_platform_from_path(stream_json)
                    res = get_or_create_result(platform_results_map, os_name, arch, compiler)
                    if "stream_parse_async" in sdata:
                        spa = sdata["stream_parse_async"]
                        tput = float(spa.get("throughput_msg_per_sec", 0.0))
                        bw = float(spa.get("bandwidth_mb_per_sec", 0.0))
                        lat = float(spa.get("avg_latency_us", 0.0))
                        res["async_tput_num"] = tput
                        res["async_lat_num"] = lat
                        res["async_tput"] = f"{tput:,.2f} msg/s"
                        res["bandwidth"] = f"{bw:.2f} MB/s"
                        res["async_lat"] = format_latency(lat, "us")
                    if "single_message_parse" in sdata:
                        smp = sdata["single_message_parse"]
                        tput = float(smp.get("throughput_msg_per_sec", 0.0))
                        lat = float(smp.get("avg_latency_us", 0.0))
                        res["single_tput_num"] = tput
                        res["single_lat_num"] = lat
                        res["single_tput"] = f"{tput:,.2f} msg/s"
                        res["single_lat"] = format_latency(lat, "us")
                    res["platform_arch"] = format_platform_arch_label(res["os"], res["arch"], res.get("os_release", ""))
                except Exception:
                    pass

    platform_results = list(platform_results_map.values())
    print(f"[publish_benchmarks] Total candidate platform entries: {len(platform_results)}", flush=True)
    for pr in platform_results:
        print(f"  - {pr.get('platform_arch')}: Throughput={pr.get('async_tput')}, Latency={pr.get('async_lat')}, Host={pr.get('host_info')}", flush=True)

    # Update docs/architecture/benchmarks.md strictly with collected platform results
    update_benchmarks_doc(repo_root, platform_results, require_all=args.require_all, required_str=args.required_platforms)

    # Primary benchmark report generator
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
