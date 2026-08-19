#!/usr/bin/env python3
"""
Publish Benchmarks Script
Automatically builds the sip2json_benchmarks target in Release mode (if needed),
executes the benchmark suite, generates JSON, JUnit XML, and interactive HTML reports,
and prepares the reports for documentation site publication.

Usage:
    python3 scripts/publish_benchmarks.py [--root REPO_ROOT] [--skip-build]
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

def run_cmd(cmd, cwd=None):
    """Helper to run command and log output."""
    print(f"[publish_benchmarks] Running: {' '.join(cmd)}")
    res = subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)
    if res.returncode != 0:
        print(f"[publish_benchmarks] Command failed with exit code {res.returncode}:\n{res.stderr}")
        return False
    return True

def main():
    parser = argparse.ArgumentParser(description="Publish sip2json benchmark reports")
    parser.add_argument("--root", type=str, help="Repository root path")
    parser.add_argument("--skip-build", action="store_true", help="Skip building benchmark binary if executable already exists")
    args = parser.parse_args()

    repo_root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parent.parent
    benchmarks_dir = repo_root / "benchmarks"
    docs_assets_dir = repo_root / "docs" / "assets"
    docs_assets_dir.mkdir(parents=True, exist_ok=True)

    # 1. Locate existing benchmark executable or build it via CMake
    possible_exe_locations = [
        repo_root / "build" / "Apple-Release" / "benchmarks" / "sip2json_benchmarks",
        repo_root / "build" / "Release" / "benchmarks" / "sip2json_benchmarks",
        repo_root / "build" / "benchmarks" / "sip2json_benchmarks",
        repo_root / "build" / "benchmarks" / "Release" / "sip2json_benchmarks.exe",
        repo_root / "build" / "benchmarks" / "sip2json_benchmarks.exe",
    ]

    bench_exe = None
    for loc in possible_exe_locations:
        if loc.exists() and os.access(loc, os.X_OK):
            bench_exe = loc
            break

    if not bench_exe or not args.skip_build:
        build_dir = repo_root / "build" / "Release"
        print(f"[publish_benchmarks] Configuring CMake in {build_dir}...")
        cfg_cmd = [
            "cmake", "-B", str(build_dir), "-S", str(repo_root),
            "-DCMAKE_BUILD_TYPE=Release",
            "-Dsip2json_BUILD_BENCHMARKS=ON",
            "-Dsip2json_BUILD_TESTS=OFF"
        ]
        if run_cmd(cfg_cmd):
            print(f"[publish_benchmarks] Building sip2json_benchmarks in Release mode...")
            build_cmd = ["cmake", "--build", str(build_dir), "--config", "Release", "--target", "sip2json_benchmarks"]
            if run_cmd(build_cmd):
                new_exe = build_dir / "benchmarks" / "sip2json_benchmarks"
                if new_exe.exists():
                    bench_exe = new_exe

    if not bench_exe or not bench_exe.exists():
        print("[publish_benchmarks] Warning: Benchmark executable could not be found or built. Skipping benchmark execution.")
        return

    # 2. Run benchmark executable and output JSON
    json_out_path = benchmarks_dir / "benchmark_results.json"
    run_bench_cmd = [
        str(bench_exe),
        f"--benchmark_out={json_out_path}",
        "--benchmark_out_format=json"
    ]
    print(f"[publish_benchmarks] Executing benchmark suite: {bench_exe}")
    if not run_cmd(run_bench_cmd):
        print("[publish_benchmarks] Error: Benchmark execution failed.")
        return

    # 3. Generate XML and HTML reports using benchmark_report_generator.py
    generator_script = benchmarks_dir / "benchmark_report_generator.py"
    if generator_script.exists():
        gen_cmd = [sys.executable, str(generator_script), str(json_out_path), str(benchmarks_dir)]
        if run_cmd(gen_cmd):
            print(f"[publish_benchmarks] Generated benchmark reports in {benchmarks_dir}")

            # 4. Copy HTML report into docs/assets/ so it is published into the documentation website
            html_report = benchmarks_dir / "benchmark_report.html"
            if html_report.exists():
                published_html = docs_assets_dir / "benchmark_report.html"
                shutil.copy2(html_report, published_html)
                print(f"[publish_benchmarks] Published interactive HTML report to: {published_html}")

if __name__ == "__main__":
    main()
