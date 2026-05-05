#!/usr/bin/env python3
"""
Benchmark Report Generator
Converts Google Benchmark JSON output to JUnit XML and HTML formats.

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
    """Generate HTML report from benchmark data."""
    total_benchmarks = len(data['benchmarks'])
    total_iterations = sum(int(b.get('iterations', 0)) for b in data['benchmarks'])
    fastest_time = round(min(b['real_time'] for b in data['benchmarks']), 2)
    slowest_time = round(max(b['real_time'] for b in data['benchmarks']), 2)
    time_unit = data['benchmarks'][0]['time_unit']
    
    html_report = '''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>sip2json Benchmark Report</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif; background: #f5f5f5; color: #333; }
        .container { max-width: 1400px; margin: 0 auto; padding: 20px; }
        header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 30px; border-radius: 8px; margin-bottom: 30px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        header h1 { font-size: 2.5em; margin-bottom: 10px; }
        header p { font-size: 1.1em; opacity: 0.9; }
        .summary { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .summary-card { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-left: 4px solid #667eea; }
        .summary-card h3 { color: #667eea; margin-bottom: 10px; font-size: 0.9em; text-transform: uppercase; }
        .summary-card .value { font-size: 2em; font-weight: bold; color: #333; }
        .summary-card .unit { font-size: 0.9em; color: #999; margin-left: 5px; }
        table { width: 100%; border-collapse: collapse; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 30px; }
        thead { background: #f8f9fa; border-bottom: 2px solid #dee2e6; }
        th { padding: 15px; text-align: left; font-weight: 600; color: #495057; }
        td { padding: 12px 15px; border-bottom: 1px solid #dee2e6; }
        tbody tr:hover { background: #f8f9fa; }
        .benchmark-name { font-family: "Courier New", monospace; font-size: 0.9em; color: #667eea; font-weight: 500; }
        .time-value { font-family: "Courier New", monospace; text-align: right; }
        .iterations { text-align: center; color: #666; }
        .cpu-time { text-align: right; color: #666; }
        footer { text-align: center; color: #999; margin-top: 40px; padding-top: 20px; border-top: 1px solid #dee2e6; }
        .section-title { font-size: 1.5em; font-weight: 600; color: #333; margin: 30px 0 20px 0; }
        .stress-tests { background: #fff3cd; border-left: 4px solid #ffc107; padding: 15px; border-radius: 4px; margin-bottom: 20px; }
        .stress-tests strong { color: #856404; }
        .stress-tests ul { margin-left: 20px; margin-top: 10px; }
        a { color: #667eea; text-decoration: none; }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>sip2json Benchmark Report</h1>
            <p>Performance Analysis - Generated on ''' + datetime.now().strftime("%Y-%m-%d %H:%M:%S") + '''</p>
        </header>
        
        <div class="summary">
            <div class="summary-card">
                <h3>Total Benchmarks</h3>
                <div class="value">''' + str(total_benchmarks) + '''</div>
            </div>
            <div class="summary-card">
                <h3>Total Iterations</h3>
                <div class="value">''' + str(total_iterations) + '''</div>
            </div>
            <div class="summary-card">
                <h3>Fastest Benchmark</h3>
                <div class="value">''' + str(fastest_time) + ''' <span class="unit">''' + time_unit + '''</span></div>
            </div>
            <div class="summary-card">
                <h3>Slowest Benchmark</h3>
                <div class="value">''' + str(slowest_time) + ''' <span class="unit">''' + time_unit + '''</span></div>
            </div>
        </div>
        
        <h2 class="section-title">Benchmark Results</h2>
        <table>
            <thead>
                <tr>
                    <th>Benchmark Name</th>
                    <th>Real Time</th>
                    <th>CPU Time</th>
                    <th>Iterations</th>
                    <th>Items/Sec</th>
                </tr>
            </thead>
            <tbody>
'''
    
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        real_time = benchmark['real_time']
        cpu_time = benchmark.get('cpu_time', 0)
        iterations = benchmark.get('iterations', 0)
        time_unit = benchmark['time_unit']
        items_per_sec = benchmark.get('items_per_second', 0)
        
        html_report += f'''
                <tr>
                    <td><span class="benchmark-name">{name}</span></td>
                    <td class="time-value">{real_time:.2f} {time_unit}</td>
                    <td class="cpu-time">{cpu_time:.2f} {time_unit}</td>
                    <td class="iterations">{iterations}</td>
                    <td class="time-value">{items_per_sec:.0f}</td>
                </tr>
'''
    
    html_report += '''
            </tbody>
        </table>
        
        <h2 class="section-title">Stress Test Benchmarks</h2>
        <div class="stress-tests">
            <strong>Note:</strong> The following benchmarks test high-frequency decoding with large packets:
            <ul>
                <li>High-Frequency Decode: Single large packet parsing</li>
                <li>Stress Tests (100/1000): Multiple large packets in sequence</li>
                <li>Variable-Size Tests: Parameterized stress testing (10, 50, 100, 500 packets)</li>
            </ul>
        </div>
        
        <footer>
            <p>sip2json Benchmark Suite | C++20 Header-Only SIP Parser</p>
            <p>For more information, visit: <a href="https://github.com/siddiqsoftware/sip2json">github.com/siddiqsoftware/sip2json</a></p>
        </footer>
    </div>
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
    print(f"✓ JUnit XML generated: {junit_path}")
    
    # Generate HTML report
    html_report = generate_html_report(data)
    html_path = Path(output_dir) / "benchmark_report.html"
    with open(html_path, 'w') as f:
        f.write(html_report)
    print(f"✓ HTML Report generated: {html_path}")


if __name__ == "__main__":
    main()
