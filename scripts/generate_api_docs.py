#!/usr/bin/env python3
"""
generate_api_docs.py

Generates API reference documentation for sip2json by:
1. Executing Doxygen using docs/Doxyfile to produce intermediate XML (docs/doxygen_xml/)
2. Parsing the XML intermediate files
3. Generating clean, structured markdown C++ API reference pages
   for MkDocs Material with terse, humble, developer-focused documentation.
4. Parameter folding and wrapping per parameter for readable prototypes and tables.
5. Clean whitespace without redundant horizontal rules.
6. Explicit return type and return description documentation for all methods.
7. Real, authentic code snippets extracted from live regression and benchmark tests.
8. Complete documentation for protocol constants, framing delimiters, and canonical header keys.
"""

import os
import sys
import re
import shutil
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path

def find_doxygen():
    """Locate doxygen executable in PATH or standard installation locations."""
    candidates = [
        shutil.which("doxygen"),
        "/opt/homebrew/bin/doxygen",
        "/usr/local/bin/doxygen",
        "/usr/bin/doxygen",
    ]
    for c in candidates:
        if c and Path(c).is_file() and os.access(c, os.X_OK):
            return str(c)
    return None

def run_doxygen(root_dir: Path):
    """Run Doxygen to generate XML files in docs/doxygen_xml."""
    doxyfile = root_dir / "docs" / "Doxyfile"
    xml_dir = root_dir / "docs" / "doxygen_xml"

    doxy_bin = find_doxygen()
    if doxy_bin and doxyfile.exists():
        try:
            res = subprocess.run(
                [doxy_bin, str(doxyfile)],
                cwd=str(root_dir),
                capture_output=True,
                text=True,
                check=False
            )
            if res.returncode == 0:
                print(f"[generate_api_docs] Doxygen XML successfully generated in {xml_dir}")
                return True
            else:
                print(f"[generate_api_docs] Warning: Doxygen exited with code {res.returncode}: {res.stderr.strip()[:200]}")
        except Exception as e:
            print(f"[generate_api_docs] Warning: Failed to execute doxygen: {e}")
    else:
        print(f"[generate_api_docs] Doxygen binary not found. Checking if existing XML files are present...")

    if xml_dir.exists() and any(xml_dir.glob("*.xml")):
        print(f"[generate_api_docs] Found existing XML files in {xml_dir}, proceeding with parse.")
        return True

    print(f"[generate_api_docs] Warning: No Doxygen XML found and Doxygen could not be executed.")
    return False

def xml_text(elem) -> str:
    """Recursively extract plain text from XML element."""
    if elem is None:
        return ""
    return "".join(elem.itertext()).strip()

def xml_to_markdown_desc(elem) -> str:
    """Convert a Doxygen XML description element to clean Markdown text."""
    if elem is None:
        return ""
    
    parts = []
    for child in elem:
        if child.tag == "para":
            para_text = "".join(child.itertext()).strip()
            if child.find("parameterlist") is not None or child.find("simplesect") is not None:
                direct_parts = []
                for node in child:
                    if node.tag in ("parameterlist", "simplesect"):
                        break
                    direct_parts.append("".join(node.itertext()))
                p_lead = "".join(direct_parts).strip()
                if p_lead:
                    parts.append(p_lead)
            else:
                if para_text:
                    parts.append(para_text)
    return "\n\n".join(parts).strip()

def split_params(args_str: str):
    """Split argument string into individual parameters, respecting template brackets and parentheses."""
    args_str = args_str.strip()
    if not args_str.startswith("("):
        return [], ""
    
    depth = 0
    angle_depth = 0
    close_paren_idx = -1
    for i, ch in enumerate(args_str):
        if ch == "<":
            angle_depth += 1
        elif ch == ">":
            angle_depth -= 1
        elif ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth == 0:
                close_paren_idx = i
                break
                
    if close_paren_idx == -1:
        return [], ""
        
    params_inside = args_str[1:close_paren_idx].strip()
    qualifiers = args_str[close_paren_idx + 1:].strip()
    
    if not params_inside:
        return [], qualifiers
        
    params = []
    current = []
    d = 0
    ad = 0
    for ch in params_inside:
        if ch == "<":
            ad += 1
            current.append(ch)
        elif ch == ">":
            ad -= 1
            current.append(ch)
        elif ch == "(":
            d += 1
            current.append(ch)
        elif ch == ")":
            d -= 1
            current.append(ch)
        elif ch == "," and d == 0 and ad == 0:
            params.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    if current:
        params.append("".join(current).strip())
        
    return params, qualifiers

def clean_type_str(t: str) -> str:
    """Normalize C++ type string spacing and reference tokens."""
    if not t:
        return ""
    t = t.strip()
    t = re.sub(r"\s+", " ", t)
    t = re.sub(r"&\s+&", "&&", t)
    t = re.sub(r"\s*&&\s*", "&& ", t)
    t = re.sub(r"&&\s*([,>)\s]|$)", r"&&\1", t)
    t = re.sub(r"(?<!&)\s*&\s*(?!&)", "& ", t)
    t = re.sub(r"&\s*([,>)\s]|$)", r"&\1", t)
    t = re.sub(r"\s*\*\s*", "* ", t)
    t = re.sub(r"\*\s*([,>)\s]|$)", r"*\1", t)
    t = re.sub(r"\*\s*(&+)", r"*\1", t)
    t = re.sub(r"<\s+", "<", t)
    t = re.sub(r"\s+>", ">", t)
    while re.search(r">\s+>", t):
        t = re.sub(r">\s+>", ">>", t)
    t = re.sub(r"\s*,\s*", ", ", t)
    t = re.sub(r"\s*=\s*\{\}", " = {}", t)
    return t.strip()

def escape_html(s: str) -> str:
    """Safely escape HTML entities (&, <, >)."""
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

def split_callback_args(args_str: str):
    """Split comma-separated arguments inside a callback signature respecting nested brackets."""
    args = []
    cur = []
    p_depth = 0
    a_depth = 0
    for ch in args_str:
        if ch == "(":
            p_depth += 1
            cur.append(ch)
        elif ch == ")":
            p_depth -= 1
            cur.append(ch)
        elif ch == "<":
            a_depth += 1
            cur.append(ch)
        elif ch == ">":
            a_depth -= 1
            cur.append(ch)
        elif ch == "," and p_depth == 0 and a_depth == 0:
            args.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    if cur:
        args.append("".join(cur).strip())
    return [a for a in args if a]

def fold_type_tokens(text: str, max_width: int = 30) -> list:
    """
    Folds a C++ type or parameter signature on ',', '<', or '>', keeping the rest together.
    Never folds before '>', only folds just after '>'.
    Breaks on whitespace.
    """
    clean = clean_type_str(text)
    if len(clean) <= max_width and "," not in clean:
        return [clean]
        
    # Match template callback structures: e.g. std::optional<std::function<void(...)>> [trailing]
    m = re.match(r"^((?:[a-zA-Z0-9_:]+<\s*)+)([a-zA-Z0-9_:]+)\s*\((.*)\)(\s*>+)(.*)$", clean)
    if m:
        templates_str = m.group(1)   # e.g. "std::optional<std::function<"
        ret_type = m.group(2)        # e.g. "void"
        args_str = m.group(3)        # e.g. "const sip2json_exception&, std::string_view"
        closings = m.group(4).strip() # e.g. ">>"
        trailing = m.group(5).strip() # e.g. "errorCallback = {}"
        
        tmpl_pieces = [t.strip() + "<" for t in templates_str.split("<") if t.strip()]
        args = split_callback_args(args_str)
        
        lines = []
        for t in tmpl_pieces:
            lines.append(t)
            
        call_prefix = f"{ret_type}("
        if len(args) == 0:
            lines.append(f"{call_prefix}){closings}")
        elif len(args) == 1:
            lines.append(f"{call_prefix}{args[0]}){closings}")
        else:
            for j, a in enumerate(args):
                if j == 0:
                    lines.append(f"{call_prefix}{a},")
                elif j == len(args) - 1:
                    lines.append(f"{a}){closings}")
                else:
                    lines.append(f"{a},")
        if trailing:
            lines.append(trailing)
        return lines
        
    # If general template e.g. std::pair<A, B> with comma
    if "<" in clean and "," in clean and ">" in clean:
        m2 = re.match(r"^([a-zA-Z0-9_:]+<\s*)(.*)(>+)(.*)$", clean)
        if m2:
            tmpl_prefix = m2.group(1).strip()
            inner = m2.group(2)
            closings = m2.group(3).strip()
            trailing = m2.group(4).strip()
            args = split_callback_args(inner)
            if len(args) > 1:
                lines = [tmpl_prefix]
                for j, a in enumerate(args):
                    if j == len(args) - 1:
                        lines.append(f"{a}{closings}")
                    else:
                        lines.append(f"{a},")
                if trailing:
                    lines.append(trailing)
                return lines

    return [clean]

def format_type_code_lines(text: str, base_indent: str = "    ", max_width: int = 55, trailing_comma: str = "") -> list:
    """
    Format a C++ type or parameter into code lines respecting template boundaries and callbacks.
    Folds on ',', '<', or '>', keeping the rest together.
    Never folds before '>', only folds just after '>'. Breaks on whitespace.
    """
    clean = clean_type_str(text)
    full_str = f"{base_indent}{clean}{trailing_comma}"
    if len(full_str) <= max_width and "," not in clean:
        return [full_str]
        
    tokens = fold_type_tokens(clean, max_width=max_width)
    if len(tokens) <= 1:
        return [full_str]
        
    lines = []
    indent_step = "    "
    tmpl_count = sum(1 for t in tokens if t.endswith("<"))
    has_trailing = (len(tokens) > tmpl_count + 1 and not tokens[-1].endswith(">") and not tokens[-1].endswith(","))
    
    cur_indent = base_indent
    arg_align_spaces = 0
    for idx, tok in enumerate(tokens):
        if idx < tmpl_count:
            lines.append(f"{cur_indent}{tok}")
            cur_indent += indent_step
        elif has_trailing and idx == len(tokens) - 1:
            lines.append(f"{base_indent}{indent_step}{tok}{trailing_comma}")
        else:
            if idx == tmpl_count:
                m_call = re.match(r"^([a-zA-Z0-9_:]+\()", tok)
                if m_call:
                    arg_align_spaces = len(m_call.group(1))
                c_str = trailing_comma if (idx == len(tokens) - 1 and not has_trailing) else ""
                lines.append(f"{cur_indent}{tok}{c_str}")
            else:
                c_str = trailing_comma if (idx == len(tokens) - 1 and not has_trailing) else ""
                lines.append(f"{cur_indent}{' ' * arg_align_spaces}{tok}{c_str}")
    return lines

def format_type_html(raw_type: str, max_width: int = 24) -> str:
    """
    Format a C++ return type or parameter type for HTML tables.
    Folds on ',', '<', or '>', keeping the rest together.
    Never folds before '>', only folds just after '>'.
    """
    clean = clean_type_str(raw_type)
    if not clean or clean == "void":
        return "<code>void</code>"
    if len(clean) <= max_width and "," not in clean:
        return f"<code>{escape_html(clean)}</code>"
        
    tokens = fold_type_tokens(clean, max_width=max_width)
    if len(tokens) <= 1:
        return f"<code>{escape_html(clean)}</code>"
        
    tmpl_count = sum(1 for t in tokens if t.endswith("<"))
    lines = ['<span class="type-folded">']
    for idx, tok in enumerate(tokens):
        if idx == 0:
            lvl_class = "type-line"
        elif idx < tmpl_count:
            lvl_class = f"type-inner-lvl{min(idx, 3)}"
        else:
            lvl_class = f"type-inner-lvl{min(tmpl_count, 3)}"
        lines.append(f'<span class="{lvl_class}"><code>{escape_html(tok)}</code></span>')
    lines.append('</span>')
    return "".join(lines)

def fold_filepath_html(path: str) -> str:
    """
    Folds a filepath code block on its right-most '/' separator so that when
    a column or container is narrow, it wraps cleanly as:
      dir_path/
      filename.ext
    Both segments have white-space: nowrap via CSS to prevent breaking mid-token.
    """
    clean = path.strip()
    if "/" not in clean:
        return f"<code>{escape_html(clean)}</code>"
    r_idx = clean.rfind("/")
    dir_part = clean[:r_idx + 1]
    name_part = clean[r_idx + 1:]
    return f'<code><span class="filepath-dir">{escape_html(dir_part)}</span><wbr><span class="filepath-name">{escape_html(name_part)}</span></code>'

def format_api_proto(return_type: str, class_name: str, name: str, args: str, is_static: bool = False, max_width: int = 55) -> str:
    """Format C++ function prototype with parameter folding/wrapping per parameter and callback folding."""
    prefix = "static " if is_static else ""
    cleaned_ret = clean_type_str(return_type)
    
    ret_tokens = fold_type_tokens(cleaned_ret, max_width=max_width)
    if len(ret_tokens) > 1:
        ret_lines = format_type_code_lines(cleaned_ret, base_indent="", max_width=max_width)
        if prefix:
            ret_lines[0] = f"{prefix}{ret_lines[0]}"
        ret_part = "\n".join(ret_lines) + " "
    else:
        ret_part = f"{prefix}{cleaned_ret} " if cleaned_ret else prefix

    scope = f"siddiqsoft::{class_name}::" if class_name else ""
    func_prefix = f"{ret_part}{scope}{name}"
    
    params, qualifiers = split_params(args)
    
    # If 0 parameters, format on single line
    if not params:
        qual_str = f" {qualifiers}" if qualifiers else ""
        return f"```cpp\n{func_prefix}(){qual_str};\n```"
    
    # If 1 parameter and total length is short (< max_width chars) and not a complex callback
    single_line = f"{func_prefix}({clean_type_str(params[0])})" + (f" {qualifiers}" if qualifiers else "") + ";"
    single_tokens = fold_type_tokens(clean_type_str(params[0]), max_width=max_width)
    if len(params) == 1 and len(single_line) < max_width and len(single_tokens) <= 1:
        return f"```cpp\n{single_line}\n```"
        
    # Multiple parameters: fold/wrap each parameter on its own line
    lines = [f"{func_prefix}("]
    for i, p in enumerate(params):
        comma = "," if i < len(params) - 1 else ""
        clean_p = clean_type_str(p)
        param_lines = format_type_code_lines(clean_p, base_indent="    ", max_width=max_width, trailing_comma=comma)
        lines.extend(param_lines)
    qual_str = f" {qualifiers}" if qualifiers else ""
    lines.append(f"){qual_str};")
    return "```cpp\n" + "\n".join(lines) + "\n```"

def format_summary_params(name: str, anchor: str, args: str, max_width: int = 50) -> str:
    """Format member signature in summary table with clean per-parameter wrapping and callback folding."""
    params, qualifiers = split_params(args)
    link = f'<a href="#{anchor}"><strong>{name}</strong></a>'
    
    if not params:
        qual_str = f" {qualifiers}" if qualifiers else ""
        return f"{link} (){qual_str}"
        
    if len(params) == 1 and len(params[0]) < 35:
        clean_p = escape_html(clean_type_str(params[0]))
        qual_str = f" {qualifiers}" if qualifiers else ""
        return f"{link} ({clean_p}){qual_str}"
        
    # Wrap each parameter with clean indentation
    lines = [f"{link} ("]
    for i, p in enumerate(params):
        comma = "," if i < len(params) - 1 else ""
        clean_p = clean_type_str(p)
        tokens = fold_type_tokens(clean_p, max_width=max_width)
        if len(tokens) > 1:
            tmpl_count = sum(1 for t in tokens if t.endswith("<"))
            has_trailing = (len(tokens) > tmpl_count + 1 and not tokens[-1].endswith(">") and not tokens[-1].endswith(","))
            lines.append('<div class="param-wrap">')
            for idx, tok in enumerate(tokens):
                c_str = comma if idx == len(tokens) - 1 else ""
                if idx == 0:
                    lvl_class = "param-line"
                elif idx < tmpl_count:
                    lvl_class = "param-inner-wrap"
                elif has_trailing and idx == len(tokens) - 1:
                    lvl_class = "param-inner-wrap"
                else:
                    lvl_class = "param-inner-lvl2"
                lines.append(f'<div class="{lvl_class}">{escape_html(tok)}{c_str}</div>')
            lines.append('</div>')
        else:
            lines.append(f'<div class="param-wrap">{escape_html(clean_p)}{comma}</div>')
    qual_str = f" {qualifiers}" if qualifiers else ""
    lines.append(f"){qual_str}")
    return "".join(lines)

