#!/usr/bin/env python3
"""
Benchmark Report Generator
Converts Google Benchmark JSON output to JUnit XML and HTML formats with visual charts.

Usage:
    python3 benchmark_report_generator.py <input_json> [output_dir]
"""

import json
import sys
from datetime import datetime
from pathlib import Path


def generate_junit_xml(data: dict) -> str:
    """Generate JUnit XML format from benchmark data."""
    junit_xml = '<?xml version="1.0" encoding="UTF-8"?>\n'
    junit_xml += '<testsuites>\n'
    junit_xml += f'  <testsuite name="sip2json_benchmarks" tests="{len(data["benchmarks"])}">\n'
    
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        time_ms = benchmark['real_time']
        time_unit = benchmark['time_unit']
        
        junit_xml += f'    <testcase classname="sip2json_benchmarks" name="{name}" time="{time_ms / 1000.0}">\n'
        junit_xml += '      <properties>\n'
        junit_xml += f'        <property name="time_unit" value="{time_unit}"/>\n'
        junit_xml += f'        <property name="cpu_time" value="{benchmark.get("cpu_time", 0)}"/>\n'
        junit_xml += f'        <property name="iterations" value="{benchmark.get("iterations", 0)}"/>\n'
        junit_xml += '      </properties>\n'
        junit_xml += '    </testcase>\n'
    
    junit_xml += '  </testsuite>\n'
    junit_xml += '</testsuites>\n'
    
    return junit_xml


