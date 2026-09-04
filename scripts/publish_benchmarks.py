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


def format_platform_arch_label(os_name: str, arch: str, os_release: str = "") -> str:
    """Derive clean platform and architecture label using actual host OS release if available."""
    arch_norm = arch.lower().strip()
    if os_release:
        return f"{os_release} ({arch_norm})"

    os_low = os_name.lower().strip()
    if os_low in ("macos", "darwin", "apple"):
        return f"Apple macOS ({arch_norm})"
    elif os_low == "windows":
        return f"Microsoft Windows ({arch_norm})"
    elif os_low == "linux":
        return f"Red Hat Enterprise Linux ({arch_norm})"
    return f"{os_name} ({arch_norm})"


def parse_platform_from_path(file_path: Path) -> tuple:
    """Extract OS, Arch, and Compiler from file path or parent directories."""
    path_str = str(file_path).replace("\\", "/")

    m = re.search(r"benchmark[-_]results[-_]([A-Za-z]+)[-_](arm64|x64|x86)[-_]([A-Za-z0-9]+)", path_str, re.IGNORECASE)
    if m:
        os_raw = m.group(1)
        os_name = "macOS" if os_raw.lower() in ("darwin", "apple", "macos") else os_raw.capitalize()
        arch_name = m.group(2).lower()
        comp_raw = m.group(3)
        compiler_name = "AppleClang" if os_name == "macOS" else comp_raw
        return os_name, arch_name, compiler_name

    m = re.search(r"build/([A-Za-z0-9_-]+)", path_str, re.IGNORECASE)
    if m:
        preset = m.group(1).lower()
        if "apple" in preset or "darwin" in preset or "macos" in preset:
            arch = "arm64" if "arm64" in preset else ("x64" if "x64" in preset else ("arm64" if platform.machine() in ("arm64", "aarch64") else "x64"))
            return "macOS", arch, "AppleClang"
        elif "windows" in preset or "win" in preset:
            arch = "arm64" if "arm64" in preset else "x64"
            return "Windows", arch, "MSVC"
        elif "linux" in preset:
            arch = "arm64" if "arm64" in preset else "x64"
            compiler = "GCC" if "gcc" in preset else "Clang"
            return "Linux", arch, compiler

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

            lines.append('  <div class="chart-row">')
            lines.append('    <div class="chart-meta">')
            lines.append(f'      <span class="chart-title">{r["platform_arch"]} {pill}</span>')
            lines.append(f'      <span class="chart-val">{tput_num:,.0f} msg/s</span>')
            lines.append('    </div>')
            lines.append('    <div class="chart-track">')
            lines.append(f'      <div class="{bar_cls}" style="width: {width_pct}%;"></div>')
            lines.append('    </div>')
            lines.append('  </div>')
    lines.append('</div>')
    lines.append('')

    # Chart 2: Per-message processing latency
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

            lines.append('  <div class="chart-row">')
            lines.append('    <div class="chart-meta">')
            lines.append(f'      <span class="chart-title">{r["platform_arch"]} {pill}</span>')
            lines.append(f'      <span class="chart-val">{lat_num:.2f} µs</span>')
            lines.append('    </div>')
            lines.append('    <div class="chart-track">')
            lines.append(f'      <div class="{bar_cls}" style="width: {width_pct}%;"></div>')
            lines.append('    </div>')
            lines.append('  </div>')
    lines.append('</div>')
    lines.append('')

    # Chart 3: Single-message parsing throughput
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

            lines.append('  <div class="chart-row">')
            lines.append('    <div class="chart-meta">')
            lines.append(f'      <span class="chart-title">{r["platform_arch"]} {pill}</span>')
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

    # Filter for results with valid throughput data
    valid_results = [
        r for r in platform_results
        if r.get("async_tput_num", 0) > 0 or r.get("single_tput_num", 0) > 0 or (r.get("async_tput", "N/A") != "N/A" and r.get("async_tput") != "0.00 msg/s")
    ]

    def sort_key(r):
        os_name = r.get("os", "").lower()
        arch = r.get("arch", "").lower()
        pair = (os_name, arch)
        order = [
            ("macos", "arm64"),
            ("darwin", "arm64"),
            ("windows", "arm64"),
            ("linux", "arm64"),
            ("linux", "x64"),
            ("windows", "x64")
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
    print(f"[publish_benchmarks] Updated {doc_path} with pipeline-derived benchmark metrics.", flush=True)


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

        # 1. First, search for host_info.json files
        for host_file in sdir.glob("**/host_info.json"):
            try:
                hdata = json.loads(host_file.read_text(encoding="utf-8"))
                os_raw = hdata.get("platform") or hdata.get("os_name", "")
                os_name = "macOS" if os_raw.lower() in ("darwin", "apple", "macos") else os_raw.capitalize()
                arch = hdata.get("arch", "x64").lower()
                compiler = hdata.get("compiler", "Clang")
                key = (os_name.lower(), arch.lower(), compiler.lower())

                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A", "host_info": "",
                    "async_tput_num": 0.0, "async_lat_num": 0.0,
                    "single_tput_num": 0.0, "single_lat_num": 0.0
                })

                res["host_info"] = hdata.get("host_summary", "")
                res["os_release"] = hdata.get("os_release", "")
                res["platform_arch"] = format_platform_arch_label(os_name, arch, res["os_release"])
                platform_results_map[key] = res
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse host_info.json {host_file}: {ex}", flush=True)

        # 2. Process stream_benchmark_results.json files
        for stream_json in sdir.glob("**/stream_benchmark_results.json"):
            try:
                sdata = json.loads(stream_json.read_text(encoding="utf-8"))
                os_name, arch, compiler = parse_platform_from_path(stream_json)
                key = (os_name.lower(), arch.lower(), compiler.lower())

                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A", "host_info": "",
                    "async_tput_num": 0.0, "async_lat_num": 0.0,
                    "single_tput_num": 0.0, "single_lat_num": 0.0
                })

                if "stream_parse_async" in sdata:
                    spa = sdata["stream_parse_async"]
                    tput = spa.get("throughput_msg_per_sec", 0.0)
                    bw = spa.get("bandwidth_mb_per_sec", 0.0)
                    lat = spa.get("avg_latency_us", 0.0)
                    res["async_tput_num"] = tput
                    res["async_lat_num"] = lat
                    res["async_tput"] = f"{tput:,.2f} msg/s"
                    res["bandwidth"] = f"{bw:.2f} MB/s"
                    res["async_lat"] = format_latency(lat, "us")

                if "single_message_parse" in sdata:
                    smp = sdata["single_message_parse"]
                    tput = smp.get("throughput_msg_per_sec", 0.0)
                    lat = smp.get("avg_latency_us", 0.0)
                    res["single_tput_num"] = tput
                    res["single_lat_num"] = lat
                    res["single_tput"] = f"{tput:,.2f} msg/s"
                    res["single_lat"] = format_latency(lat, "us")

                platform_results_map[key] = res
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse stream json {stream_json}: {ex}", flush=True)

        # 3. Process text summaries (stream_benchmark_summary.txt)
        for txt_file in sdir.glob("**/stream_benchmark_summary.txt"):
            try:
                os_name, arch, compiler = parse_platform_from_path(txt_file)
                if os_name == "Linux" and "Windows" in str(txt_file): os_name = "Windows"

                content = txt_file.read_text(encoding="utf-8", errors="ignore")
                key = (os_name.lower(), arch.lower(), compiler.lower())
                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A", "host_info": "",
                    "async_tput_num": 0.0, "async_lat_num": 0.0,
                    "single_tput_num": 0.0, "single_lat_num": 0.0
                })

                stream_match = re.search(r"parseAsync.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Data Bandwidth\s*:\s*([\d,.]+)\s*MB/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if stream_match:
                    tput, bw, lat, unit = stream_match.groups()
                    tput_f = float(tput.replace(',', ''))
                    lat_f = float(lat.replace(',', ''))
                    res["async_tput_num"] = tput_f
                    res["async_lat_num"] = lat_f
                    res["async_tput"] = f"{tput_f:,.2f} msg/s"
                    res["bandwidth"] = f"{float(bw.replace(',', '')):.2f} MB/s"
                    res["async_lat"] = format_latency(lat_f, unit)

                single_match = re.search(r"parseFromBuffer.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if single_match:
                    tput, lat, unit = single_match.groups()
                    tput_f = float(tput.replace(',', ''))
                    lat_f = float(lat.replace(',', ''))
                    res["single_tput_num"] = tput_f
                    res["single_lat_num"] = lat_f
                    res["single_tput"] = f"{tput_f:,.2f} msg/s"
                    res["single_lat"] = format_latency(lat_f, unit)

                platform_results_map[key] = res
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse text summary {txt_file}: {ex}", flush=True)

        # 4. Process Google Benchmark JSON files matching benchmark_results_*.json
        for jfile in sdir.glob("**/benchmark_results_*.json"):
            try:
                data = json.loads(jfile.read_text(encoding="utf-8"))
                os_name, arch, compiler = parse_platform_from_path(jfile)
                key = (os_name.lower(), arch.lower(), compiler.lower())
                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A", "host_info": "",
                    "async_tput_num": 0.0, "async_lat_num": 0.0,
                    "single_tput_num": 0.0, "single_lat_num": 0.0
                })

                if "context" in data and not res.get("host_info"):
                    ctx = data["context"]
                    res["host_info"] = f"{ctx.get('host_name', os_name)} ({ctx.get('num_cpus', '')} Cores, {ctx.get('mhz_per_cpu', '')} MHz)"

                b_list = data.get("benchmarks", [])
                for b in b_list:
                    bname = b.get("name", "").lower()
                    items_sec = b.get("items_per_second", 0.0)
                    rtime = b.get("real_time", 0.0)
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

                platform_results_map[key] = res
            except Exception as ex:
                print(f"[publish_benchmarks] Could not parse JSON file {jfile}: {ex}", flush=True)

    platform_results = list(platform_results_map.values())

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