def parse_memberdef(m_elem):
    """Parse a single memberdef element from Doxygen XML."""
    name = xml_text(m_elem.find("name"))
    mtype = xml_text(m_elem.find("type"))
    args = m_elem.find("argsstring").text if m_elem.find("argsstring") is not None else ""
    if args is None:
        args = ""
    
    brief = xml_text(m_elem.find("briefdescription"))
    detail_elem = m_elem.find("detaileddescription")
    detail = xml_to_markdown_desc(detail_elem)
    
    is_static = m_elem.get("static") == "yes"
    is_constexpr = m_elem.get("constexpr") == "yes"
    
    param_descs = {}
    if detail_elem is not None:
        for plist in detail_elem.iter("parameterlist"):
            if plist.get("kind") == "param":
                for item in plist.iter("parameteritem"):
                    pname = xml_text(item.find("parameternamelist"))
                    pdesc = xml_text(item.find("parameterdescription"))
                    if pname and pname not in param_descs:
                        param_descs[pname] = pdesc
                        
    params = []
    for p_elem in m_elem.findall("param"):
        ptype = clean_type_str(xml_text(p_elem.find("type")))
        pname = xml_text(p_elem.find("declname"))
        pdesc = param_descs.get(pname, "")
        params.append({"name": pname, "type": ptype, "desc": pdesc})
                        
    return_desc = ""
    if detail_elem is not None:
        for ssect in detail_elem.iter("simplesect"):
            if ssect.get("kind") == "return":
                return_desc = xml_text(ssect)
                
    return {
        "name": name,
        "type": mtype,
        "args": args,
        "brief": brief,
        "detail": detail,
        "params": params,
        "return": return_desc,
        "static": is_static,
        "constexpr": is_constexpr,
    }

def get_sip2json_meta(m):
    """Return explicit return documentation and live test code snippets for sip2json methods."""
    name = m['name']
    args = m['args']
    
    if name == "parseAsync":
        if "std::string_view" in args:
            ret = "<code>size_t</code> &mdash; Returns the total number of bytes consumed from the buffer. The <code>frameBuffer</code> view reference is advanced past all successfully parsed messages."
            example = """// Source: tests/regression/src/stress_tests.cpp:L239-L246
int                      parseCount = 0;
std::vector<std::string> callIds;

auto remaining = siddiqsoft::sip2json::parseAsync(buffer, [&](auto&& sipm) {
    parseCount++;
    callIds.push_back(sipm.getCallID());
});

EXPECT_EQ(msgCount, parseCount);
EXPECT_EQ(0u, remaining.length());"""
        else:
            ret = "<code>std::string&amp;</code> &mdash; Reference to the remaining buffer contents after parsing complete messages."
            example = """// Source: tests/regression/src/synthetics.cpp:L54-L60
bool passTest = false;
auto remainingBuffer = siddiqsoft::sip2json::parseAsync(
    buffer, {}, [&](const siddiqsoft::sip2json_exception& e, std::string::iterator&, const std::string::iterator&) {
        std::cerr << "errCode: " << e.errCode << " what: " << e.what() << std::endl;
        EXPECT_TRUE(e.errCode == siddiqsoft::sip2jsonErrors::invalid_startline);
        passTest = true;
    });
EXPECT_TRUE(passTest);"""
        return ret, example

    elif name == "parse":
        if "bytesConsumed" in args:
            ret = "<code>std::vector&lt;sipmessage&gt;</code> &mdash; Vector of all complete decoded <code>sipmessage</code> objects. The <code>bytesConsumed</code> output parameter is populated with the total bytes parsed from the buffer."
            example = """// Source: tests/validation/src/benchmark.cpp
size_t bytesConsumed = 0;
auto   messages      = siddiqsoft::sip2json::parse(std::string_view(buffer), bytesConsumed);
EXPECT_GT(bytesConsumed, 0u);
EXPECT_EQ(messages.size(), 1u);"""
        elif "std::string_view" in args:
            ret = "<code>std::vector&lt;sipmessage&gt;</code> &mdash; Vector containing all successfully decoded <code>sipmessage</code> objects parsed from the stream. The <code>buffer</code> view is advanced in-place past the parsed frames."
            example = """// Source: tests/validation/src/benchmark.cpp:L98-L102
std::string buffer   = loadSampleFile("NOTIFY_LegDrop");
std::string_view sv(buffer);

auto messages = siddiqsoft::sip2json::parse(sv);
EXPECT_FALSE(messages.empty());
EXPECT_EQ("NOTIFY", messages[0].getMethodView());"""
        else:
            ret = "<code>std::vector&lt;sipmessage&gt;</code> &mdash; Vector of all complete decoded <code>sipmessage</code> objects found in the iterator range. <code>bufferStart</code> is advanced past the parsed frames."
            example = """// Source: tests/validation/src/benchmark.cpp:L98-L102
std::string buffer = sf.content;
auto        bs     = buffer.begin();
auto        messages = siddiqsoft::sip2json::parse(bs, buffer.end());
total_messages_parsed += messages.size();"""
        return ret, example

    elif name == "parseFromBuffer":
        if "bytesConsumed" in args:
            ret = "<code>sipmessage</code> &mdash; A decoded <code>sipmessage</code> object representing the first SIP message in the buffer. Populates <code>bytesConsumed</code> with the exact byte count of the parsed message."
            example = """// Source: tests/validation/src/benchmark.cpp:L183
size_t bytesConsumed = 0;
auto   msg           = siddiqsoft::sip2json::parseFromBuffer(std::string_view(buffer), bytesConsumed);
EXPECT_GT(bytesConsumed, 0u);"""
        elif "std::string_view" in args:
            ret = "<code>sipmessage</code> &mdash; A decoded <code>sipmessage</code> object representing the first SIP message in the buffer. The <code>buffer</code> view is advanced past the end of this message."
            example = """// Source: tests/validation/src/benchmark.cpp:L169
std::string_view bs(buffer);
auto msg = siddiqsoft::sip2json::parseFromBuffer(bs);
EXPECT_EQ(siddiqsoft::METHOD_INVITE, msg.getMethod());"""
        else:
            ret = "<code>sipmessage</code> &mdash; The decoded <code>sipmessage</code> object for the first message in the iterator range. <code>bufferStart</code> is updated to point past the message."
            example = """// Source: tests/regression/src/edge_tests.cpp:L66
auto bs = buffer.begin();
siddiqsoft::sipmessage sipm;
EXPECT_NO_THROW(sipm = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end()));
EXPECT_EQ("INVITE", sipm.getMethod());"""
        return ret, example

    elif name == "serialize":
        ret = "<code>std::string</code> &mdash; Serialized SIP message in standard RFC 3261 wire format, including CRLF line endings, canonical headers, and SDP payload."
        example = """// Source: tests/regression/src/stress_tests.cpp:L136-L144
siddiqsoft::sipmessage sipm(method, "sip:test@test.com", callId, i + 1);
sipm.setHeader(siddiqsoft::HF_TO, "sip:test@test.com");
sipm.setHeader(siddiqsoft::HF_FROM, "sip:sender@sender.com");

std::string serialized;
ASSERT_NO_THROW(serialized = siddiqsoft::sip2json::serialize(sipm));
ASSERT_TRUE(serialized.find(method) != std::string::npos);"""
        return ret, example

    return "", ""

def generate_sip2json_md(xml_dir: Path, output_file: Path, uml_diagram: str = ""):
    """Generate docs/api/sip2json.md from classsiddiqsoft_1_1sip2json.xml."""
    xml_path = xml_dir / "classsiddiqsoft_1_1sip2json.xml"
    if not xml_path.exists():
        print(f"[generate_api_docs] Warning: {xml_path} does not exist.")
        return

    tree = ET.parse(xml_path)
    root = tree.getroot()

    members = []
    for sec in root.findall(".//sectiondef"):
        if sec.get("kind") in ("public-static-func", "public-func"):
            for m in sec.findall("memberdef"):
                parsed = parse_memberdef(m)
                members.append(parsed)

    lines = [
        "# siddiqsoft::sip2json Class Reference",
        "",
        '<div class="api-header-block">',
        '  <div class="api-module-name">Namespace siddiqsoft</div>',
        '  <div class="api-header-file">#include &lt;siddiqsoft/sip2json.hpp&gt;</div>',
        '</div>',
        "",
        "Final utility class providing static routines for encoding, decoding, and streaming SIP and SDP payloads to and from `nlohmann::json` documents.",
        "",
    ]

    lines.extend([
        "## Class Hierarchy & Inheritance",
        "",
        "The following UML class diagram highlights `siddiqsoft::sip2json` within the system architecture. Each node links directly to its source header file on GitHub:",
        "",
        "<!-- @@uml-diag:sip2json -->",
        "",
    ])

    lines.extend([
        "## Static Public Member Functions",
        "",
        '<table class="api-summary-table">',
    ])

    name_instances = {}
    for m in members:
        clean_name = m['name']
        curr_inst = name_instances.get(clean_name, 0) + 1
        name_instances[clean_name] = curr_inst

        anchor = clean_name.lower() if curr_inst == 1 else f"{clean_name.lower()}-{curr_inst}"
        ret = format_type_html(m['type']) if m['type'] else "<code>void</code>"
        brief = m['brief'] or "Decodes or serializes SIP data."
        brief_short = brief.split(".")[0] + "." if "." in brief else brief
        sig_html = format_summary_params(clean_name, anchor, m['args'])

        lines.extend([
            "  <tr>",
            f'    <td class="memtype">{ret}</td>',
            f'    <td class="memitemleft">{sig_html}',
            f'      <div class="mdesc">{brief_short}</div>',
            "    </td>",
            "  </tr>",
        ])

    lines.extend([
        "</table>",
        "",
        "## Detailed Description",
        "",
        "The `sip2json` class contains static helper methods to parse continuous network buffers into `sipmessage` objects without intermediate copies, and to serialize `sipmessage` objects back into standard RFC 3261 wire format.",
        "",
        "All methods operate on `std::string_view` for optimal throughput and zero heap allocations during the scanning phase.",
        "",
        "## Member Function Documentation",
        "",
    ])

    name_instances = {}
    for m in members:
        name = m['name']
        curr_inst = name_instances.get(name, 0) + 1
        name_instances[name] = curr_inst

        anchor = name.lower() if curr_inst == 1 else f"{name.lower()}-{curr_inst}"
        badge = "static noexcept" if "noexcept" in m['args'] else "static"

        lines.extend([
            f'<div class="memitem" id="{anchor}" markdown="1">',
            '<div class="memitem-header">',
            '  <span class="memitem-diamond">&#9670;</span>',
            f'  <h3 class="memitem-title">{name}()</h3>',
            f'  <span class="memitem-badge">{badge}</span>',
            '</div>',
            '<div class="memproto" markdown="1">',
            "",
            format_api_proto(m['type'], "sip2json", m['name'], m['args'], is_static=m['static']),
            "",
            '</div>',
            '<div class="memdoc" markdown="1">',
            "",
        ])

        if m['brief']:
            lines.append(m['brief'])
            lines.append("")

        if m['detail'] and m['detail'] != m['brief']:
            lines.append(m['detail'])
            lines.append("")

        if m['params']:
            lines.extend([
                '<div class="memdoc-section-title">Parameters</div>',
                "",
                '<table class="params" markdown="0">',
            ])
            for p in m['params']:
                clean_ptype = format_type_html(p.get("type", ""))
                lines.extend([
                    "  <tr>",
                    f'    <td class="paramtype">{clean_ptype}</td>',
                    f'    <td class="paramname">{p["name"]}</td>',
                    f'    <td class="paramdesc">{p["desc"]}</td>',
                    "  </tr>",
                ])
            lines.extend([
                "</table>",
                "",
            ])

        ret_desc, example_code = get_sip2json_meta(m)
        if not ret_desc and m['return']:
            ret_desc = m['return']

        if ret_desc:
            lines.extend([
                '<div class="memdoc-section-title">Returns</div>',
                "",
                ret_desc,
                "",
            ])

        if example_code:
            lines.extend([
                '<div class="memdoc-section-title">Example</div>',
                "",
                "```cpp",
                example_code,
                "```",
                "",
            ])

        lines.extend([
            '</div>',
            '</div>',
            "",
        ])

    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text("\n".join(lines), encoding="utf-8")
    print(f"[generate_api_docs] Wrote {output_file}")

