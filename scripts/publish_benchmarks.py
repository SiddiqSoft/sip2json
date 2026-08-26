#!/usr/bin/env python3
"""
Publish Benchmarks Script
Automatically locates benchmark outputs across CI build matrix runners (Windows x64/arm64, Linux x64/arm64, macOS),
compiles multi-platform performance reports, and dynamically updates docs/features/benchmarks.md.

Usage:
    python3 scripts/publish_benchmarks.py [--root REPO_ROOT] [--skip-build] [--skip-exec] [--require-all]
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

def get_system_ram_gb() -> str:
    """Derive total system memory in GB uniformly across OS platforms."""
    try:
        if hasattr(os, "sysconf") and "SC_PAGE_SIZE" in os.sysconf_names and "SC_PHYS_PAGES" in os.sysconf_names:
            bytes_ram = os.sysconf("SC_PAGE_SIZE") * os.sysconf("SC_PHYS_PAGES")
            gb = round(bytes_ram / (1024**3))
            return f"{gb} GB RAM"
    except Exception:
        pass
    if platform.system() == "Linux":
        try:
            mem_bytes = os.sysconf('SC_PAGE_SIZE') * os.sysconf('SC_PHYS_PAGES')
            return f"{round(mem_bytes / (1024**3))} GB RAM"
        except Exception:
            pass
    elif platform.system() == "Windows":
        try:
            import ctypes
            class MEMORYSTATUSEX(ctypes.Structure):
                _fields_ = [
                    ("dwLength", ctypes.c_ulong),
                    ("dwMemoryLoad", ctypes.c_ulong),
                    ("ullTotalPhys", ctypes.c_ulonglong),
                    ("ullAvailPhys", ctypes.c_ulonglong),
                    ("ullTotalPageFile", ctypes.c_ulonglong),
                    ("ullAvailPageFile", ctypes.c_ulonglong),
                    ("ullTotalVirtual", ctypes.c_ulonglong),
                    ("ullAvailVirtual", ctypes.c_ulonglong),
                    ("sullAvailExtendedVirtual", ctypes.c_ulonglong),
                ]
            stat = MEMORYSTATUSEX()
            stat.dwLength = ctypes.sizeof(MEMORYSTATUSEX)
            if ctypes.windll.kernel32.GlobalMemoryStatusEx(ctypes.byref(stat)):
                gb = round(stat.ullTotalPhys / (1024**3))
                return f"{gb} GB RAM"
        except Exception:
            pass
    return ""

def get_host_runner_info() -> str:
    """Dynamically derive platform OS, architecture, CPU count, and RAM (strictly NO domain names or hostnames)."""
    sys_name = platform.system()
    arch_name = platform.machine() or platform.processor() or "x64"
    cpu_count = os.cpu_count() or 1
    ram_str = get_system_ram_gb()
    
    os_detail = f"{sys_name} {platform.release()}"
    
    if sys_name == "Linux":
        try:
            if hasattr(platform, "freedesktop_os_release"):
                info = platform.freedesktop_os_release()
                os_detail = info.get("PRETTY_NAME", os_detail)
            elif Path("/etc/os-release").exists():
                for line in Path("/etc/os-release").read_text().splitlines():
                    if line.startswith("PRETTY_NAME="):
                        os_detail = line.split("=", 1)[1].strip('"\'')
                        break
        except Exception:
            pass

    elif sys_name == "Windows":
        try:
            cmd = ["powershell", "-NoProfile", "-Command", "(Get-CimInstance Win32_OperatingSystem).Caption"]
            res = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
            if res.returncode == 0 and res.stdout.strip():
                os_detail = res.stdout.strip()
            else:
                win_ver = platform.win32_ver()
                if win_ver[0]:
                    os_detail = f"Windows {win_ver[0]} (Build {win_ver[1]})"
        except Exception:
            pass

    elif sys_name == "Darwin":
        try:
            mac_ver = platform.mac_ver()[0]
            if mac_ver:
                os_detail = f"macOS {mac_ver}"
        except Exception:
            pass

    specs = [arch_name]
    if cpu_count:
        specs.append(f"{cpu_count} CPU Cores" if cpu_count > 1 else "1 CPU Core")
    if ram_str:
        specs.append(ram_str)

    return f"{os_detail} ({', '.join(specs)})"

def sanitize_host_info(raw_info: str) -> str:
    """Strip any hostname, domain name, or FQDN from host runner descriptions, keeping only platform and CPU/memory."""
    if not raw_info:
        return ""
    # Remove 'on `hostname`' or 'on hostname'
    cleaned = re.sub(r"\s+on\s+`?[^`\s]+`?", "", raw_info).strip()
    # Remove FQDN / domain names
    cleaned = re.sub(r"[a-zA-Z0-9_-]+\.[a-zA-Z0-9_.-]+", "", cleaned).strip()
    return cleaned

def format_latency(val_float: float, unit: str = "us") -> str:
    """Convert latency into human-readable microseconds or milliseconds uniformly (never nanoseconds)."""
    unit_lower = unit.lower().strip()
    # Normalize to microseconds
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

def parse_platform_from_filename(filename_str: str) -> tuple:
    """Extract OS, Arch, and Compiler from benchmark JSON/TXT artifact filename if available."""
    name = Path(filename_str).stem
    if name.startswith("benchmark_results_"):
        parts = name.replace("benchmark_results_", "").split("_")
        os_name = parts[0] if len(parts) > 0 else "Linux"
        arch_name = parts[1] if len(parts) > 1 else "x64"
        compiler_name = parts[2] if len(parts) > 2 else "Clang/MSVC"
        return os_name, arch_name, compiler_name
    elif "Linux" in filename_str:
        arch = "arm64" if "arm64" in filename_str else "x64"
        compiler = "GCC" if "GCC" in filename_str else "Clang"
        return "Linux", arch, compiler
    elif "Windows" in filename_str:
        arch = "arm64" if "arm64" in filename_str else "x64"
        return "Windows", arch, "MSVC"
    elif "macOS" in filename_str or "Darwin" in filename_str or "Apple" in filename_str:
        return "macOS", "arm64", "AppleClang"
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

def format_os_name(os_name: str) -> str:
    """Format OS names with proper capitalization."""
    low = os_name.lower().strip()
    if low in ("macos", "darwin", "apple", "osx"):
        return "macOS"
    elif low == "windows":
        return "Windows"
    elif low == "linux":
        return "Linux"
    return os_name.capitalize()

def update_benchmarks_doc(repo_root: Path, platform_results: list, require_all: bool = False, required_str: str = ""):
    """Dynamically update docs/features/benchmarks.md between PIPELINE_BENCHMARKS markers with host info embedded in the table matrix."""
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

    table_lines = [
        start_marker,
        "## 1. Multi-Platform & Cross-Architecture Pipeline Benchmark Matrix",
        "",
        "*Empirical build pipeline measurements collected dynamically across live matrix runners (Build Version `{ version }`):*",
        ""
    ]

    if platform_results:
        # Sort results: Linux, Windows, macOS, then Arch, then Compiler
        os_order = ["Linux", "Windows", "macOS"]
        sorted_results = sorted(
            platform_results,
            key=lambda r: (
                os_order.index(format_os_name(r.get("os", "Linux"))) if format_os_name(r.get("os", "Linux")) in os_order else 99,
                r.get("arch", ""),
                r.get("compiler", "")
            )
        )

        table_lines.append("| OS | Arch | Compiler | Host Environment | Stream Throughput | Bandwidth | Stream Latency | Single Msg Throughput | Single Latency |")
        table_lines.append("| :--- | :---: | :---: | :--- | :---: | :---: | :---: | :---: | :---: |")

        for res in sorted_results:
            os_name = format_os_name(res.get("os", "Linux"))
            arch = res.get("arch", "x64")
            compiler = res.get("compiler", "Clang")
            host_info = sanitize_host_info(res.get("host_info", "")) or get_host_runner_info()
            async_tput = res.get("async_tput", "N/A")
            bandwidth = res.get("bandwidth", "N/A")
            async_lat = res.get("async_lat", "N/A")
            single_tput = res.get("single_tput", "N/A")
            single_lat = res.get("single_lat", "N/A")

            table_lines.append(
                f"| **{os_name}** | **{arch}** | {compiler} | {host_info} | **{async_tput}** | **{bandwidth}** | **{async_lat}** | **{single_tput}** | **{single_lat}** |"
            )
        table_lines.append("")
    else:
        # Strictly DO NOT fake missing platforms or architectures!
        table_lines.append('!!! info "Dynamic CI Matrix Benchmarks"')
        table_lines.append("    Empirical multi-platform benchmarks are collected automatically during CI build pipeline execution across Linux (x64/arm64, Clang/GCC) and Windows (x64/arm64, MSVC).")
        table_lines.append("")

    table_lines.append(end_marker)
    new_section = "\n".join(table_lines)

    # Replace content between markers
    pattern = re.compile(f"{re.escape(start_marker)}.*?{re.escape(end_marker)}", re.DOTALL)
    updated_content = pattern.sub(new_section, content)

    if updated_content == content:
        return

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
                os_name, arch, compiler = parse_platform_from_filename(str(txt_file.parent.name) + " " + str(txt_file))
                
                content = txt_file.read_text(encoding="utf-8", errors="ignore")
                key = (os_name, arch, compiler)
                res = platform_results_map.get(key, {
                    "os": os_name, "arch": arch, "compiler": compiler,
                    "async_tput": "N/A", "bandwidth": "N/A", "async_lat": "N/A",
                    "single_tput": "N/A", "single_lat": "N/A", "host_info": ""
                })

                host_match = re.search(r"\[HOST INFO\]\s*(.*)", content)
                if host_match:
                    raw_host = host_match.group(1).strip()
                    res["host_info"] = sanitize_host_info(raw_host)
                    if "AppleClang" in raw_host or "macOS" in raw_host or "Apple" in raw_host:
                        os_name, arch, compiler = "macOS", "arm64", "AppleClang"
                    elif "MSVC" in raw_host or "Windows" in raw_host:
                        os_name = "Windows"
                        arch = "arm64" if "arm64" in raw_host else "x64"
                        compiler = "MSVC"
                    elif "Linux" in raw_host:
                        os_name = "Linux"
                        arch = "arm64" if "arm64" in raw_host else "x64"
                        compiler = "GCC" if "GCC" in raw_host else "Clang"
                    key = (os_name, arch, compiler)
                    res["os"] = os_name
                    res["arch"] = arch
                    res["compiler"] = compiler

                stream_match = re.search(r"parseAsync.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Data Bandwidth\s*:\s*([\d,.]+)\s*MB/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if stream_match:
                    tput_s, bw_s, lat_s, unit = stream_match.groups()
                    tput = float(tput_s.replace(',', ''))
                    bw = float(bw_s.replace(',', ''))
                    lat = float(lat_s.replace(',', ''))
                    res["async_tput"] = f"{tput:,.2f} msg/s"
                    res["bandwidth"] = f"{bw:,.2f} MB/s"
                    res["async_lat"] = format_latency(lat, unit)

                single_match = re.search(r"parseFromBuffer.*?Throughput\s*:\s*([\d,.]+)\s*msg/sec.*?Avg Latency/Msg\s*:\s*([\d,.]+)\s*(\w+)/msg", content, re.DOTALL)
                if single_match:
                    tput_s, lat_s, unit = single_match.groups()
                    tput = float(tput_s.replace(',', ''))
                    lat = float(lat_s.replace(',', ''))
                    res["single_tput"] = f"{tput:,.2f} msg/s"
                    res["single_lat"] = format_latency(lat, unit)

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
                    "single_tput": "N/A", "single_lat": "N/A", "host_info": ""
                })

                if "context" in data:
                    ctx = data["context"]
                    cpu_cnt = ctx.get("num_cpus", "")
                    mhz = ctx.get("mhz_per_cpu", "")
                    specs = []
                    if cpu_cnt: specs.append(f"{cpu_cnt} CPU Cores")
                    if mhz: specs.append(f"{mhz} MHz")
                    ram_str = get_system_ram_gb()
                    if ram_str: specs.append(ram_str)
                    res["host_info"] = f"{os_name} ({', '.join(specs)})" if specs else os_name

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
                            res["async_lat"] = format_latency(rtime, tunit)
                    elif any(k in bname for k in ["single", "parsefrombuffer", "minimalresponse", "registerrequest", "invitewithsdp"]):
                        if items_sec > 0:
                            res["single_tput"] = f"{items_sec:,.2f} msg/s"
                        if rtime > 0:
                            res["single_lat"] = format_latency(rtime, tunit)

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