def generate_html_report(data: dict) -> str:
    """Generate HTML report with visual interactive charts from benchmark data."""
    total_benchmarks = len(data['benchmarks'])
    total_iterations = sum(int(b.get('iterations', 0)) for b in data['benchmarks'])
    fastest_time = round(min(b['real_time'] for b in data['benchmarks']), 2)
    slowest_time = round(max(b['real_time'] for b in data['benchmarks']), 2)
    time_unit = data['benchmarks'][0]['time_unit'] if data['benchmarks'] else "ns"

    # Categorize benchmarks for charts
    parse_rate_labels = []
    parse_rate_values = []
    
    stream_arch_labels = []
    stream_arch_values = []
    
    multithread_labels = []
    multithread_values = []

    for b in data['benchmarks']:
        name = b['name']
        items_sec = b.get('items_per_second', 0)

        # Single-stream architecture benchmarks
        if "BM_SimulatedStream" in name:
            label = name.replace("BM_SimulatedStream_", "").replace("_", " ")
            stream_arch_labels.append(label)
            stream_arch_values.append(round(items_sec, 0))
        # Multi-threaded async benchmarks
        elif "BM_MultiThreaded" in name:
            multithread_labels.append(name.replace("BM_MultiThreaded", "").replace("_", " "))
            multithread_values.append(round(items_sec, 0))
        # Core parsing benchmarks
        elif any(k in name for k in ["BM_ParseMinimalResponse", "BM_ParseRegisterRequest", "BM_ParseInviteWithSDP", "BM_ParseInviteComplexSDP", "BM_ParseNotifyLF"]):
            parse_rate_labels.append(name.replace("BM_Parse", ""))
            parse_rate_values.append(round(items_sec, 0))

    html_report = f'''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>sip2json Benchmark Performance Report</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * {{ margin: 0; padding: 0; box-sizing: border-box; }}
        body {{ font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif; background: #f4f6f9; color: #333; }}
        .container {{ max-width: 1400px; margin: 0 auto; padding: 30px 20px; }}
        header {{ background: linear-gradient(135deg, #4f46e5 0%, #7c3aed 100%); color: white; padding: 35px; border-radius: 12px; margin-bottom: 30px; box-shadow: 0 10px 15px -3px rgba(0,0,0,0.1); }}
        header h1 {{ font-size: 2.4em; margin-bottom: 8px; font-weight: 700; }}
        header p {{ font-size: 1.1em; opacity: 0.9; }}
        .summary {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(240px, 1fr)); gap: 20px; margin-bottom: 30px; }}
        .summary-card {{ background: white; padding: 22px; border-radius: 10px; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.05); border-left: 5px solid #4f46e5; }}
        .summary-card h3 {{ color: #6b7280; margin-bottom: 8px; font-size: 0.85em; text-transform: uppercase; letter-spacing: 0.05em; }}
        .summary-card .value {{ font-size: 2.1em; font-weight: 700; color: #1f2937; }}
        .summary-card .unit {{ font-size: 0.9em; color: #9ca3af; margin-left: 4px; }}
        .charts-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(600px, 1fr)); gap: 25px; margin-bottom: 35px; }}
        .chart-card {{ background: white; padding: 25px; border-radius: 10px; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.05); }}
        .chart-card h3 {{ margin-bottom: 20px; color: #1f2937; font-size: 1.2em; font-weight: 600; border-bottom: 2px solid #f3f4f6; padding-bottom: 10px; }}
        table {{ width: 100%; border-collapse: collapse; background: white; border-radius: 10px; overflow: hidden; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.05); margin-bottom: 35px; }}
        thead {{ background: #f9fafb; border-bottom: 2px solid #e5e7eb; }}
        th {{ padding: 16px; text-align: left; font-weight: 600; color: #374151; font-size: 0.9em; text-transform: uppercase; }}
        td {{ padding: 14px 16px; border-bottom: 1px solid #f3f4f6; font-size: 0.95em; }}
        tbody tr:hover {{ background: #f9fafb; }}
        .benchmark-name {{ font-family: "JetBrains Mono", "Courier New", monospace; font-size: 0.9em; color: #4f46e5; font-weight: 600; }}
        .time-value {{ font-family: "JetBrains Mono", "Courier New", monospace; text-align: right; font-weight: 600; }}
        .iterations {{ text-align: center; color: #6b7280; }}
        .cpu-time {{ text-align: right; color: #6b7280; }}
        footer {{ text-align: center; color: #9ca3af; margin-top: 50px; padding-top: 20px; border-top: 1px solid #e5e7eb; font-size: 0.9em; }}
        .section-title {{ font-size: 1.6em; font-weight: 700; color: #1f2937; margin: 35px 0 20px 0; }}
        a {{ color: #4f46e5; text-decoration: none; font-weight: 500; }}
        a:hover {{ text-decoration: underline; }}
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>sip2json Benchmark Performance Report</h1>
            <p>Generated on {datetime.now().strftime("%Y-%m-%d %H:%M:%S")} | Modern C++23 Header-Only SIP Parser</p>
        </header>
        
        <div class="summary">
            <div class="summary-card">
                <h3>Total Benchmarks</h3>
                <div class="value">{total_benchmarks}</div>
            </div>
            <div class="summary-card">
                <h3>Total Iterations</h3>
                <div class="value">{total_iterations:,}</div>
            </div>
            <div class="summary-card">
                <h3>Fastest Operation</h3>
                <div class="value">{fastest_time} <span class="unit">{time_unit}</span></div>
            </div>
            <div class="summary-card">
                <h3>Slowest Operation</h3>
                <div class="value">{slowest_time} <span class="unit">{time_unit}</span></div>
            </div>
        </div>

        <h2 class="section-title">Visual Performance Graphs</h2>
        <div class="charts-grid">
            <div class="chart-card">
                <h3>Single-Threaded Parsing Rate (Messages / Sec)</h3>
                <canvas id="parseRateChart"></canvas>
            </div>
            <div class="chart-card">
                <h3>Single Stream Architecture Comparison (Messages / Sec)</h3>
                <canvas id="streamArchChart"></canvas>
            </div>
        </div>

        <div class="charts-grid">
            <div class="chart-card" style="grid-column: 1 / -1;">
                <h3>Multi-Threaded Stream Scaling (Messages / Sec)</h3>
                <canvas id="multiThreadChart"></canvas>
            </div>
        </div>
        
        <h2 class="section-title">Detailed Benchmark Data</h2>
        <table>
            <thead>
                <tr>
                    <th>Benchmark Name</th>
                    <th>Real Time</th>
                    <th>CPU Time</th>
                    <th>Iterations</th>
                    <th>Items/Sec (Throughput)</th>
                </tr>
            </thead>
            <tbody>
'''
    
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        real_time = benchmark['real_time']
        cpu_time = benchmark.get('cpu_time', 0)
        iterations = benchmark.get('iterations', 0)
        t_unit = benchmark['time_unit']
        items_per_sec = benchmark.get('items_per_second', 0)
        
        html_report += f'''
                <tr>
                    <td><span class="benchmark-name">{name}</span></td>
                    <td class="time-value">{real_time:.2f} {t_unit}</td>
                    <td class="cpu-time">{cpu_time:.2f} {t_unit}</td>
                    <td class="iterations">{iterations:,}</td>
                    <td class="time-value">{items_per_sec:,.0f}</td>
                </tr>
'''
    
    html_report += f'''
            </tbody>
        </table>
        
        <footer>
            <p>sip2json Benchmark Suite | Siddiq Software LLC</p>
            <p>Repository: <a href="https://github.com/SiddiqSoft/sip2json">github.com/SiddiqSoft/sip2json</a></p>
        </footer>
    </div>

    <script>
        // Chart 1: Parsing Rate
        const ctxParse = document.getElementById('parseRateChart').getContext('2d');
        new Chart(ctxParse, {{
            type: 'bar',
            data: {{
                labels: {json.dumps(parse_rate_labels)},
                datasets: [{{
                    label: 'Parse Rate (Messages / Sec)',
                    data: {json.dumps(parse_rate_values)},
                    backgroundColor: 'rgba(79, 70, 229, 0.85)',
                    borderColor: '#4f46e5',
                    borderWidth: 1,
                    borderRadius: 6
                }}]
            }},
            options: {{
                responsive: true,
                plugins: {{ legend: {{ display: false }} }},
                scales: {{ y: {{ beginAtZero: true, title: {{ display: true, text: 'Messages / Sec' }} }} }}
            }}
        }});

        // Chart 2: Single Stream Architecture Comparison
        const ctxStream = document.getElementById('streamArchChart').getContext('2d');
        new Chart(ctxStream, {{
            type: 'bar',
            data: {{
                labels: {json.dumps(stream_arch_labels)},
                datasets: [{{
                    label: 'Throughput (Messages / Sec)',
                    data: {json.dumps(stream_arch_values)},
                    backgroundColor: ['#10b981', '#3b82f6', '#ef4444', '#f59e0b'],
                    borderRadius: 6
                }}]
            }},
            options: {{
                responsive: true,
                plugins: {{ legend: {{ display: false }} }},
                scales: {{ y: {{ beginAtZero: true, title: {{ display: true, text: 'Messages / Sec' }} }} }}
            }}
        }});

        // Chart 3: Multi-Threaded Scaling
        const ctxMulti = document.getElementById('multiThreadChart').getContext('2d');
        new Chart(ctxMulti, {{
            type: 'bar',
            data: {{
                labels: {json.dumps(multithread_labels)},
                datasets: [{{
                    label: 'Aggregate Multi-Thread Throughput (Messages / Sec)',
                    data: {json.dumps(multithread_values)},
                    backgroundColor: 'rgba(124, 58, 237, 0.85)',
                    borderColor: '#7c3aed',
                    borderRadius: 6
                }}]
            }},
            options: {{
                responsive: true,
                plugins: {{ legend: {{ display: false }} }},
                scales: {{ y: {{ beginAtZero: true, title: {{ display: true, text: 'Aggregate Throughput (Msgs / Sec)' }} }} }}
            }}
        }});
    </script>
</body>
</html>
'''
    
    return html_report


def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: python3 benchmark_report_generator.py <input_json> [output_dir]")
        sys.exit(1)
    
    input_json = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else "."
    
    # Ensure output directory exists
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    
    # Read benchmark data
    try:
        with open(input_json, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Error: Input file '{input_json}' not found", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in '{input_json}': {e}", file=sys.stderr)
        sys.exit(1)
    
    # Generate JUnit XML
    junit_xml = generate_junit_xml(data)
    junit_path = Path(output_dir) / "benchmark_results.xml"
    with open(junit_path, 'w') as f:
        f.write(junit_xml)
    print(f"JUnit XML generated: {junit_path}")
    
    # Generate HTML report with visual graphs
    html_report = generate_html_report(data)
    html_path = Path(output_dir) / "benchmark_report.html"
    with open(html_path, 'w') as f:
        f.write(html_report)
    print(f"HTML Report with visual graphs generated: {html_path}")


if __name__ == "__main__":
    main()