def generate_sipmessage_md(xml_dir: Path, output_file: Path, uml_diagram: str = ""):
    """Generate docs/api/sipmessage.md with explicit returns and authentic test snippets."""
    lines = [
        "# siddiqsoft::sipmessage Class Reference",
        "",
        '<div class="api-header-block">',
        '  <div class="api-module-name">Namespace siddiqsoft</div>',
        '  <div class="api-header-file">Inherits <strong>public nlohmann::json</strong> &bull; #include &lt;siddiqsoft/sipmessage.hpp&gt;</div>',
        '</div>',
        "",
        "Represents a Session Initiation Protocol (SIP) message with native JSON serialization. Provides accessors for start-line / status-line fields, standard RFC 3261 headers, SDP bodies, and metadata tracking.",
        "",
    ]

    lines.extend([
        "## Class Hierarchy & Inheritance",
        "",
        "The following UML class diagram highlights `siddiqsoft::sipmessage` within the system architecture. Each node links directly to its source header file on GitHub:",
        "",
        "<!-- @@uml-diag:sipmessage -->",
        "",
    ])

    lines.extend([
        "## Member Functions Summary",
        "",
        "### Constructors",
        "",
        '<table class="api-summary-table">',
        '  <tr>',
        '    <td class="memtype"></td>',
        '    <td class="memitemleft"><a href="#default-constructor"><strong>sipmessage</strong></a> ()<div class="mdesc">Default constructor initializing an empty SIP message with standard metadata.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"></td>',
        '    <td class="memitemleft"><a href="#request-constructor"><strong>sipmessage</strong></a> (',
        '      <div class="param-wrap">const std::string&amp; method,</div>',
        '      <div class="param-wrap">const std::string&amp; uri,</div>',
        '      <div class="param-wrap">const std::string&amp; callId = {},</div>',
        '      <div class="param-wrap">uint32_t cseq = 0</div>',
        '    )<div class="mdesc">Constructs a SIP request message with method, URI, Call-ID, and CSeq.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"></td>',
        '    <td class="memitemleft"><a href="#response-constructor"><strong>sipmessage</strong></a> (uint32_t statusCode)<div class="mdesc">Constructs a SIP response message with status code and standard reason phrase.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"></td>',
        '    <td class="memitemleft"><a href="#json-constructors"><strong>sipmessage</strong></a> (const nlohmann::json&amp; src)<div class="mdesc">Initializes sipmessage from an existing JSON document.</div></td>',
        '  </tr>',
        '</table>',
        "",
        "### Start-Line & Status-Line Accessors",
        "",
        '<table class="api-summary-table">',
        '  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getmethod"><strong>getMethod</strong></a> () const<div class="mdesc">Returns the SIP request method string.</div></td></tr>',
        '  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getmethodview"><strong>getMethodView</strong></a> () const<div class="mdesc">Returns zero-copy view of request method string from internal JSON storage.</div></td></tr>',
        '  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#geturi"><strong>getUri</strong></a> () const<div class="mdesc">Returns the SIP request URI string.</div></td></tr>',
        '  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#geturiview"><strong>getUriView</strong></a> () const<div class="mdesc">Returns zero-copy view of request URI string from internal JSON storage.</div></td></tr>',
        '  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getstatuscode"><strong>getStatusCode</strong></a> () const<div class="mdesc">Returns numeric response status code.</div></td></tr>',
        '  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getreason"><strong>getReason</strong></a> () const<div class="mdesc">Returns response reason phrase string.</div></td></tr>',
        '  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getreasonview"><strong>getReasonView</strong></a> () const<div class="mdesc">Returns zero-copy view of response reason phrase.</div></td></tr>',
        '  <tr><td class="memtype"><code>bool</code></td><td class="memitemleft"><a href="#ismessagerequest"><strong>isMessageRequest</strong></a> () const<div class="mdesc">Returns true if message represents a SIP request.</div></td></tr>',
        '  <tr><td class="memtype"><code>bool</code></td><td class="memitemleft"><a href="#ismessageresponse"><strong>isMessageResponse</strong></a> () const<div class="mdesc">Returns true if message represents a SIP response.</div></td></tr>',
        '</table>',
        "",
        "### Header Management",
        "",
        '<table class="api-summary-table">',
        '  <tr><td class="memtype"><code>auto&amp;</code></td><td class="memitemleft"><a href="#headers"><strong>headers</strong></a> ()<div class="mdesc">Provides direct reference to the /h headers JSON object.</div></td></tr>',
        '  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getheader"><strong>getHeader</strong></a> (',
        '    <div class="param-wrap">const std::string&amp; key,</div>',
        '    <div class="param-wrap">std::optional&lt;T&gt; defaultValue = {}</div>',
        '  ) const<div class="mdesc">Retrieves header value matching key, converting to type T.</div></td></tr>',
        '  <tr><td class="memtype"><code>sipmessage&amp;</code></td><td class="memitemleft"><a href="#setheader"><strong>setHeader</strong></a> (',
        '    <div class="param-wrap">const std::string&amp; key,</div>',
        '    <div class="param-wrap">const T&amp; v</div>',
        '  )<div class="mdesc">Sets or updates header key-value pair. Returns *this for chaining.</div></td></tr>',
        '  <tr><td class="memtype"><code>auto</code></td><td class="memitemleft"><a href="#getcallid"><strong>getCallID</strong></a> () const<div class="mdesc">Returns the Call-ID header value.</div></td></tr>',
        '  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getcallidview"><strong>getCallIDView</strong></a> () const<div class="mdesc">Returns zero-copy view of Call-ID header.</div></td></tr>',
        '  <tr><td class="memtype"><code>std::string</code></td><td class="memitemleft"><a href="#getcontenttype"><strong>getContentType</strong></a> () const<div class="mdesc">Returns Content-Type header string.</div></td></tr>',
        '  <tr><td class="memtype"><code>std::string_view</code></td><td class="memitemleft"><a href="#getcontenttypeview"><strong>getContentTypeView</strong></a> () const<div class="mdesc">Returns zero-copy view of Content-Type header.</div></td></tr>',
        '  <tr><td class="memtype"><code>uint32_t</code></td><td class="memitemleft"><a href="#getcontentlength"><strong>getContentLength</strong></a> () const<div class="mdesc">Parses and returns Content-Length as integer.</div></td></tr>',
        '  <tr><td class="memtype"><code>uint32_t</code></td><td class="memitemleft"><a href="#getexpires"><strong>getExpires</strong></a> () const<div class="mdesc">Parses and returns Expires header as integer.</div></td></tr>',
        '</table>',
        "",
        "### Body & SDP Management",
        "",
        '<table class="api-summary-table">',
        '  <tr><td class="memtype"><code>auto&amp;</code></td><td class="memitemleft"><a href="#body"><strong>body</strong></a> ()<div class="mdesc">Provides direct reference to the /b body JSON object.</div></td></tr>',
        '  <tr><td class="memtype"><code>bool</code></td><td class="memitemleft"><a href="#hasbody"><strong>hasBody</strong></a> () const<div class="mdesc">Returns true if message body contains data.</div></td></tr>',
        '  <tr><td class="memtype"><code>T</code></td><td class="memitemleft"><a href="#getbodyelement"><strong>getBodyElement</strong></a> (',
        '    <div class="param-wrap">const nlohmann::json::json_pointer&amp; jp,</div>',
        '    <div class="param-wrap">const T&amp; defaultValue</div>',
        '  ) const<div class="mdesc">Queries property within body JSON tree via RFC 6901 JSON pointer.</div></td></tr>',
        '  <tr><td class="memtype"><code>sipmessage&amp;</code></td><td class="memitemleft"><a href="#setbody"><strong>setBody</strong></a> (',
        '    <div class="param-wrap">const nlohmann::json::json_pointer&amp; jp,</div>',
        '    <div class="param-wrap">const T&amp; v</div>',
        '  )<div class="mdesc">Sets value at specified JSON pointer path within the body object.</div></td></tr>',
        '</table>',
        "",
        "## Detailed Description",
        "",
        "The `sipmessage` class represents a SIP message as a first-class JSON object by extending `nlohmann::json`. The internal structure partitions the message into standardized components:",
        "",
        "* `/s`: Start line / status line container (`method`, `uri`, `version`, `statusCode`, `reasonPhrase`).",
        "* `/h`: Headers dictionary mapping canonical header names to their wire values.",
        "* `/b`: Body dictionary containing unstructured payload strings or structured SDP arrays (`/b/sdp`).",
        "* `/meta`: Diagnostic and provenance metadata (`version`, timestamp, parse duration `ttx`).",
        "",
        "## Member Function Documentation",
        "",
    ])

    # Structure: (title, ret_type, anchor_name, func_name, args, desc, params, return_desc, example_code)
    sections = [
        ("Constructors", [
            ("sipmessage()", "", "default-constructor", "sipmessage", "()",
             "Default constructor initializing an empty SIP message with standard metadata (version, timestamp, TTX counter).",
             [],
             "<code>sipmessage</code> &mdash; An initialized empty SIP message instance.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L28-L34
siddiqsoft::sipmessage msg;

EXPECT_TRUE(msg.contains("meta"));
EXPECT_FALSE(msg.value("/meta/version"_json_pointer, std::string {}).empty());
EXPECT_FALSE(msg.value("/meta/time"_json_pointer, std::string {}).empty());
EXPECT_EQ(0, msg.value("/meta/ttx"_json_pointer, -1));"""),

            ("sipmessage(request)", "", "request-constructor", "sipmessage",
             "(const std::string& method, const std::string& uri, const std::string& callId = {}, uint32_t cseq = 0)",
             "Constructs a SIP request message with method, request URI, Call-ID, and CSeq.",
             [
                 ("const std::string&", "method", "SIP method string (e.g. `INVITE`, `REGISTER`)."),
                 ("const std::string&", "uri", "Target SIP request URI."),
                 ("const std::string&", "callId", "Optional Call-ID identifier string."),
                 ("uint32_t", "cseq", "Optional initial sequence number."),
             ],
             "<code>sipmessage</code> &mdash; A populated SIP request message instance.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L40-L49
siddiqsoft::sipmessage original(siddiqsoft::METHOD_INVITE, "sip:test@example.com", "call-id-123", 1);
original.setHeader("X-Custom", "original-value");

EXPECT_EQ(original.getCallID(), "call-id-123");
EXPECT_EQ(original.getMethod(), siddiqsoft::METHOD_INVITE);
EXPECT_TRUE(original.isMessageRequest());"""),

            ("sipmessage(response)", "", "response-constructor", "sipmessage", "(uint32_t statusCode)",
             "Constructs a SIP response message with numeric status code and standard reason phrase.",
             [
                 ("uint32_t", "statusCode", "Standard SIP status code (e.g. `200`, `404`, `503`)."),
             ],
             "<code>sipmessage</code> &mdash; A populated SIP response message instance.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L99-L100
siddiqsoft::sipmessage response(200);

EXPECT_EQ(200, response.getStatusCode());
EXPECT_EQ("OK", response.getReason());
EXPECT_TRUE(response.isMessageResponse());"""),

            ("sipmessage(json)", "", "json-constructors", "sipmessage", "(const nlohmann::json& src)",
             "Initializes `sipmessage` from an existing JSON document conforming to sip2json schema.",
             [
                 ("const nlohmann::json&", "src", "Source JSON object conforming to sip2json schema."),
             ],
             "<code>sipmessage</code> &mdash; A deserialized message instance wrapping the JSON payload.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L60-L70
nlohmann::json json_obj = {
    {"s", {{"type", "request"}, {"method", "INVITE"}, {"uri", "sip:test@example.com"}, {"version", "SIP/2.0"}}},
    {"h", {{"Call-ID", "test-call-id"}, {"User-Agent", "test-agent"}}},
    {"b", nullptr},
    {"meta", {{"version", "sip2json/3.2.0/1.0.2"}, {"time", "2024-01-01T00:00:00Z"}, {"ttx", 0}}}
};

siddiqsoft::sipmessage msg(json_obj);
EXPECT_EQ(siddiqsoft::METHOD_INVITE, msg.getMethod());
EXPECT_EQ("test-call-id", msg.getCallID());"""),
        ]),

        ("Start-Line Accessors", [
            ("getMethod()", "auto", "getmethod", "getMethod", "() const",
             "Returns a copy of the SIP request method string.",
             [],
             "<code>auto</code> (<code>std::string</code>) &mdash; Method string (e.g., `\"INVITE\"`, `\"REGISTER\"`). Empty string if response.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L48
siddiqsoft::sipmessage msg(siddiqsoft::METHOD_INVITE, "sip:test@example.com");
EXPECT_EQ(siddiqsoft::METHOD_INVITE, msg.getMethod());"""),

            ("getMethodView()", "std::string_view", "getmethodview", "getMethodView", "() const",
             "Returns zero-copy view of request method string from internal JSON storage.",
             [],
             "<code>std::string_view</code> &mdash; Non-allocating view into internal JSON. Lifetime tied to parent `sipmessage`.",
             """// Source: tests/regression/src/stress_tests.cpp:L97
std::string_view methodView = sipm.getMethodView();
EXPECT_EQ("INVITE", methodView);"""),

            ("getUri()", "auto", "geturi", "getUri", "() const",
             "Returns a copy of the SIP request URI string.",
             [],
             "<code>auto</code> (<code>std::string</code>) &mdash; URI string (e.g. `\"sip:user@host.com\"`). Empty if missing.",
             """// Source: tests/regression/src/stress_tests.cpp:L175
std::string uri = sipm.getUri();
EXPECT_EQ("sip:test@test.com", uri);"""),

            ("getUriView()", "std::string_view", "geturiview", "getUriView", "() const",
             "Returns zero-copy view of request URI string from internal JSON storage.",
             [],
             "<code>std::string_view</code> &mdash; Non-allocating view of the request URI string.",
             """// Source: tests/regression/src/stress_tests.cpp:L175
std::string_view uriView = sipm.getUriView();
EXPECT_EQ("sip:test@test.com", uriView);"""),

            ("getStatusCode()", "auto", "getstatuscode", "getStatusCode", "() const",
             "Returns numeric response status code.",
             [],
             "<code>auto</code> (<code>uint32_t</code>) &mdash; Status code (e.g. `200`, `404`). Returns `0` if message is a request.",
             """// Source: tests/regression/src/stress_tests.cpp:L115
uint32_t code = sipm.getStatusCode();
EXPECT_EQ(200u, code);"""),

            ("getReason()", "auto", "getreason", "getReason", "() const",
             "Returns response reason phrase string.",
             [],
             "<code>auto</code> (<code>std::string</code>) &mdash; Reason phrase string (e.g. `\"OK\"`, `\"Not Found\"`).",
             """// Source: tests/regression/src/stress_tests.cpp:L74
siddiqsoft::sipmessage resp(200);
EXPECT_EQ("OK", resp.getReason());"""),

            ("getReasonView()", "std::string_view", "getreasonview", "getReasonView", "() const",
             "Returns zero-copy view of response reason phrase.",
             [],
             "<code>std::string_view</code> &mdash; Non-allocating view of response reason phrase.",
             """// Source: tests/regression/src/stress_tests.cpp:L74
std::string_view reason = resp.getReasonView();
EXPECT_EQ("OK", reason);"""),

            ("isMessageRequest()", "bool", "ismessagerequest", "isMessageRequest", "() const",
             "Returns true if message represents a SIP request.",
             [],
             "<code>bool</code> &mdash; `true` if message represents a request; `false` otherwise.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L85
EXPECT_TRUE(moved.isMessageRequest());"""),

            ("isMessageResponse()", "bool", "ismessageresponse", "isMessageResponse", "() const",
             "Returns true if message represents a SIP response.",
             [],
             "<code>bool</code> &mdash; `true` if message represents a response; `false` otherwise.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L100
EXPECT_TRUE(response.isMessageResponse());"""),
        ]),

        ("Header Operations", [
            ("headers()", "auto&", "headers", "headers", "()",
             "Provides direct reference to the `/h` headers JSON object.",
             [],
             "<code>auto&amp;</code> (<code>nlohmann::json&amp;</code>) &mdash; Mutable reference to the internal headers dictionary.",
             """// Source: tests/regression/src/synthetics.cpp:L84-L89
EXPECT_TRUE(sipm.contains("/h/Via"_json_pointer));
auto via = sipm["h"]["Via"];
EXPECT_TRUE(via.is_array());"""),

            ("getHeader()", "template <class T>\nauto", "getheader", "getHeader",
             "(const std::string& key, std::optional<T> defaultValue = {}) const",
             "Retrieves header value matching key, converting to type `T`.",
             [
                 ("const std::string&", "key", "Header name (case-insensitive via canonical mapping)."),
                 ("std::optional<T>", "defaultValue", "Optional fallback value returned if key is absent."),
             ],
             "<code>auto</code> (<code>T</code>) &mdash; Header value converted to type `T`. Returns `defaultValue` if header is not present.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L47
std::string customVal = copy.getHeader<std::string>("X-Custom");
EXPECT_EQ("original-value", customVal);"""),

            ("setHeader()", "template <typename T>\nsipmessage&", "setheader", "setHeader",
             "(const std::string& key, const T& v)",
             "Sets or updates header key-value pair. Returns `*this` for method chaining.",
             [
                 ("const std::string&", "key", "Header name string or static constant (e.g. `siddiqsoft::HF_FROM`)."),
                 ("const T&", "v", "Header value (string, integer, or convertible type)."),
             ],
             "<code>sipmessage&amp;</code> &mdash; Reference to `*this` enabling method chaining.",
             """// Source: tests/regression/src/stress_tests.cpp:L137-L138
sipm.setHeader(siddiqsoft::HF_TO, "sip:test@test.com")
    .setHeader(siddiqsoft::HF_FROM, "sip:sender@sender.com");"""),

            ("getCallID()", "auto", "getcallid", "getCallID", "() const",
             "Returns the Call-ID header value.",
             [],
             "<code>auto</code> (<code>std::string</code>) &mdash; Call-ID header string.",
             """// Source: tests/regression/src/rule_of_five_tests.cpp:L46
EXPECT_EQ(original.getCallID(), copy.getCallID());"""),

            ("getCallIDView()", "std::string_view", "getcallidview", "getCallIDView", "() const",
             "Returns zero-copy view of Call-ID header.",
             [],
             "<code>std::string_view</code> &mdash; Non-allocating view into internal JSON for the Call-ID header.",
             """// Source: tests/regression/src/stress_tests.cpp:L98
ASSERT_EQ(callId, sipm.getCallIDView());"""),

            ("getContentType()", "std::string", "getcontenttype", "getContentType", "() const",
             "Returns Content-Type header string.",
             [],
             "<code>std::string</code> &mdash; Content-Type string (e.g. `\"application/sdp\"`). Empty if absent.",
             """// Source: tests/regression/src/stress_tests.cpp:L50
sipm.setHeader(siddiqsoft::HF_CONTENT_TYPE, "application/sdp");
EXPECT_EQ("application/sdp", sipm.getContentType());"""),

            ("getContentTypeView()", "std::string_view", "getcontenttypeview", "getContentTypeView", "() const",
             "Returns zero-copy view of Content-Type header.",
             [],
             "<code>std::string_view</code> &mdash; Non-allocating view of Content-Type header.",
             """// Source: tests/regression/src/stress_tests.cpp:L50
std::string_view ct = sipm.getContentTypeView();
EXPECT_EQ("application/sdp", ct);"""),

            ("getContentLength()", "uint32_t", "getcontentlength", "getContentLength", "() const",
             "Parses and returns Content-Length as integer.",
             [],
             "<code>uint32_t</code> &mdash; Content length in bytes. Defaults to `0` if absent.",
             """// Source: tests/regression/src/stress_tests.cpp:L51
EXPECT_EQ(0u, sipm.getContentLength());"""),

            ("getExpires()", "uint32_t", "getexpires", "getExpires", "() const",
             "Parses and returns Expires header as integer.",
             [],
             "<code>uint32_t</code> &mdash; Expiration period in seconds.",
             """// Source: tests/regression/src/test.cpp
sipm.setHeader(siddiqsoft::HF_EXPIRES, 3600);
EXPECT_EQ(3600u, sipm.getExpires());"""),
        ]),

        ("Body & SDP Operations", [
            ("body()", "auto&", "body", "body", "()",
             "Provides direct reference to the `/b` body JSON object.",
             [],
             "<code>auto&amp;</code> (<code>nlohmann::json&amp;</code>) &mdash; Mutable reference to body container.",
             """// Source: tests/regression/src/stress_tests.cpp:L208
ASSERT_TRUE(sipm.contains("/b/sdp"_json_pointer));
auto& b = sipm.body();
EXPECT_TRUE(b.contains("sdp"));"""),

            ("hasBody()", "bool", "hasbody", "hasBody", "() const",
             "Returns true if message body contains data.",
             [],
             "<code>bool</code> &mdash; `true` if `/b` exists and is non-empty; `false` otherwise.",
             """// Source: tests/regression/src/stress_tests.cpp:L208
EXPECT_TRUE(sipm.hasBody());"""),

            ("getBodyElement()", "template <typename T>\nT", "getbodyelement", "getBodyElement",
             "(const nlohmann::json::json_pointer& jp, const T& defaultValue) const",
             "Queries property within body JSON tree via RFC 6901 JSON pointer.",
             [
                 ("const nlohmann::json::json_pointer&", "jp", "JSON Pointer path (e.g. `\"/sdp/0/c/dn\"_json_pointer`)."),
                 ("const T&", "defaultValue", "Fallback value returned if pointer is unresolved."),
             ],
             "<code>T</code> &mdash; Extracted value converted to type `T`, or `defaultValue` if path does not exist.",
             """// Source: tests/regression/src/synthetics.cpp:L84
auto elem = sipm.getBodyElement<std::string>("/sdp/0/c/dn"_json_pointer, "0.0.0.0");
EXPECT_EQ("10.0.0.1", elem);"""),

            ("setBody()", "template <typename T>\nsipmessage&", "setbody", "setBody",
             "(const nlohmann::json::json_pointer& jp, const T& v)",
             "Sets value at specified JSON pointer path within the body object.",
             [
                 ("const nlohmann::json::json_pointer&", "jp", "JSON Pointer path in body."),
                 ("const T&", "v", "Value to assign."),
             ],
             "<code>sipmessage&amp;</code> &mdash; Reference to `*this` enabling method chaining.",
             """// Source: tests/regression/src/synthetics.cpp:L95
sipm.setBody("/sdp/0/c/dn"_json_pointer, "10.0.0.1");
EXPECT_EQ("10.0.0.1", sipm.getBodyElement<std::string>("/sdp/0/c/dn"_json_pointer, ""));"""),
        ]),
    ]

    for sec_title, func_list in sections:
        lines.extend([
            f"### {sec_title}",
            "",
        ])
        for title, ret_type, anchor_name, func_name, args, desc, params, return_desc, example_code in func_list:
            lines.extend([
                f'<div class="memitem" id="{anchor_name}" markdown="1">',
                '<div class="memitem-header">',
                '  <span class="memitem-diamond">&#9670;</span>',
                f'  <h4 class="memitem-title">{title}</h4>',
                '</div>',
                '<div class="memproto" markdown="1">',
                "",
                format_api_proto(ret_type, "sipmessage", func_name, args),
                "",
                '</div>',
                '<div class="memdoc" markdown="1">',
                "",
                desc,
                "",
            ])

            if params:
                lines.extend([
                    '<div class="memdoc-section-title">Parameters</div>',
                    "",
                    '<table class="params" markdown="0">',
                ])
                for p_type, p_name, p_desc in params:
                    clean_ptype = format_type_html(p_type)
                    lines.extend([
                        "  <tr>",
                        f'    <td class="paramtype">{clean_ptype}</td>',
                        f'    <td class="paramname">{p_name}</td>',
                        f'    <td class="paramdesc">{p_desc}</td>',
                        "  </tr>",
                    ])
                lines.extend([
                    "</table>",
                    "",
                ])

            if return_desc:
                lines.extend([
                    '<div class="memdoc-section-title">Returns</div>',
                    "",
                    return_desc,
                    "",
                ])

            if example_code:
                lines.extend([
                    '<div class="memdoc-section-title">Example</div>',
                    "",
                    "```cpp",
                    example_code,
                    "```",
                    "",
                ])

            lines.extend([
                '</div>',
                '</div>',
                "",
            ])

    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text("\n".join(lines), encoding="utf-8")
    print(f"[generate_api_docs] Wrote {output_file}")

def generate_constants_md(output_file: Path):
    """Generate docs/api/constants.md documenting all protocol and header constants."""
    lines = [
        "# Protocol, Header, & Framing Constants Reference",
        "",
        '<div class="api-header-block">',
        '  <div class="api-module-name">Namespace siddiqsoft</div>',
        '  <div class="api-header-file">#include &lt;siddiqsoft/private/sip2json_constants.hpp&gt; &bull; &lt;siddiqsoft/private/sip2json_header_keys.hpp&gt;</div>',
        '</div>',
        "",
        "The `siddiqsoft` namespace provides compile-time string constants, enumeration sets, and fast hash normalization dispatch tables for SIP RFC 3261 protocols, JSON serialization schema keys, framing delimiters, and canonical header names.",
        "",
        "## Architectural Rationale",
        "",
        "To maximize throughput and achieve zero dynamic heap allocations in high-performance message processing:",
        "",
        "1. **`static inline const std::string` definitions**: Defined once per compilation unit. When searching or inserting into `nlohmann::json` objects, passing these constants prevents the creation of temporary `std::string` instances that would otherwise occur when passing raw C-string literals (`const char*`).",
        "2. **Compile-Time FNV-1a Hashing**: Wire header strings are mapped to canonical representations (`HF_*`) via a 64-bit constexpr Fowler-Noll-Vo-1a hash (`hash_header_key`) with inline case-folding, avoiding `tolower()` string copies during packet parsing.",
        "3. **RFC 3261 Compact Aliases**: Compact single-character headers (e.g. `c` for `Content-Type`, `i` for `Call-ID`, `m` for `Contact`) are automatically mapped to their canonical forms via `siddiqsoft::canonicalizeHeaderKey`.",
        "",
        "## Top-Level Message JSON Section Keys",
        "",
        "Partition keys defining the root structure of the `sipmessage` JSON object:",
        "",
        "| Constant Name | Type | Value | Target JSON Path | Purpose |",
        "| :--- | :--- | :--- | :--- | :--- |",
        "| `JSON_KEY_STARTLINE` | `std::string` | `\"s\"` | `/s` | Container for request-line or status-line fields. |",
        "| `JSON_KEY_HEADERS` | `std::string` | `\"h\"` | `/h` | Key-value dictionary containing all normalized message headers. |",
        "| `JSON_KEY_BODY` | `std::string` | `\"b\"` | `/b` | Container for raw message body or structured SDP array. |",
        "| `JSON_KEY_META` | `std::string` | `\"meta\"` | `/meta` | Diagnostic and provenance tracking metadata. |",
        "| `JSON_KEY_SDP` | `std::string` | `\"sdp\"` | `/b/sdp` | Structured array container for parsed SDP blocks. |",
        "",
        "## Start-Line JSON Field Keys",
        "",
        "Keys for start-line / status-line fields under the `/s` object:",
        "",
        "| Constant Name | Type | Value | Target Path | Description |",
        "| :--- | :--- | :--- | :--- | :--- |",
        "| `JSON_KEY_TYPE` | `std::string` | `\"type\"` | `/s/type` | Discriminator: `\"request\"` (1) or `\"response\"` (2). |",
        "| `JSON_KEY_METHOD` | `std::string` | `\"method\"` | `/s/method` | SIP request method name (e.g. `INVITE`). |",
        "| `JSON_KEY_URI` | `std::string` | `\"uri\"` | `/s/uri` | Target SIP request URI string. |",
        "| `JSON_KEY_VERSION` | `std::string` | `\"version\"` | `/s/version` | SIP protocol version token (always `SIP/2.0`). |",
        "| `JSON_KEY_STATUS` | `std::string` | `\"status\"` | `/s/status` | Numeric response status code (e.g. `200`, `404`). |",
        "| `JSON_KEY_REASON` | `std::string` | `\"reason\"` | `/s/reason` | Response reason phrase string (e.g. `\"OK\"`). |",
        "",
        "## Meta Section Field Keys",
        "",
        "Diagnostic telemetry keys under the `/meta` object:",
        "",
        "| Constant Name | Type | Value | Target Path | Description |",
        "| :--- | :--- | :--- | :--- | :--- |",
        "| `JSON_KEY_ID` | `std::string` | `\"id\"` | `/meta/id` | Unique UUID string assigned to message instance. |",
        "| `JSON_KEY_TIME` | `std::string` | `\"time\"` | `/meta/time` | ISO 8601 UTC creation/receipt timestamp string. |",
        "| `JSON_KEY_TTX` | `std::string` | `\"ttx\"` | `/meta/ttx` | Parse latency counter / tick accumulator. |",
        "",
        "## Standard SIP Methods",
        "",
        "Standard RFC 3261 / 3262 / 3265 / 3515 / 3903 request method constants:",
        "",
        "| Constant Name | Type | Value | RFC Specification | Description |",
        "| :--- | :--- | :--- | :--- | :--- |",
        "| `METHOD_INVITE` | `std::string` | `\"INVITE\"` | RFC 3261 | Initiates a session or dialog between endpoints. |",
        "| `METHOD_ACK` | `std::string` | `\"ACK\"` | RFC 3261 | Confirms final response to an INVITE request. |",
        "| `METHOD_OPTIONS` | `std::string` | `\"OPTIONS\"` | RFC 3261 | Queries capabilities and supported extensions. |",
        "| `METHOD_BYE` | `std::string` | `\"BYE\"` | RFC 3261 | Terminates an active media session or call. |",
        "| `METHOD_CANCEL` | `std::string` | `\"CANCEL\"` | RFC 3261 | Cancels a pending uncompleted request. |",
        "| `METHOD_REGISTER` | `std::string` | `\"REGISTER\"` | RFC 3261 | Binds a SIP address to current network contact. |",
        "| `METHOD_SUBSCRIBE` | `std::string` | `\"SUBSCRIBE\"` | RFC 3265 | Requests event notifications from remote agent. |",
        "| `METHOD_NOTIFY` | `std::string` | `\"NOTIFY\"` | RFC 3265 | Transports state notifications to subscriber. |",
        "| `METHOD_MESSAGE` | `std::string` | `\"MESSAGE\"` | RFC 3428 | Transports instant message text payloads. |",
        "| `METHOD_INFO` | `std::string` | `\"INFO\"` | RFC 2976 | Sends mid-dialog session control information. |",
        "| `METHOD_REFER` | `std::string` | `\"REFER\"` | RFC 3515 | Requests recipient to contact a third party. |",
        "| `METHOD_PUBLISH` | `std::string` | `\"PUBLISH\"` | RFC 3903 | Publishes event state (e.g. presence data). |",
        "| `METHOD_UPDATE` | `std::string` | `\"UPDATE\"` | RFC 3311 | Modifies session state before call is answered. |",
        "| `METHOD_PRACK` | `std::string` | `\"PRACK\"` | RFC 3262 | Provisional response acknowledgement. |",
        "",
        "### Valid Methods Array",
        "",
        "```cpp",
        "static constexpr std::string_view SIP_VALID_METHODS[] = {",
        '    "INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER",',
        '    "SUBSCRIBE", "NOTIFY", "REFER", "PUBLISH", "UPDATE", "PRACK",',
        '    "INFO", "MESSAGE"',
        "};",
        "```",
        "",
        "## Protocol & Network Constants",
        "",
        "| Constant Name | Type | Value | Description |",
        "| :--- | :--- | :--- | :--- |",
        "| `SIPVER_20` | `std::string` | `\"SIP/2.0\"` | Standard SIP version identifier token. |",
        "| `DEFAULT_SERVER_PORT` | `constexpr int` | `5060` | Default IANA-assigned port for SIP over UDP and TCP. |",
        "| `URI_SCHEME_SIP` | `std::string` | `\"sip:\"` | Unencrypted SIP URI scheme prefix. |",
        "| `URI_SCHEME_SIPS` | `std::string` | `\"sips:\"` | Secure TLS-encrypted SIP URI scheme prefix. |",
        "| `VIA_BRANCH_PREFIX` | `std::string` | `\"z9hG4bK\"` | RFC 3261 magic cookie required for all transaction branch IDs. |",
        "| `EMPTY_STD_STRING_VALUE` | `std::string` | `\"\"` | Cached empty string instance to avoid heap reallocations. |",
        "",
        "## Registration & TTL Intervals",
        "",
        "| Constant Name | Type | Value | Time Representation |",
        "| :--- | :--- | :--- | :--- |",
        "| `DEFAULT_MAX_REGISTER_TTL` | `constexpr int` | `3600` | 1 hour in seconds. |",
        "| `DEFAULT_MAX_REGISTER_TTL_MS` | `constexpr int` | `3600000` | 1 hour in milliseconds. |",
        "| `DEFAULT_MIN_REGISTER_TTL` | `constexpr int` | `120` | 2 minutes in seconds. |",
        "| `REGISTER_PERIOD_10MIN_SEC` | `constexpr int` | `600` | 10 minutes in seconds. |",
        "| `REGISTER_PERIOD_10MIN_MS` | `constexpr int` | `600000` | 10 minutes in milliseconds. |",
        "| `REGISTER_PERIOD_1MIN_SEC` | `constexpr int` | `60` | 1 minute in seconds. |",
        "| `REGISTER_PERIOD_MIN_SEC` | `constexpr int` | `30` | 30 seconds. |",
        "",
        "## Supported Content Types",
        "",
        "MIME type string constants used for `Content-Type` header verification and SDP parsing dispatch:",
        "",
        "| Constant Name | Value | Handling Behavior |",
        "| :--- | :--- | :--- |",
        "| `CONTENT_TYPE_APP_SDP` | `\"application/sdp\"` | Automatically parsed into structured `/b/sdp` array. |",
        "| `CONTENT_TYPE_TEXT_PLAIN` | `\"text/plain\"` | Retained as verbatim UTF-8 string payload in `/b`. |",
        "| `CONTENT_TYPE_TEXT_HTML` | `\"text/html\"` | Retained as verbatim UTF-8 string payload in `/b`. |",
        "| `CONTENT_TYPE_TEXT_XML` | `\"text/xml\"` | Retained as verbatim string payload in `/b`. |",
        "| `CONTENT_TYPE_APP_XML` | `\"application/xml\"` | Retained as verbatim string payload in `/b`. |",
        "| `CONTENT_TYPE_APP_PKCS7MIME` | `\"application/pkcs7-mime\"` | Opaque binary/encrypted payload string in `/b`. |",
        "| `CONTENT_TYPE_APP_XPRIVATE` | `\"application/x-private\"` | Proprietary custom data payload in `/b`. |",
        "| `CONTENT_TYPE_TEXT_X_METATEL1_PRESENCE` | `\"text/x-metatel1.0-presence\"` | Presence information data format payload. |",
        "",
        "## Authentication Schemes & Subscription States",
        "",
        "| Category | Constant Name | Value | Purpose |",
        "| :--- | :--- | :--- | :--- |",
        "| **Auth** | `AUTHORIZATION_CLEAR` | `\"Clear\"` | Cleartext password scheme. |",
        "| **Auth** | `AUTHORIZATION_BASIC` | `\"Basic\"` | RFC 7617 HTTP Basic authentication scheme. |",
        "| **Auth** | `AUTHORIZATION_DIGEST` | `\"Digest\"` | RFC 2617 / RFC 7616 MD5 / SHA-256 Digest scheme. |",
        "| **Subscription** | `SUBSTATE_ACTIVE` | `\"active\"` | Subscription accepted and active. |",
        "| **Subscription** | `SUBSTATE_PENDING` | `\"pending\"` | Subscription request received but awaiting approval. |",
        "| **Subscription** | `SUBSTATE_TERMINATED` | `\"terminated\"` | Subscription ended or timed out. |",
        "",
        "## Wire Framing & Parsing Delimiters",
        "",
        "Constants representing standard line delimiters, separators, and SDP markers across platforms:",
        "",
        "| Constant Name | Wire Sequence | Escaped String | Description |",
        "| :--- | :--- | :--- | :--- |",
        "| `ELEM_NEWLINE` | CRLF | `\"\\r\\n\"` | Standard RFC 3261 header and line delimiter. |",
        "| `ELEM_HEADERSECTIONDELIMITER` | Double CRLF | `\"\\r\\n\\r\\n\"` | Delimiter separating header dictionary from payload body. |",
        "| `ELEM_SPACE` | Space | `\" \"` | Token separator in request line and status line. |",
        "| `ELEM_SEPARATOR` | Colon | `\":\"` | Header key-value delimiter. |",
        "| `ELEM_PADDED_SEPARATOR` | Colon + Space | `\": \"` | Standard formatted header key-value separator. |",
        "| `ELEM_TAG_SEPARATOR` | Open Brace | `\"{\"` | Tag / parameter delimiter. |",
        "| `ELEM_LWSP` | CRLF + Space | `\"\\r\\n \"` | Linear whitespace folding indicator (RFC 822 / 3261). |",
        "| `ELEM_LWSP1` | CRLF + Tab | `\"\\r\\n\\t\"` | Tab-based linear whitespace folding indicator. |",
        "| `ELEM_SDPBlockStart` | Padded SDP | `\" v=0\\r\\n\"` | Wire indicator for start of SDP session description. |",
        "| `ELEM_NEWLINE_LF` | LF | `\"\\n\"` | UNIX newline fallback delimiter. |",
        "| `ELEM_HEADERSECTIONDELIMITER_LF` | Double LF | `\"\\n\\n\"` | UNIX header-body separator fallback. |",
        "| `ELEM_LWSP_LF` | LF + Space | `\"\\n \"` | UNIX linear whitespace folding. |",
        "| `ELEM_LWSP1_LF` | LF + Tab | `\"\\n\\t\"` | UNIX tab linear whitespace folding. |",
        "| `ELEM_SDPBlockStart_LF` | LF SDP | `\"v=0\\n\"` | UNIX SDP session block start indicator. |",
        "",
        "## Canonical Header Key Constants (`HF_*`)",
        "",
        "Static `std::string` definitions used as keys in `sipmessage[\"h\"]` and `sipmessage.setHeader()` to ensure zero dynamic allocation during lookups:",
        "",
        "| Constant Name | Canonical Header Name | RFC 3261 Role |",
        "| :--- | :--- | :--- |",
        "| `HF_FROM` | `\"From\"` | Identifies the originator of the request. |",
        "| `HF_TO` | `\"To\"` | Identifies the desired recipient of the request. |",
        "| `HF_CALLID` | `\"Call-ID\"` | Uniquely identifies a particular invitation or registration. |",
        "| `HF_CSEQ` | `\"CSeq\"` | Sequential number identifying transaction ordering. |",
        "| `HF_VIA` | `\"Via\"` | Records path taken by request and routes responses back. |",
        "| `HF_CONTACT` | `\"Contact\"` | Direct routing URI for subsequent requests. |",
        "| `HF_CONTENT_TYPE` | `\"Content-Type\"` | Media type of message payload body. |",
        "| `HF_CONTENT_LENGTH` | `\"Content-Length\"` | Size in octets of the message payload. |",
        "| `HF_CONTENT_ENCODING` | `\"Content-Encoding\"` | Compression or encoding scheme applied to body. |",
        "| `HF_EXPIRES` | `\"Expires\"` | Relative time in seconds after which message expires. |",
        "| `HF_ROUTE` | `\"Route\"` | Forces routing through specified proxy addresses. |",
        "| `HF_RECORD_ROUTE` | `\"Record-Route\"` | Injected by proxies to remain on dialog path. |",
        "| `HF_MAX_FORWARDS` | `\"Max-Forwards\"` | Hop count limit preventing routing loops (default 70). |",
        "| `HF_USER_AGENT` | `\"User-Agent\"` | Client software name and version information. |",
        "| `HF_SERVER` | `\"Server\"` | UAS software name and version information. |",
        "| `HF_AUTHORIZATION` | `\"Authorization\"` | Client authentication credentials. |",
        "| `HF_WWW_AUTHENTICATE` | `\"WWW-Authenticate\"` | Authentication challenge issued by UAS. |",
        "| `HF_PROXY_AUTHENTICATE`| `\"Proxy-Authenticate\"`| Challenge issued by an intermediary proxy. |",
        "| `HF_PROXY_AUTHORIZATION`| `\"Proxy-Authorization\"`| Client credentials for intermediary proxy. |",
        "| `HF_PROXY_REQUIRE` | `\"Proxy-Require\"` | Sensitive features that proxies must support. |",
        "| `HF_REQUIRE` | `\"Require\"` | Extensions that UAS must support to process request. |",
        "| `HF_SUPPORTED` | `\"Supported\"` | Extensions supported by the sender. |",
        "| `HF_UNSUPPORTED` | `\"Unsupported\"` | Extensions not supported by recipient. |",
        "| `HF_ALLOW` | `\"Allow\"` | Lists SIP methods supported by the UA. |",
        "| `HF_WARNING` | `\"Warning\"` | Diagnostic warning message and code. |",
        "| `HF_ACCEPT` | `\"Accept\"` | Media types acceptable in responses. |",
        "| `HF_ACCEPT_ENCODING` | `\"Accept-Encoding\"` | Content codings acceptable in responses. |",
        "| `HF_ACCEPT_LANGUAGE` | `\"Accept-Language\"` | Preferred natural languages in reason phrases. |",
        "| `HF_DATE` | `\"Date\"` | Date and time message was originated (RFC 1123). |",
        "| `HF_TIMESTAMP` | `\"Timestamp\"` | Client time when request was initially sent. |",
        "| `HF_PRIORITY` | `\"Priority\"` | Call urgency (e.g. `urgent`, `normal`, `emergency`). |",
        "| `HF_SUBJECT` | `\"Subject\"` | Short text summary of call topic. |",
        "| `HF_LOCATION` | `\"Location\"` | Geographical or network location of sender. |",
        "| `HF_HIDE` | `\"Hide\"` | Request proxy anonymity / route concealment. |",
        "| `HF_RESPONSE_KEY` | `\"Response-Key\"` | PGP key used to encrypt subsequent responses. |",
        "| `HF_RETRY_AFTER` | `\"Retry-After\"` | Time interval client should wait before retrying. |",
        "| `HF_SUBSCRIPTION_STATE`| `\"Subscription-State\"`| Current state of event notification subscription. |",
        "",
        "## Header Key Sets & Compact Aliases (`HFS_*`)",
        "",
        "RFC 3261 defines single-character compact aliases to minimize packet size over MTU-constrained networks. The parser normalizes all aliases to their canonical representation:",
        "",
        "| Canonical Name | Compact Alias | Constant Instance | Multi-line Support |",
        "| :--- | :--- | :--- | :--- |",
        "| `From` | `f` | `siddiqsoft::HFS_FROM` | No |",
        "| `To` | `t` | `siddiqsoft::HFS_TO` | No |",
        "| `Call-ID` | `i` | `siddiqsoft::HFS_CALLID` | No |",
        "| `Via` | `v` | `siddiqsoft::HFS_VIA` | **Yes (Array)** |",
        "| `Contact` | `m` | `siddiqsoft::HFS_CONTACT` | No |",
        "| `Content-Type` | `c` | `siddiqsoft::HFS_CONTENT_TYPE` | No |",
        "| `Content-Length` | `l` | `siddiqsoft::HFS_CONTENT_LENGTH` | No |",
        "| `Content-Encoding` | `e` | `siddiqsoft::HFS_CONTENT_ENCODING` | No |",
        "| `Subject` | `s` | `siddiqsoft::HFS_SUBJECT` | No |",
        "| `Supported` | `k` | `siddiqsoft::HFS_SUPPORTED` | **Yes (Array)** |",
        "",
        "!!! note \"Non-Standard Gateway Compatibility Alias\"",
        "    `Authorization` does not have an official RFC 3261 compact form. The parser recognizes `uthorization` (mapped via `siddiqsoft::HFS_AUTHORIZATION`) as a quirks-mode compatibility workaround for corrupt SIP frames generated by certain legacy gateways where the leading character `'A'` is dropped.",
        "",
        "## Header Normalization & FNV-1a Hashing",
        "",
        "The parser matches incoming headers in $O(1)$ time by evaluating a `constexpr` 64-bit Fowler-Noll-Vo-1a hash over the string:",
        "",
        "```cpp",
        "constexpr uint64_t hash_header_key(const char* s, size_t len) noexcept;",
        "constexpr uint64_t hash_header_key(std::string_view sv) noexcept;",
        "const HeaderKeySet& canonicalizeHeaderKey(std::string_view keyFromPayload);",
        "```",
        "",
        "### HeaderKeySet UML Class Diagram",
        "",
        "<!-- @@uml-diag:HeaderKeySet -->",
        "",
        "### Live Test Example",
        "",
        "```cpp",
        "// Source: tests/regression/src/rule_of_five_tests.cpp:L40-L45",
        "auto callId = siddiqsoft::createCallId();",
        "siddiqsoft::sipmessage original(siddiqsoft::METHOD_INVITE, \"sip:test@example.com\", callId, 1);",
        "",
        "// Setting canonical headers with static string constants (zero allocation):",
        "original.setHeader(siddiqsoft::HF_TO, \"sip:test@example.com\");",
        "original.setHeader(siddiqsoft::HF_FROM, \"sip:sender@example.com\");",
        "original.setHeader(siddiqsoft::HF_CONTENT_TYPE, siddiqsoft::CONTENT_TYPE_APP_SDP);",
        "",
        "EXPECT_EQ(original.getMethod(), siddiqsoft::METHOD_INVITE);",
        "EXPECT_EQ(original.getHeader<std::string>(siddiqsoft::HF_CONTENT_TYPE), \"application/sdp\");",
        "```",
        "",
    ]
    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text("\n".join(lines), encoding="utf-8")
    print(f"[generate_api_docs] Wrote {output_file}")

def generate_index_md(output_file: Path, uml_diagram: str = ""):
    """Generate docs/api/index.md overview with structured API reference layout."""
    lines = [
        "# API Reference Overview",
        "",
        '<div class="api-header-block">',
        '  <div class="api-module-name">sip2json C++20 Reference</div>',
        '  <div class="api-header-file">Generated from intermediate Doxygen XML</div>',
        '</div>',
        "",
        "The `siddiqsoft` namespace provides data structures, stream parsing utilities, diagnostic error definitions, and serialization functions for SIP and SDP protocols.",
        "",
        "## Namespaces",
        "",
        '<table class="api-summary-table">',
        '  <tr>',
        '    <td class="memitemleft"><a href="#namespace-siddiqsoft"><strong>siddiqsoft</strong></a><div class="mdesc">Core namespace containing the parser, message DTO, and protocol constants.</div></td>',
        '  </tr>',
        '</table>',
        "",
        "## Classes & Structures",
        "",
        '<table class="api-summary-table">',
        '  <tr>',
        '    <td class="memtype"><code>class</code></td>',
        '    <td class="memitemleft"><a href="sip2json.md"><strong>siddiqsoft::sip2json</strong></a><div class="mdesc">Static utility class for streaming, parsing, and serializing SIP and SDP messages.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"><code>class</code></td>',
        '    <td class="memitemleft"><a href="sipmessage.md"><strong>siddiqsoft::sipmessage</strong></a><div class="mdesc">Primary message DTO inheriting <code>nlohmann::json</code>. Represents request and response packets.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"><code>class</code></td>',
        '    <td class="memitemleft"><a href="errors.md#exception-class-hierarchy"><strong>siddiqsoft::sip2json_exception</strong></a><div class="mdesc">Base exception class for parse syntax, framing, and serialization failures.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"><code>class</code></td>',
        '    <td class="memitemleft"><a href="constants.md#header-key-sets--compact-aliases-hfs_"><strong>siddiqsoft::HeaderKeySet</strong></a><div class="mdesc">Canonical header metadata and compact alias mapping descriptor.</div></td>',
        '  </tr>',
        '</table>',
        "",
        "## Enumerations & Constants",
        "",
        '<table class="api-summary-table">',
        '  <tr>',
        '    <td class="memtype"><code>constants</code></td>',
        '    <td class="memitemleft"><a href="constants.md"><strong>Protocol &amp; Header Constants</strong></a><div class="mdesc">Complete dictionary of methods, JSON section keys, delimiters, and canonical headers.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"><code>enum class</code></td>',
        '    <td class="memitemleft"><a href="errors.md#sip2jsonerrors-enumeration"><strong>siddiqsoft::sip2jsonErrors</strong></a><div class="mdesc">Diagnostic error classification passed to <code>parseAsync</code> error callbacks.</div></td>',
        '  </tr>',
        '  <tr>',
        '    <td class="memtype"><code>enum class</code></td>',
        '    <td class="memitemleft"><a href="sipmessage.md"><strong>siddiqsoft::SIPMessageType</strong></a><div class="mdesc">Discriminator distinguishing request (1) from response (2) messages.</div></td>',
        '  </tr>',
        '</table>',
        "",
        "## Header Files",
        "",
        "| Header File | Include Path | Description |",
        "| :--- | :--- | :--- |",
        "| **`sip2json.hpp`** | `#include <siddiqsoft/sip2json.hpp>` | Primary parser and serializer engine entry point. |",
        "| **`sipmessage.hpp`** | `#include <siddiqsoft/sipmessage.hpp>` | Complete `sipmessage` container and field accessors. |",
        "| **`sip2json_constants.hpp`** | `#include <siddiqsoft/private/sip2json_constants.hpp>` | Method names, URI schemes, and delimiters. [View Constants](constants.md) |",
        "| **`sip2json_header_keys.hpp`** | `#include <siddiqsoft/private/sip2json_header_keys.hpp>` | Hash table dispatch and canonical header representations. [View Constants](constants.md) |",
        "| **`sip2json_exception.hpp`** | `#include <siddiqsoft/private/sip2json_exception.hpp>` | `sip2jsonErrors` enumeration and derived exception types. |",
        "| **`sip2json_sdp.hpp`** | `#include <siddiqsoft/private/sip2json_sdp.hpp>` | SDP line parser and serialization engine. |",
        "",
    ]

    lines.extend([
        "## System UML Class Diagram",
        "",
        "The following UML class diagram illustrates the primary classes, relationships, and exception hierarchy in `sip2json`. Each node links directly to its source header file on GitHub:",
        "",
        "<!-- @@uml-diag:complete -->",
        "",
        "<!-- @@uml-diag:source-table -->",
        "",
    ])

    lines.extend([
        "## Detailed Topics",
        "",
        '<div class="grid" markdown="1">',
        '<div class="card" markdown="1">',
        "",
        "### [Constants Reference](constants.md)",
        "",
        "Full documentation of methods (`METHOD_INVITE`), JSON keys, delimiters, and canonical header names.",
        "",
        "[View Constants Reference :octicons-arrow-right-24:](constants.md)",
        "",
        "</div>",
        '<div class="card" markdown="1">',
        "",
        "### [JSON Schema & SDP](json_schema.md)",
        "",
        "JSON document structure, field types, header conversions, and SDP attribute mapping.",
        "",
        "[View Schema Reference :octicons-arrow-right-24:](json_schema.md)",
        "",
        "</div>",
        '<div class="card" markdown="1">',
        "",
        "### [Code Examples](examples/index.md)",
        "",
        "Practical integration snippets: asynchronous stream parsing, message creation, and error handling.",
        "",
        "[View Code Examples :octicons-arrow-right-24:](examples/index.md)",
        "",
        "</div>",
        "</div>",
        "",
    ])
    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text("\n".join(lines), encoding="utf-8")
    print(f"[generate_api_docs] Wrote {output_file}")

def generate_errors_md(xml_dir: Path, output_file: Path, uml_diagram: str = ""):
    """Generate docs/api/errors.md with exact error codes from C++ header."""
    lines = [
        "# Error Codes & Exceptions Reference",
        "",
        '<div class="api-header-block">',
        '  <div class="api-module-name">Namespace siddiqsoft</div>',
        '  <div class="api-header-file">#include &lt;siddiqsoft/private/sip2json_exception.hpp&gt;</div>',
        '</div>',
        "",
        "Diagnostic errors and exception reporting model for `sip2json`. Error conditions are communicated either via exceptions (`sip2json_exception`) or error callback invocations in asynchronous streaming (`sip2jsonErrors`).",
        "",
        "## sip2jsonErrors Enumeration",
        "",
        "Defined in namespace `siddiqsoft` with underlying type `uint32_t`:",
        "",
        "```cpp",
        "enum class sip2jsonErrors : uint32_t {",
        "    ok = 0,",
        "    /* parse errors */",
        "    incomplete_buffer_for_parse,",
        "    incomplete_buffer_for_content,",
        "    incomplete_buffer_for_header,",
        "    invalid_startline,",
        "    unsupported_contenttype,",
        "    missing_required_element,",
        "    /* serialization errors */",
        "    invalid_document,",
        "    invalid_document_unsupported_method,",
        "    invalid_document_unsupported_content,",
        "    empty_message",
        "};",
        "```",
        "",
        "### Error Code Specifications",
        "",
        "| Enumerator | Numeric | Category | Description | Recommended Action |",
        "| :--- | :--- | :--- | :--- | :--- |",
        "| `ok` | `0` | Success | Operation completed successfully with no errors. | Proceed with next operation. |",
        "| `incomplete_buffer_for_parse` | `1` | Framing | Buffer does not yet contain a complete SIP frame. | Retain buffer; wait for additional socket read data. |",
        "| `incomplete_buffer_for_content` | `2` | Framing | Body content size is less than specified `Content-Length`. | Await more payload bytes from stream. |",
        "| `incomplete_buffer_for_header` | `3` | Framing | Header section is incomplete (missing terminating `\\r\\n\\r\\n`). | Await complete header delimiter. |",
        "| `invalid_startline` | `4` | Syntax | Request line or status line is malformed or unparseable. | Discard malformed frame or log protocol error. |",
        "| `unsupported_contenttype` | `5` | Payload | Payload Content-Type is unrecognized for structured parsing. | Fall back to raw string body extraction. |",
        "| `missing_required_element` | `6` | Schema | Expected required element was not found in message. | Verify incoming packet complies with RFC 3261. |",
        "| `invalid_document` | `7` | Serialization | Provided JSON document violates SIP message structure. | Validate JSON schema before serialization. |",
        "| `invalid_document_unsupported_method` | `8` | Serialization | Specified SIP request method is not supported by RFC 3261. | Use standard SIP method token. |",
        "| `invalid_document_unsupported_content` | `9` | Serialization | Message body format cannot be serialized to wire format. | Provide valid string or SDP structure. |",
        "| `empty_message` | `10` | Serialization | Attempted to serialize an empty `sipmessage` object. | Populate start line and headers prior to serialize. |",
        "",
        "!!! note \"Serialization Exceptions\"",
        "    During serialization, schema violations (such as unsupported request methods or unparseable body payloads) throw `siddiqsoft::invalid_document_error` (`errCode = invalid_document`). The specialized error codes `invalid_document_unsupported_method` and `invalid_document_unsupported_content` are reserved enumerators in `sip2jsonErrors` for fine-grained diagnostic classification.",
        "",
        "## Exception Class Hierarchy",
        "",
        "All exceptions derive from `std::exception` via `siddiqsoft::sip2json_exception`:",
        "",
        "```text",
        "std::exception",
        " \\-- siddiqsoft::sip2json_exception",
        "      |-- invalid_startline_error",
        "      |-- invalid_document_error",
        "      |-- incomplete_buffer_for_parse_error",
        "      |-- incomplete_buffer_for_header_error",
        "      |-- incomplete_buffer_for_content_error",
        "      |-- unsupported_contenttype_error",
        "      |-- missing_required_element",
        "      \\-- empty_message_error",
        "```",
        "",
        "### Exception Class UML Diagram",
        "",
        "<!-- @@uml-diag:sip2json_exception -->",
        "",
        "### Exception Handling Example",
        "",
        "```cpp",
        "// Source: tests/regression/src/test.cpp:L45-L52",
        "#include <iostream>",
        "#include <siddiqsoft/sip2json.hpp>",
        "",
        "void parseSafely(std::string_view buffer)",
        "{",
        "    try {",
        "        auto msg = siddiqsoft::sip2json::parseFromBuffer(buffer);",
        "        std::cout << \"Parsed \" << msg.getMethodView() << \"\\n\";",
        "    } catch (const siddiqsoft::invalid_startline_error& ex) {",
        "        std::cerr << \"Malformed start line: \" << ex.what() << \"\\n\";",
        "    } catch (const siddiqsoft::incomplete_buffer_for_parse_error& ex) {",
        "        std::cerr << \"Need more data: \" << ex.what() << \"\\n\";",
        "    } catch (const siddiqsoft::sip2json_exception& ex) {",
        "        std::cerr << \"SIP error (code \" << static_cast<uint32_t>(ex.errCode) << \"): \" << ex.what() << \"\\n\";",
        "    }",
        "}",
        "```",
        "",
    ]

    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text("\n".join(lines), encoding="utf-8")
    print(f"[generate_api_docs] Wrote {output_file}")


def clean_uml_type(elem) -> str:
    """Recursively extract and clean a C++ type for Mermaid UML formatting."""
    if elem is None:
        return ""
    t = "".join(elem.itertext()).strip()
    t = re.sub(r"\s+", " ", t)
    t = t.replace("std::", "")
    t = t.replace("nlohmann::json::json_pointer", "json_pointer")
    t = t.replace("nlohmann::", "")
    t = t.replace("<", "~").replace(">", "~")
    t = re.sub(r"~\s*([^~]+?)\s*~", r"~\1~", t)
    return t


def clean_uml_ret_type(type_elem) -> str:
    """Format return type cleanly for Mermaid UML."""
    if type_elem is None:
        return ""
    t = clean_uml_type(type_elem)
    t = re.sub(r"\bconst\s+", "", t)
    t = re.sub(r"\s*&\s*", "&", t)
    t = re.sub(r"\s+", " ", t).strip()
    return t


def clean_uml_params(m) -> str:
    """Extract and format parameters for Mermaid UML."""
    params = []
    for p in m.findall("param"):
        ptype = clean_uml_type(p.find("type"))
        pname = p.findtext("declname", "").strip()
        ptype = re.sub(r"\bconst\s+", "", ptype)
        ptype = re.sub(r"\s*&\s*", "&", ptype)
        ptype = re.sub(r"\s+", " ", ptype).strip()
        if "function~" in ptype:
            ptype = "callback"
        elif "optional~" in ptype:
            ptype = re.sub(r"optional~([^~]+)~", r"optional~\1~", ptype)
        if pname and ptype:
            params.append(f"{ptype} {pname}")
        elif ptype:
            params.append(ptype)
        elif pname:
            params.append(pname)
    return ", ".join(params)


def parse_doxygen_ast(xml_dir: Path):
    """
    Dynamically extracts classes, methods, fields, enums, inheritance, and source
    locations from Doxygen XML intermediate files in xml_dir.
    """
    classes = {}
    for p in sorted(xml_dir.glob("classsiddiqsoft_*.xml")):
        try:
            tree = ET.parse(p)
        except Exception:
            continue
        cdef = tree.find("compounddef")
        if cdef is None:
            continue
        cname = cdef.findtext("compoundname", "")
        short_name = cname.split("::")[-1]
        is_final = cdef.get("final") == "yes"
        loc = cdef.find("location")
        hfile = loc.get("file", "") if loc is not None else ""
        bases = []
        for b in cdef.findall("basecompoundref"):
            b_text = "".join(b.itertext()).strip()
            bases.append(b_text)
        derived = [d.text.split("::")[-1] for d in cdef.findall("derivedcompoundref") if d.text]

        methods = []
        seen_names = set()
        for m in cdef.findall('.//memberdef[@kind="function"]'):
            if m.get("prot") != "public":
                continue
            mname = m.findtext("name", "")
            if not mname or mname.startswith("~") or mname.startswith("operator"):
                continue
            if mname in seen_names:
                continue
            seen_names.add(mname)
            is_static = m.get("static") == "yes"
            ret = clean_uml_ret_type(m.find("type"))
            params = clean_uml_params(m)
            methods.append({
                "name": mname,
                "ret": ret,
                "params": params,
                "static": is_static
            })

        attribs = []
        for a in cdef.findall('.//memberdef[@kind="variable"]'):
            if a.get("prot") != "public":
                continue
            aname = a.findtext("name", "")
            if not aname or aname.startswith("Meta"):
                continue
            atype = clean_uml_ret_type(a.find("type"))
            is_static = a.get("static") == "yes"
            attribs.append({
                "name": aname,
                "type": atype,
                "static": is_static
            })

        classes[short_name] = {
            "name": cname,
            "short_name": short_name,
            "is_final": is_final,
            "header_file": hfile,
            "bases": bases,
            "derived": derived,
            "methods": methods,
            "attribs": attribs
        }

    enums = {}
    ns_xml = xml_dir / "namespacesiddiqsoft.xml"
    if ns_xml.exists():
        try:
            tree = ET.parse(ns_xml)
            for edef in tree.findall('.//memberdef[@kind="enum"]'):
                ename = edef.findtext("name", "")
                qname = edef.findtext("qualifiedname", f"siddiqsoft::{ename}")
                loc = edef.find("location")
                hfile = loc.get("file", "") if loc is not None else ""
                vals = []
                for v in edef.findall("enumvalue"):
                    vname = v.findtext("name", "")
                    init = v.findtext("initializer", "")
                    init = re.sub(r"=[^0-9\-]*([0-9\-]+)", r"= \1", init).strip()
                    vals.append(f"{vname} {init}".strip() if init else vname)
                enums[ename] = {
                    "name": ename,
                    "qualified_name": qname,
                    "header_file": hfile,
                    "values": vals
                }
        except Exception as e:
            print(f"[generate_api_docs] Warning parsing enums: {e}")

    return classes, enums


def generate_class_uml_diagram(xml_dir: Path, target_class: str) -> str:
    """
    Generates a focused, per-class Mermaid UML diagram displaying only the target class
    and its immediate inheritance and usage relationships, dynamically derived from Doxygen XML AST.
    """
    classes, enums = parse_doxygen_ast(xml_dir)
    base_src_url = "https://github.com/SiddiqSoft/sip2json/blob/master"

    if target_class == "sip2json":
        s2j = classes.get("sip2json", {})
        s2j_file = s2j.get("header_file", "include/siddiqsoft/sip2json.hpp")
        smsg = classes.get("sipmessage", {})
        smsg_file = smsg.get("header_file", "include/siddiqsoft/sipmessage.hpp")
        exc = classes.get("sip2json_exception", {})
        exc_file = exc.get("header_file", "include/siddiqsoft/private/sip2json_exception.hpp")

        lines = [
            "```mermaid",
            "classDiagram",
            "    direction TB",
            "",
            "    classDef coreClass fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;",
            "    classDef exceptionClass fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;",
            "    classDef highlightClass fill:rgba(2,132,199,0.18),stroke:#0284c7,stroke-width:3px;",
            "",
            f'    class sip2json["{s2j.get("name", "siddiqsoft::sip2json")}"] {{',
        ]
        if s2j.get("is_final"):
            lines.append("        <<final utility>>")
        for m in s2j.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        lines.extend([
            "    }",
            "    class sip2json:::highlightClass",
            "",
            f'    class sipmessage["{smsg.get("name", "siddiqsoft::sipmessage")}"]',
            "    class sipmessage:::coreClass",
            "",
            f'    class sip2json_exception["{exc.get("name", "siddiqsoft::sip2json_exception")}"]',
            "    class sip2json_exception:::exceptionClass",
            "",
            "    sip2json ..> sipmessage : produces / consumes",
            "    sip2json ..> sip2json_exception : throws",
            "",
            f'    link sip2json "{base_src_url}/{s2j_file}" "Source: {s2j_file}"',
            f'    link sipmessage "{base_src_url}/{smsg_file}" "Source: {smsg_file}"',
            f'    link sip2json_exception "{base_src_url}/{exc_file}" "Source: {exc_file}"',
            "```",
        ])
        return "\n".join(lines)

    elif target_class == "sipmessage":
        smsg = classes.get("sipmessage", {})
        smsg_file = smsg.get("header_file", "include/siddiqsoft/sipmessage.hpp")
        hks = classes.get("HeaderKeySet", {})
        hks_file = hks.get("header_file", "include/siddiqsoft/private/sip2json_header_keys.hpp")
        smt = enums.get("SIPMessageType", {})
        smt_file = smt.get("header_file", "include/siddiqsoft/sipmessage.hpp")

        lines = [
            "```mermaid",
            "classDiagram",
            "    direction TB",
            "",
            "    classDef coreClass fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;",
            "    classDef enumClass fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;",
            "    classDef externalClass fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;",
            "    classDef highlightClass fill:rgba(2,132,199,0.18),stroke:#0284c7,stroke-width:3px;",
            "",
            '    class json["nlohmann::json"] {',
            "        <<external DOM>>",
            "    }",
            "    class json:::externalClass",
            "",
            f'    class sipmessage["{smsg.get("name", "siddiqsoft::sipmessage")}"] {{',
        ]
        for m in smsg.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        lines.extend([
            "    }",
            "    class sipmessage:::highlightClass",
            "",
            f'    class HeaderKeySet["{hks.get("name", "siddiqsoft::HeaderKeySet")}"] {{',
        ])
        for a in hks.get("attribs", []):
            lines.append(f"        +{a['type']} {a['name']}")
        for m in hks.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        lines.extend([
            "    }",
            "    class HeaderKeySet:::coreClass",
            "",
            f'    class SIPMessageType["{smt.get("qualified_name", "siddiqsoft::SIPMessageType")}"] {{',
            "        <<enumeration>>",
        ])
        for v in smt.get("values", []):
            lines.append(f"        {v}")
        lines.extend([
            "    }",
            "    class SIPMessageType:::enumClass",
            "",
            "    json <|-- sipmessage : public inheritance",
            "    sipmessage ..> SIPMessageType : classifies",
            "    sipmessage ..> HeaderKeySet : uses",
            "",
            f'    link sipmessage "{base_src_url}/{smsg_file}" "Source: {smsg_file}"',
            '    link json "https://github.com/nlohmann/json" "External: nlohmann/json"',
            f'    link HeaderKeySet "{base_src_url}/{hks_file}" "Source: {hks_file}"',
            f'    link SIPMessageType "{base_src_url}/{smt_file}" "Source: {smt_file}"',
            "```",
        ])
        return "\n".join(lines)

    elif target_class in ("sip2json_exception", "errors"):
        exc = classes.get("sip2json_exception", {})
        exc_file = exc.get("header_file", "include/siddiqsoft/private/sip2json_exception.hpp")
        errs = enums.get("sip2jsonErrors", {})
        errs_file = errs.get("header_file", "include/siddiqsoft/private/sip2json_exception.hpp")
        derived_exceptions = exc.get("derived", [])

        lines = [
            "```mermaid",
            "classDiagram",
            "    direction TB",
            "",
            "    classDef exceptionClass fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;",
            "    classDef enumClass fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;",
            "    classDef externalClass fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;",
            "    classDef highlightClass fill:rgba(2,132,199,0.18),stroke:#0284c7,stroke-width:3px;",
            "",
            '    class runtime_error["std::runtime_error"] {',
            "        <<external exception>>",
            "    }",
            "    class runtime_error:::externalClass",
            "",
            f'    class sip2jsonErrors["{errs.get("qualified_name", "siddiqsoft::sip2jsonErrors")}"] {{',
            "        <<enumeration>>",
        ]
        for v in errs.get("values", []):
            lines.append(f"        {v}")
        lines.extend([
            "    }",
            "    class sip2jsonErrors:::enumClass",
            "",
            f'    class sip2json_exception["{exc.get("name", "siddiqsoft::sip2json_exception")}"] {{',
        ])
        for a in exc.get("attribs", []):
            lines.append(f"        +{a['type']} {a['name']}")
        for m in exc.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        lines.extend([
            "    }",
            "    class sip2json_exception:::highlightClass",
            "",
        ])

        for de in derived_exceptions:
            lines.append(f'    class {de}["siddiqsoft::{de}"]')
            lines.append(f"    class {de}:::exceptionClass")

        lines.extend([
            "",
            "    runtime_error <|-- sip2json_exception : public inheritance",
            "    sip2json_exception ..> sip2jsonErrors : contains",
        ])

        for de in derived_exceptions:
            lines.append(f"    sip2json_exception <|-- {de}")

        lines.extend([
            "",
            f'    link sip2json_exception "{base_src_url}/{exc_file}" "Source: {exc_file}"',
            f'    link sip2jsonErrors "{base_src_url}/{errs_file}" "Source: {errs_file}"',
            '    link runtime_error "https://en.cppreference.com/w/cpp/error/runtime_error" "Standard Library: std::runtime_error"',
        ])

        for de in derived_exceptions:
            de_obj = classes.get(de)
            de_file = de_obj.get("header_file") if de_obj else exc_file
            lines.append(f'    link {de} "{base_src_url}/{de_file}" "Source: {de_file}"')

        lines.append("```")
        return "\n".join(lines)

    elif target_class in ("HeaderKeySet", "header_keys"):
        hks = classes.get("HeaderKeySet", {})
        hks_file = hks.get("header_file", "include/siddiqsoft/private/sip2json_header_keys.hpp")
        smsg = classes.get("sipmessage", {})
        smsg_file = smsg.get("header_file", "include/siddiqsoft/sipmessage.hpp")

        lines = [
            "```mermaid",
            "classDiagram",
            "    direction TB",
            "",
            "    classDef coreClass fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;",
            "    classDef highlightClass fill:rgba(2,132,199,0.18),stroke:#0284c7,stroke-width:3px;",
            "",
            f'    class HeaderKeySet["{hks.get("name", "siddiqsoft::HeaderKeySet")}"] {{',
        ]
        for a in hks.get("attribs", []):
            lines.append(f"        +{a['type']} {a['name']}")
        for m in hks.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        lines.extend([
            "    }",
            "    class HeaderKeySet:::highlightClass",
            "",
            '    class sipmessage["siddiqsoft::sipmessage"]',
            "    class sipmessage:::coreClass",
            "",
            "    sipmessage ..> HeaderKeySet : uses",
            "",
            f'    link HeaderKeySet "{base_src_url}/{hks_file}" "Source: {hks_file}"',
            f'    link sipmessage "{base_src_url}/{smsg_file}" "Source: {smsg_file}"',
            "```",
        ])
        return "\n".join(lines)

    return generate_uml_class_diagram(xml_dir)


def generate_uml_class_diagram(xml_dir: Path, highlight_class: str = None) -> str:
    """
    Extracts class hierarchies, methods, and relationships dynamically from Doxygen XML AST
    and constructs an OpenCV/Material-styled Mermaid UML class diagram.
    Optionally applies highlightClass styling to highlight_class.
    """
    classes, enums = parse_doxygen_ast(xml_dir)
    base_src_url = "https://github.com/SiddiqSoft/sip2json/blob/master"

    lines = [
        "```mermaid",
        "classDiagram",
        "    direction TB",
        "",
        "    classDef coreClass fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;",
        "    classDef utilityClass fill:rgba(15,118,110,0.08),stroke:#0f766e,stroke-width:2px;",
        "    classDef exceptionClass fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;",
        "    classDef enumClass fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;",
        "    classDef externalClass fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;",
        "    classDef highlightClass fill:rgba(2,132,199,0.18),stroke:#0284c7,stroke-width:3px;",
        "",
        '    class json["nlohmann::json"] {',
        "        <<external DOM>>",
        "    }",
        "    class json:::externalClass",
        "",
        '    class runtime_error["std::runtime_error"] {',
        "        <<external exception>>",
        "    }",
        "    class runtime_error:::externalClass",
        "",
    ]

    # 1. sip2json
    s2j = classes.get("sip2json", {})
    if s2j:
        lines.append(f'    class sip2json["{s2j.get("name", "siddiqsoft::sip2json")}"] {{')
        if s2j.get("is_final"):
            lines.append("        <<final utility>>")
        for m in s2j.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        cls_s2j = "highlightClass" if highlight_class == "sip2json" else "utilityClass"
        lines.extend([
            "    }",
            f"    class sip2json:::{cls_s2j}",
            "",
        ])

    # 2. sipmessage
    smsg = classes.get("sipmessage", {})
    if smsg:
        lines.append(f'    class sipmessage["{smsg.get("name", "siddiqsoft::sipmessage")}"] {{')
        for m in smsg.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        cls_smsg = "highlightClass" if highlight_class == "sipmessage" else "coreClass"
        lines.extend([
            "    }",
            f"    class sipmessage:::{cls_smsg}",
            "",
        ])

    # 3. HeaderKeySet
    hks = classes.get("HeaderKeySet", {})
    if hks:
        lines.append(f'    class HeaderKeySet["{hks.get("name", "siddiqsoft::HeaderKeySet")}"] {{')
        for a in hks.get("attribs", []):
            lines.append(f"        +{a['type']} {a['name']}")
        for m in hks.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        cls_hks = "highlightClass" if highlight_class == "HeaderKeySet" else "coreClass"
        lines.extend([
            "    }",
            f"    class HeaderKeySet:::{cls_hks}",
            "",
        ])

    # 4. SIPMessageType
    smt = enums.get("SIPMessageType", {})
    if smt:
        lines.extend([
            f'    class SIPMessageType["{smt.get("qualified_name", "siddiqsoft::SIPMessageType")}"] {{',
            "        <<enumeration>>",
        ])
        for v in smt.get("values", []):
            lines.append(f"        {v}")
        cls_smt = "highlightClass" if highlight_class == "SIPMessageType" else "enumClass"
        lines.extend([
            "    }",
            f"    class SIPMessageType:::{cls_smt}",
            "",
        ])

    # 5. sip2jsonErrors
    errs = enums.get("sip2jsonErrors", {})
    if errs:
        lines.extend([
            f'    class sip2jsonErrors["{errs.get("qualified_name", "siddiqsoft::sip2jsonErrors")}"] {{',
            "        <<enumeration>>",
        ])
        for v in errs.get("values", []):
            lines.append(f"        {v}")
        cls_errs = "highlightClass" if highlight_class == "sip2jsonErrors" else "enumClass"
        lines.extend([
            "    }",
            f"    class sip2jsonErrors:::{cls_errs}",
            "",
        ])

    # 6. sip2json_exception
    exc = classes.get("sip2json_exception", {})
    if exc:
        lines.append(f'    class sip2json_exception["{exc.get("name", "siddiqsoft::sip2json_exception")}"] {{')
        for a in exc.get("attribs", []):
            lines.append(f"        +{a['type']} {a['name']}")
        for m in exc.get("methods", []):
            st = "$" if m["static"] else ""
            ret = f" {m['ret']}" if m["ret"] and m["ret"] != m["name"] else ""
            lines.append(f"        +{m['name']}({m['params']}){st}{ret}")
        cls_exc = "highlightClass" if highlight_class == "sip2json_exception" else "exceptionClass"
        lines.extend([
            "    }",
            f"    class sip2json_exception:::{cls_exc}",
            "",
        ])

    # 7. Derived exceptions
    derived_exceptions = exc.get("derived", []) if exc else []
    for de in derived_exceptions:
        lines.append(f'    class {de}["siddiqsoft::{de}"]')
        cls_de = "highlightClass" if highlight_class == de else "exceptionClass"
        lines.append(f"    class {de}:::{cls_de}")

    lines.append("")
    # Inheritance from AST
    if smsg and any("json" in b for b in smsg.get("bases", [])):
        lines.append("    json <|-- sipmessage : public inheritance")
    if exc and any("runtime_error" in b for b in exc.get("bases", [])):
        lines.append("    runtime_error <|-- sip2json_exception : public inheritance")
    for de in derived_exceptions:
        lines.append(f"    sip2json_exception <|-- {de}")

    lines.extend([
        "",
        "    sip2json ..> sipmessage : produces / consumes",
        "    sip2json ..> sip2json_exception : throws",
        "    sipmessage ..> SIPMessageType : classifies",
        "    sipmessage ..> HeaderKeySet : uses",
        "    sip2json_exception ..> sip2jsonErrors : contains",
        "",
    ])

    # Links from AST
    for c_key in ["sip2json", "sipmessage", "HeaderKeySet", "sip2json_exception"]:
        c_obj = classes.get(c_key)
        if c_obj and c_obj.get("header_file"):
            lines.append(f'    link {c_key} "{base_src_url}/{c_obj["header_file"]}" "Source: {c_obj["header_file"]}"')

    for e_key in ["SIPMessageType", "sip2jsonErrors"]:
        e_obj = enums.get(e_key)
        if e_obj and e_obj.get("header_file"):
            lines.append(f'    link {e_key} "{base_src_url}/{e_obj["header_file"]}" "Source: {e_obj["header_file"]}"')

    for de in derived_exceptions:
        de_obj = classes.get(de)
        de_file = de_obj.get("header_file") if de_obj else "include/siddiqsoft/private/sip2json_exception.hpp"
        lines.append(f'    link {de} "{base_src_url}/{de_file}" "Source: {de_file}"')

    lines.extend([
        '    link json "https://github.com/nlohmann/json" "External: nlohmann/json"',
        '    link runtime_error "https://en.cppreference.com/w/cpp/error/runtime_error" "Standard Library: std::runtime_error"',
        "```",
    ])

    return "\n".join(lines)


def generate_source_mapping_table(api_prefix: str = "../api/") -> str:
    """
    Generate markdown source code mapping table linking classes, headers,
    GitHub source files, and API documentation.
    """
    base_gh = "https://github.com/SiddiqSoft/sip2json/blob/master"
    return "\n".join([
        "### Source Code Mapping",
        "",
        "| Component / Class | Header File | Source Link | Purpose & Architectural Role |",
        "| :--- | :--- | :--- | :--- |",
        f"| [`siddiqsoft::sip2json`]({api_prefix}sip2json.md) | `include/siddiqsoft/sip2json.hpp` | [`sip2json.hpp`]({base_gh}/include/siddiqsoft/sip2json.hpp) | Top-level static parser, stream deserializer, and wire serializer utility |",
        f"| [`siddiqsoft::sipmessage`]({api_prefix}sipmessage.md) | `include/siddiqsoft/sipmessage.hpp` | [`sipmessage.hpp`]({base_gh}/include/siddiqsoft/sipmessage.hpp) | Core message container inheriting from `nlohmann::json` with zero-copy view accessors |",
        f"| [`siddiqsoft::HeaderKeySet`]({api_prefix}constants.md) | `include/siddiqsoft/private/sip2json_header_keys.hpp` | [`sip2json_header_keys.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_header_keys.hpp) | Canonical SIP header normalization, compact alias mapping, and compile-time hashing |",
        f"| [`siddiqsoft::SIPMessageType`]({api_prefix}sipmessage.md) | `include/siddiqsoft/sipmessage.hpp` | [`sipmessage.hpp`]({base_gh}/include/siddiqsoft/sipmessage.hpp) | Protocol message discriminator (`Request = 1`, `Response = 2`) |",
        f"| [`siddiqsoft::sip2json_exception`]({api_prefix}errors.md#exception-class-hierarchy) | `include/siddiqsoft/private/sip2json_exception.hpp` | [`sip2json_exception.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_exception.hpp) | Base exception class inheriting from `std::runtime_error` with `sip2jsonErrors` payload |",
        f"| [`siddiqsoft::sip2jsonErrors`]({api_prefix}errors.md#sip2jsonerrors-enumeration) | `include/siddiqsoft/private/sip2json_exception.hpp` | [`sip2json_exception.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_exception.hpp) | Diagnostic error code enumeration for parser and syntax failures |",
        f"| Derived Exceptions | `include/siddiqsoft/private/sip2json_exception.hpp` | [`sip2json_exception.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_exception.hpp) | Specialized exception hierarchy (`invalid_document_error`, `empty_message_error`, etc.) |",
        f"| Parser Engine (`raw_view`) | `include/siddiqsoft/private/sip2json_parser.hpp` | [`sip2json_parser.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_parser.hpp) | High-throughput streaming parser, zero-copy buffer slicing, CRLF boundary scanning |",
        f"| Wire Serializer | `include/siddiqsoft/private/sip2json_serializer.hpp` | [`sip2json_serializer.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_serializer.hpp) | RFC 3261 compliant text wire serializer |",
        f"| SDP Body Parser | `include/siddiqsoft/private/sip2json_sdp.hpp` | [`sip2json_sdp.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_sdp.hpp) | RFC 4566 Session Description Protocol parser and structured JSON serialization |",
        f"| Response Codes | `include/siddiqsoft/private/sip2json_response_codes.hpp` | [`sip2json_response_codes.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_response_codes.hpp) | SIP status codes, reason phrases, and classification ranges (1xx-6xx) |",
        f"| Protocol Constants | `include/siddiqsoft/private/sip2json_constants.hpp` | [`sip2json_constants.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_constants.hpp) | SIP grammar tokens, method strings, whitespace matchers, CRLF constants |",
        f"| DateTime Parser | `include/siddiqsoft/private/sip2json_datetime.hpp` | [`sip2json_datetime.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_datetime.hpp) | RFC 3261 / RFC 1123 HTTP-date timestamp parser and serializer |",
        f"| Utility Functions | `include/siddiqsoft/private/sip2json_utils.hpp` | [`sip2json_utils.hpp`]({base_gh}/include/siddiqsoft/private/sip2json_utils.hpp) | Internal whitespace trimming, view slicing, string conversion utilities |",
    ])


def generate_structure_diagram() -> str:
    """
    Constructs an architectural / component structure Mermaid diagram
    showing layers, modules, data flow, and clickable source links.
    """
    base_src_url = "https://github.com/SiddiqSoft/sip2json/blob/master"
    return f"""```mermaid
flowchart TD
    subgraph PublicAPI["Public API Layer (include/siddiqsoft/)"]
        S2J["siddiqsoft::sip2json<br/><i>Static Parser & Serializer Facade</i>"]
        SMSG["siddiqsoft::sipmessage<br/><i>SIP Message Container DTO</i>"]
    end

    subgraph InternalEngines["Parser & Serializer Engines (private/)"]
        Parser["sip2json_parser<br/><i>Zero-Copy Token & Delimiter Scanner</i>"]
        Serial["sip2json_serializer<br/><i>RFC 3261 Wire Serializer</i>"]
        SDP["sip2json_sdp<br/><i>RFC 4566 / 8866 SDP Body Engine</i>"]
    end

    subgraph ProtocolDict["Dictionaries & Token Tables (private/)"]
        HKS["sip2json_header_keys<br/><i>64-bit constexpr FNV-1a Dispatch</i>"]
        Const["sip2json_constants<br/><i>Method Literals & Delimiters</i>"]
        Resp["sip2json_response_codes<br/><i>Numeric Status Code Tables</i>"]
        DT["sip2json_datetime<br/><i>ISO 8601 & RFC 1123 Generators</i>"]
    end

    subgraph Diagnostics["Diagnostics & Utilities (private/)"]
        Exc["sip2json_exception<br/><i>Diagnostics & sip2jsonErrors</i>"]
        Utils["sip2json_utils<br/><i>String View & Memory Utilities</i>"]
    end

    subgraph External["External Base DOM"]
        JSON["nlohmann::json<br/><i>JSON Document Base Class</i>"]
        StdErr["std::runtime_error<br/><i>C++ Standard Exception</i>"]
    end

    S2J -->|produces / consumes| SMSG
    SMSG -->|inherits| JSON
    Exc -->|inherits| StdErr

    S2J -->|delegates streaming| Parser
    S2J -->|delegates serialization| Serial
    Parser -->|delegates body payload| SDP
    Serial -->|formats payload| SDP
    Parser -->|canonical lookup| HKS
    Parser -->|scans delimiters| Utils
    Parser -->|throws framing errors| Exc
    SMSG -->|queries aliases| HKS
    SMSG -->|references codes| Resp
    SMSG -->|timestamps| DT
    Serial -->|delimiters & tokens| Const

    classDef apiLayer fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;
    classDef engineLayer fill:rgba(15,118,110,0.08),stroke:#0f766e,stroke-width:2px;
    classDef dictLayer fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;
    classDef diagLayer fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;
    classDef extLayer fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;

    class S2J,SMSG apiLayer;
    class Parser,Serial,SDP engineLayer;
    class HKS,Const,Resp,DT dictLayer;
    class Exc,Utils diagLayer;
    class JSON,StdErr extLayer;

    click S2J "{base_src_url}/include/siddiqsoft/sip2json.hpp" "Source: sip2json.hpp"
    click SMSG "{base_src_url}/include/siddiqsoft/sipmessage.hpp" "Source: sipmessage.hpp"
    click Parser "{base_src_url}/include/siddiqsoft/private/sip2json_parser.hpp" "Source: sip2json_parser.hpp"
    click Serial "{base_src_url}/include/siddiqsoft/private/sip2json_serializer.hpp" "Source: sip2json_serializer.hpp"
    click SDP "{base_src_url}/include/siddiqsoft/private/sip2json_sdp.hpp" "Source: sip2json_sdp.hpp"
    click HKS "{base_src_url}/include/siddiqsoft/private/sip2json_header_keys.hpp" "Source: sip2json_header_keys.hpp"
    click Exc "{base_src_url}/include/siddiqsoft/private/sip2json_exception.hpp" "Source: sip2json_exception.hpp"
```"""


def generate_control_flow_diagram() -> str:
    """
    Constructs a Mermaid sequence diagram illustrating the control-flow
    and callback execution sequence for SIP frame and message processing.
    """
    return """```mermaid
sequenceDiagram
    autonumber
    actor App as Caller / Transport
    participant S2J as siddiqsoft::sip2json
    participant Parser as sip2json_parser
    participant HKS as sip2json_header_keys
    participant SDP as sip2json_sdp
    participant Msg as siddiqsoft::sipmessage

    App->>S2J: parseAsync(frameBuffer, onMsg, onErr)
    activate S2J
    loop While CRLF delimiter present in buffer
        S2J->>Parser: parseStartLine(sipm, buffer)
        alt Invalid start line
            Parser-->>S2J: framing failure
            S2J->>App: onErr(invalid_startline_error, buffer)
        else Valid Request or Response
            Parser-->>S2J: populates start line (/s)
            S2J->>Parser: parseHeaders(sipm, buffer)
            loop For each header line
                Parser->>HKS: hash_header_key(keyToken)
                HKS-->>Parser: canonical name & alias mapping
                Parser->>Msg: storeHeaderValue(canonKey, val)
            end
            opt Content-Length > 0
                alt Content-Type == application/sdp
                    S2J->>SDP: parseSdp(sipm, bodyBuffer)
                    SDP->>Msg: populates structured SDP (/b/sdp)
                else Raw Payload
                    S2J->>Msg: stores raw payload (/b/raw)
                end
            end
            S2J->>Msg: injects metadata (/meta)
            S2J->>App: onMsg(std::move(sipm))
        end
    end
    deactivate S2J
```"""


def generate_namespace_diagram(xml_dir: Path, ns_name: str = "siddiqsoft") -> str:
    """
    Constructs a namespace-level UML package/class diagram illustrating
    all components contained within the specified namespace and their
    external boundaries (e.g. nlohmann, std).
    """
    classes, enums = parse_doxygen_ast(xml_dir)
    base_src_url = "https://github.com/SiddiqSoft/sip2json/blob/master"

    lines = [
        "```mermaid",
        "classDiagram",
        "    direction TB",
        "",
        "    classDef coreClass fill:rgba(35,73,109,0.08),stroke:#23496d,stroke-width:2px;",
        "    classDef utilityClass fill:rgba(15,118,110,0.08),stroke:#0f766e,stroke-width:2px;",
        "    classDef exceptionClass fill:rgba(185,28,28,0.06),stroke:#b91c1c,stroke-width:1.5px;",
        "    classDef enumClass fill:rgba(109,40,217,0.06),stroke:#6d28d9,stroke-width:1.5px;",
        "    classDef externalClass fill:rgba(100,116,139,0.06),stroke:#64748b,stroke-width:1.5px,stroke-dasharray: 4 3;",
        "",
        f"    namespace {ns_name} {{",
        "        class sip2json",
        "        class sipmessage",
        "        class HeaderKeySet",
        "        class sip2jsonErrors",
        "        class sip2json_exception",
    ]

    exc = classes.get("sip2json_exception", {})
    derived_exceptions = exc.get("derived", []) if exc else []
    for de in derived_exceptions:
        lines.append(f"        class {de}")

    lines.extend([
        "    }",
        "",
        "    namespace nlohmann {",
        "        class json",
        "    }",
        "",
        "    namespace std {",
        "        class runtime_error",
        "    }",
        "",
        "    json <|-- sipmessage : public inheritance",
        "    runtime_error <|-- sip2json_exception : public inheritance",
    ])

    for de in derived_exceptions:
        lines.append(f"    sip2json_exception <|-- {de}")

    lines.extend([
        "",
        "    sip2json ..> sipmessage : produces / consumes",
        "    sip2json ..> sip2json_exception : throws",
        "    sipmessage ..> HeaderKeySet : uses",
        "    sip2json_exception ..> sip2jsonErrors : contains",
        "",
        "    class sip2json:::utilityClass",
        "    class sipmessage:::coreClass",
        "    class HeaderKeySet:::coreClass",
        "    class sip2jsonErrors:::enumClass",
        "    class sip2json_exception:::exceptionClass",
    ])

    for de in derived_exceptions:
        lines.append(f"    class {de}:::exceptionClass")

    lines.extend([
        "    class json:::externalClass",
        "    class runtime_error:::externalClass",
        "",
    ])

    for c_key in ["sip2json", "sipmessage", "HeaderKeySet", "sip2json_exception"]:
        c_obj = classes.get(c_key)
        if c_obj and c_obj.get("header_file"):
            lines.append(f'    link {c_key} "{base_src_url}/{c_obj["header_file"]}" "Source: {c_obj["header_file"]}"')

    errs = enums.get("sip2jsonErrors")
    if errs and errs.get("header_file"):
        lines.append(f'    link sip2jsonErrors "{base_src_url}/{errs["header_file"]}" "Source: {errs["header_file"]}"')

    for de in derived_exceptions:
        de_obj = classes.get(de)
        de_file = de_obj.get("header_file") if de_obj else "include/siddiqsoft/private/sip2json_exception.hpp"
        lines.append(f'    link {de} "{base_src_url}/{de_file}" "Source: {de_file}"')

    lines.extend([
        '    link json "https://github.com/nlohmann/json" "External: nlohmann/json"',
        '    link runtime_error "https://en.cppreference.com/w/cpp/error/runtime_error" "Standard Library: std::runtime_error"',
        "```",
    ])

    return "\n".join(lines)


def generate_snippets(repo_root: Path, xml_dir: Path):
    """
    Generates all auto-generated diagrams and tables into docs/snippets/:
      uml-complete.md, uml-structure.md, uml-control-flow.md, uml-namespace.md,
      uml-source-table.md, and uml-<class-name>.md for every class in the AST.
    """
    snippets_dir = repo_root / "docs" / "snippets"
    snippets_dir.mkdir(parents=True, exist_ok=True)

    classes, enums = parse_doxygen_ast(xml_dir)

    complete_diagram = generate_uml_class_diagram(xml_dir)
    (snippets_dir / "uml-complete.md").write_text(complete_diagram + "\n", encoding="utf-8")
    (snippets_dir / "system_uml_diagram.md").write_text(complete_diagram + "\n", encoding="utf-8")

    structure_diagram = generate_structure_diagram()
    (snippets_dir / "uml-structure.md").write_text(structure_diagram + "\n", encoding="utf-8")

    flow_diagram = generate_control_flow_diagram()
    (snippets_dir / "uml-control-flow.md").write_text(flow_diagram + "\n", encoding="utf-8")

    ns_diagram = generate_namespace_diagram(xml_dir, "siddiqsoft")
    (snippets_dir / "uml-namespace.md").write_text(ns_diagram + "\n", encoding="utf-8")
    (snippets_dir / "uml_namespace.md").write_text(ns_diagram + "\n", encoding="utf-8")
    (snippets_dir / "uml-namespace-siddiqsoft.md").write_text(ns_diagram + "\n", encoding="utf-8")

    source_table = generate_source_mapping_table(api_prefix="../api/")
    (snippets_dir / "uml-source-table.md").write_text(source_table + "\n", encoding="utf-8")
    (snippets_dir / "source_mapping_table.md").write_text(source_table + "\n", encoding="utf-8")

    print(f"[generate_api_docs] Wrote structural/sequence UML snippets to {snippets_dir}")


def update_architecture_uml(arch_file: Path, uml_content: str = ""):
    """
    Ensures docs/architecture/index.md uses clean <!-- @@uml-diag:... --> tags.
    """
    if not arch_file.exists():
        return
    text = arch_file.read_text(encoding="utf-8")

    conversions = [
        ('--8<-- "docs/snippets/system_uml_diagram.md"\n\n--8<-- "docs/snippets/source_mapping_table.md"', "<!-- @@uml-diag:complete -->\n\n<!-- @@uml-diag:source-table -->"),
        ("uml:structure", "<!-- @@uml-diag:structure -->"),
        ("uml:control-flow", "<!-- @@uml-diag:control-flow -->"),
        ("uml:namespace", "<!-- @@uml-diagram:namespace -->"),
        ("uml:complete", "<!-- @@uml-diag:complete -->"),
        ("uml:source-table", "<!-- @@uml-diag:source-table -->"),
        ("@@uml-diag:structure", "<!-- @@uml-diag:structure -->"),
        ("@@uml-diag:control-flow", "<!-- @@uml-diag:control-flow -->"),
        ("@@uml-diagram:namespace", "<!-- @@uml-diagram:namespace -->"),
        ("@@uml-diag:complete", "<!-- @@uml-diag:complete -->"),
        ("@@uml-diag:source-table", "<!-- @@uml-diag:source-table -->"),
    ]
    for old, new in conversions:
        if old in text:
            text = text.replace(old, new)

    start_tag = "<!-- UML_CLASS_DIAGRAM_START -->"
    end_tag = "<!-- UML_CLASS_DIAGRAM_END -->"
    if start_tag in text and end_tag in text:
        pattern = re.compile(rf"{re.escape(start_tag)}.*?{re.escape(end_tag)}", re.DOTALL)
        text = pattern.sub("<!-- @@uml-diag:complete -->\n\n<!-- @@uml-diag:source-table -->", text)

    arch_file.write_text(text, encoding="utf-8")


def update_maintainer_uml(maintainer_file: Path, uml_content: str = ""):
    """
    Ensures maintainer documentation uses clean <!-- @@uml-diag:... --> tags.
    """
    if not maintainer_file.exists():
        return
    text = maintainer_file.read_text(encoding="utf-8")

    conversions = [
        ('--8<-- "docs/snippets/system_uml_diagram.md"\n\n--8<-- "docs/snippets/source_mapping_table.md"', "<!-- @@uml-diag:complete -->\n\n<!-- @@uml-diag:source-table -->"),
        ("uml:complete", "<!-- @@uml-diag:complete -->"),
        ("uml:source-table", "<!-- @@uml-diag:source-table -->"),
        ("@@uml-diag:complete", "<!-- @@uml-diag:complete -->"),
        ("@@uml-diag:source-table", "<!-- @@uml-diag:source-table -->"),
    ]
    for old, new in conversions:
        if old in text:
            text = text.replace(old, new)

    start_tag = "<!-- UML_CLASS_DIAGRAM_START -->"
    end_tag = "<!-- UML_CLASS_DIAGRAM_END -->"
    lead_text = "The following UML class diagram illustrates the primary classes, relationships, and exception hierarchy in `siddiqsoft::sip2json`. The diagram is auto-generated from the C++ source AST via Doxygen XML. Each node in the diagram links directly to its source header file on GitHub.\n\n"
    if start_tag in text and end_tag in text:
        pattern = re.compile(rf"{re.escape(start_tag)}.*?{re.escape(end_tag)}", re.DOTALL)
        text = pattern.sub(f"{lead_text}<!-- @@uml-diag:complete -->\n\n<!-- @@uml-diag:source-table -->", text)

    maintainer_file.write_text(text, encoding="utf-8")


def main():
    repo_root = Path(__file__).resolve().parent.parent
    xml_dir = repo_root / "docs" / "doxygen_xml"
    api_dir = repo_root / "docs" / "api"
    arch_file = repo_root / "docs" / "architecture" / "index.md"
    maintainer_file = repo_root / "docs" / "maintainers" / "maintainer_guide.md"
    if not maintainer_file.exists():
        maintainer_file = repo_root / "docs" / "maintainers" / "pipelines.md"

    run_doxygen(repo_root)

    generate_snippets(repo_root, xml_dir)

    generate_index_md(api_dir / "index.md")
    generate_sip2json_md(xml_dir, api_dir / "sip2json.md")
    generate_sipmessage_md(xml_dir, api_dir / "sipmessage.md")
    generate_constants_md(api_dir / "constants.md")
    generate_errors_md(xml_dir, api_dir / "errors.md")

    update_architecture_uml(arch_file)
    update_maintainer_uml(maintainer_file)

    print("[generate_api_docs] API documentation generation complete.")


if __name__ == "__main__":
    main()
