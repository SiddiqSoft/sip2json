import os
import sys
import re
import subprocess
from pathlib import Path

def on_config(config, **kwargs):
    """
    MkDocs hook that dynamically injects the build version into the site configuration (config['extra']['version']).
    It resolves the version from CI environment variables (GitVersion / Azure Pipelines / GitHub Actions),
    or falls back to git describe / local default.
    """
    # 1. Resolve version from CI/GitVersion environment variables
    version = (
        os.getenv("GITVERSION_SEMVER")
        or os.getenv("GITVERSION_MAJORMINORPATCH")
        or os.getenv("BUILD_VERSION")
        or os.getenv("BUILD_BUILDNUMBER")
        or os.getenv("CI_BUILDID")
    )
    
    # 2. If not running in CI or env vars are missing, try checking GitVersion.yml next-version or git describe
    if not version:
        try:
            root_dir = Path(__file__).resolve().parent.parent
            gv_file = root_dir / "GitVersion.yml"
            if gv_file.exists():
                for line in gv_file.read_text().splitlines():
                    if line.startswith("next-version:"):
                        version = line.split(":", 1)[1].strip()
                        break
        except Exception:
            pass

    if not version:
        try:
            version = subprocess.check_output(
                ["git", "describe", "--tags", "--always", "--dirty"],
                stderr=subprocess.DEVNULL,
                text=True
            ).strip()
        except Exception:
            version = "0.0.0-dev"

    # 3. Automatically regenerate dependencies documentation from CMakeLists.txt
    try:
        root_dir = Path(__file__).resolve().parent.parent
        script_path = root_dir / "scripts" / "generate_dependencies_md.py"
        if script_path.exists():
            subprocess.run([sys.executable, str(script_path), "--root", str(root_dir)], check=True)
    except Exception as e:
        print(f"[docs/hooks.py] Warning: Failed to generate dependencies documentation: {e}")

    # 4. Automatically regenerate API documentation via Doxygen XML
    try:
        root_dir = Path(__file__).resolve().parent.parent
        api_script = root_dir / "scripts" / "generate_api_docs.py"
        if api_script.exists():
            subprocess.run([sys.executable, str(api_script)], check=True)
    except Exception as e:
        print(f"[docs/hooks.py] Warning: Failed to generate API documentation from Doxygen XML: {e}")

    # 5. Automatically execute benchmarks and publish reports during site build
    try:
        root_dir = Path(__file__).resolve().parent.parent
        bench_script = root_dir / "scripts" / "publish_benchmarks.py"
        if bench_script.exists():
            subprocess.run([sys.executable, str(bench_script), "--root", str(root_dir), "--skip-build", "--skip-exec"], check=True)
    except Exception as e:
        print(f"[docs/hooks.py] Warning: Failed to generate benchmark reports during publication: {e}")

    # 6. Inject version into MkDocs extra configuration
    if "extra" not in config or config["extra"] is None:
        config["extra"] = {}
    config["extra"]["version"] = version

    print(f"[docs/hooks.py] Resolved build version: {version}")
    return config

def on_page_markdown(markdown, page, config, files):
    """
    Replaces dynamic version placeholders in markdown documentation pages at publication time.
    Supported placeholders:
      - {{ version }} / { version }: Full build version (e.g. 2.6.0-47-g6992bc7)
      - {{ tag_version }} / { tag_version }: Clean semver tag version (e.g. v2.6.0)
    """
    version = config.get("extra", {}).get("version", "0.0.0-dev")
    tag_ver = version.split("-")[0] if "-" in version else version
    semver  = tag_ver[1:] if tag_ver.startswith("v") else tag_ver
    if not tag_ver.startswith("v"):
        tag_ver = f"v{tag_ver}"

    placeholders = {
        "{{ version }}": version,
        "{ version }": version,
        "{{version}}": version,
        "{version}": version,
        "{{ tag_version }}": tag_ver,
        "{ tag_version }": tag_ver,
        "{{tag_version}}": tag_ver,
        "{tag_version}": tag_ver,
        "{{ semver }}": semver,
        "{ semver }": semver,
        "{{semver}}": semver,
        "{semver}": semver,
    }

    for ph, val in placeholders.items():
        if ph in markdown:
            markdown = markdown.replace(ph, val)

    # Dynamically expand uml:... diagram references
    root_dir = Path(__file__).resolve().parent.parent
    markdown = replace_uml_tags(markdown, root_dir)

    return markdown


class DoxygenGraphvizCatalog:
    """
    Catalog that dynamically resolves C++ symbols from Doxygen XML (docs/doxygen_xml/index.xml)
    and maps them to GraphViz SVG diagrams generated in docs/doxygen_html/.
    Handles name changes, filename moves, and class/struct/enum updates dynamically.
    """
    def __init__(self, root_dir: Path):
        self.root_dir = root_dir
        self.xml_dir = root_dir / "docs" / "doxygen_xml"
        self.html_dir = root_dir / "docs" / "doxygen_html"
        self.snippets_dir = root_dir / "docs" / "snippets"
        self.compounds = {}
        self.members = {}
        self._loaded_mtime = None

    def _ensure_loaded(self):
        index_file = self.xml_dir / "index.xml"
        if not index_file.exists():
            return
        current_mtime = index_file.stat().st_mtime
        if self._loaded_mtime == current_mtime:
            return

        self.compounds.clear()
        self.members.clear()
        try:
            import xml.etree.ElementTree as ET
            tree = ET.parse(index_file)
            root = tree.getroot()
            for comp in root.findall("compound"):
                kind = comp.attrib.get("kind", "")
                refid = comp.attrib.get("refid", "")
                name = comp.findtext("name", "").strip()
                short_name = name.split("::")[-1]
                info = {"refid": refid, "kind": kind, "name": name, "short_name": short_name}

                # Primary mappings
                for key in (name.lower(), short_name.lower(),
                            name.lower().replace("_", ""), short_name.lower().replace("_", "")):
                    self.compounds[key] = info

                # Member elements (enums, typedefs)
                for m in comp.findall("member"):
                    m_kind = m.attrib.get("kind", "")
                    if m_kind in ("enum", "typedef"):
                        m_refid = m.attrib.get("refid", "")
                        m_name = m.findtext("name", "").strip()
                        m_short = m_name.split("::")[-1]
                        minfo = {
                            "refid": m_refid,
                            "kind": m_kind,
                            "name": m_name,
                            "short_name": m_short,
                            "parent_refid": refid,
                            "parent_name": name,
                        }
                        for key in (m_name.lower(), m_short.lower(),
                                    m_name.lower().replace("_", ""), m_short.lower().replace("_", "")):
                            self.members[key] = minfo

            self._loaded_mtime = current_mtime
        except Exception as e:
            print(f"[docs/hooks.py] Warning: Error indexing Doxygen XML: {e}")

    def clean_svg(self, raw_svg: str) -> str:
        """Strips XML declaration and sets responsive styling on root SVG element."""
        cleaned = re.sub(r"<\?xml[^>]*\?>\s*", "", raw_svg)
        cleaned = re.sub(r"<!DOCTYPE[^>]*>\s*", "", cleaned)

        def _adjust_svg_tag(match):
            tag = match.group(0)
            tag = re.sub(r'\s+width="[^"]*"', '', tag)
            tag = re.sub(r'\s+height="[^"]*"', '', tag)
            if 'class=' not in tag:
                tag = tag.replace('<svg', '<svg class="graphviz-uml-svg" style="max-width: 100%; height: auto;"')
            else:
                tag = re.sub(r'class="([^"]*)"', r'class="\1 graphviz-uml-svg" style="max-width: 100%; height: auto;"', tag)
            return tag

        return re.sub(r"<svg\b[^>]*>", _adjust_svg_tag, cleaned, count=1).strip()

    def render_enum_svg(self, enum_name: str, parent_refid: str) -> str:
        """Synthesizes an authentic GraphViz UML enumeration diagram via dot -Tsvg."""
        cache_file = self.html_dir / f"enum_{parent_refid}_{enum_name}.svg"
        if cache_file.exists():
            return self.clean_svg(cache_file.read_text(encoding="utf-8"))

        parent_xml = self.xml_dir / f"{parent_refid}.xml"
        qname = enum_name
        values = []
        if parent_xml.exists():
            try:
                import xml.etree.ElementTree as ET
                tree = ET.parse(parent_xml)
                for mem in tree.iter("memberdef"):
                    if mem.attrib.get("kind") == "enum" and mem.findtext("name", "") == enum_name:
                        qname = mem.findtext("qualifiedname", "") or enum_name
                        for ev in mem.findall("enumvalue"):
                            vname = ev.findtext("name", "")
                            if vname:
                                values.append(vname)
                        break
            except Exception:
                pass

        val_lines = r"\l+ ".join(values) + r"\l" if values else ""
        label = rf"{{\<\<enumeration\>\>\n{qname}|+ {val_lines}}}" if val_lines else rf"{{\<\<enumeration\>\>\n{qname}}}"
        dot_script = f"""digraph "{enum_name}" {{
  rankdir=TB;
  node [fontname="Helvetica,sans-Serif", fontsize="10", shape=record, fillcolor="#f8fafc", style="filled", color="#64748b"];
  "{enum_name}" [label="{label}"];
}}"""
        try:
            res = subprocess.run(["dot", "-Tsvg"], input=dot_script, text=True, capture_output=True, check=False)
            if res.returncode == 0 and res.stdout:
                cache_file.write_text(res.stdout, encoding="utf-8")
                return self.clean_svg(res.stdout)
        except Exception:
            pass
        return None

    def wrap_svg(self, svg_content: str, diagram_id: str, graph_type: str, display_name: str) -> str:
        return f"""<div class="uml-diagram-container graphviz-uml" data-diagram="{diagram_id}" data-graph-type="{graph_type}">
  <figure class="uml-diagram-figure">
    <div class="uml-diagram-viewport">
      {svg_content}
    </div>
    <figcaption>Figure: GraphViz UML {graph_type} diagram for <code>{display_name}</code></figcaption>
  </figure>
</div>"""

    def resolve(self, raw_target: str) -> str:
        self._ensure_loaded()
        target = raw_target.strip()

        # 1. Extract aspect qualifier if present
        aspect = None
        for a_token, a_val in [
            ("inheritance:", "inherit"),
            ("inherit:", "inherit"),
            ("collaboration:", "coll"),
            ("coll:", "coll"),
            ("usage:", "coll"),
            ("includes:", "incl"),
            ("include:", "incl"),
            ("incl:", "incl"),
            ("included-by:", "dep_incl"),
            ("dep-incl:", "dep_incl"),
            ("dep:", "dep_incl"),
        ]:
            if target.startswith(a_token):
                aspect = a_val
                target = target[len(a_token):].strip()
                break

        # 2. Extract entity kind qualifier if present
        for k_token in ("class:", "struct:", "enum:", "file:", "namespace:", "ns:", "dir:"):
            if target.startswith(k_token):
                target = target[len(k_token):].strip()
                break

        # 3. Special architectural view fallbacks
        if target in ("complete", "system", "classDiagram", "all"):
            for c in ("uml-complete.md", "system_uml_diagram.md"):
                p = self.snippets_dir / c
                if p.exists():
                    return p.read_text(encoding="utf-8").strip()

        if target in ("structure", "topology", "subsystem"):
            p = self.snippets_dir / "uml-structure.md"
            if p.exists():
                return p.read_text(encoding="utf-8").strip()

        if target in ("control-flow", "control_flow", "flow", "sequence"):
            p = self.snippets_dir / "uml-control-flow.md"
            if p.exists():
                return p.read_text(encoding="utf-8").strip()

        if target in ("source-table", "source_table", "sources", "mapping-table", "mapping_table", "mapping"):
            for c in ("uml-source-table.md", "source_mapping_table.md"):
                p = self.snippets_dir / c
                if p.exists():
                    return p.read_text(encoding="utf-8").strip()

        if target in ("namespace", "ns", "packages"):
            for c in ("uml-namespace.md", "uml_namespace.md", "uml-namespace-siddiqsoft.md"):
                p = self.snippets_dir / c
                if p.exists():
                    return p.read_text(encoding="utf-8").strip()

        if target in ("errors", "exceptions"):
            # Map directly to sip2json_exception
            target = "sip2json_exception"

        # 4. Lookup entity in AST compounds or members
        norm_key = target.lower()
        norm_key_clean = norm_key.replace("_", "")
        ent = (self.compounds.get(norm_key)
               or self.compounds.get(norm_key_clean)
               or self.members.get(norm_key)
               or self.members.get(norm_key_clean))

        if ent:
            kind = ent["kind"]
            refid = ent["refid"]
            display_name = ent["name"]

            # Class or Struct resolution
            if kind in ("class", "struct"):
                inherit_svg_p = self.html_dir / f"{refid}__inherit__graph.svg"
                coll_svg_p = self.html_dir / f"{refid}__coll__graph.svg"

                has_inherit = inherit_svg_p.exists()
                has_coll = coll_svg_p.exists()

                if aspect == "inherit" and has_inherit:
                    raw = inherit_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "inheritance", display_name)

                if aspect == "coll" and has_coll:
                    raw = coll_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "collaboration", display_name)

                # Both exist and no aspect preference -> render native Material tabs
                if has_inherit and has_coll:
                    raw_inh = inherit_svg_p.read_text(encoding="utf-8")
                    raw_col = coll_svg_p.read_text(encoding="utf-8")
                    wrap_inh = self.wrap_svg(self.clean_svg(raw_inh), target, "inheritance", display_name)
                    wrap_col = self.wrap_svg(self.clean_svg(raw_col), target, "collaboration", display_name)
                    wrap_inh_ind = "\n".join("    " + l if l.strip() else "" for l in wrap_inh.splitlines())
                    wrap_col_ind = "\n".join("    " + l if l.strip() else "" for l in wrap_col.splitlines())
                    return f'=== "Inheritance Diagram"\n\n{wrap_inh_ind}\n\n=== "Collaboration Diagram"\n\n{wrap_col_ind}'

                if has_inherit:
                    raw = inherit_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "inheritance", display_name)

                if has_coll:
                    raw = coll_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "collaboration", display_name)

            # File resolution
            if kind == "file":
                incl_svg_p = self.html_dir / f"{refid}__incl.svg"
                dep_svg_p = self.html_dir / f"{refid}__dep__incl.svg"
                has_incl = incl_svg_p.exists()
                has_dep = dep_svg_p.exists()

                if aspect == "incl" and has_incl:
                    raw = incl_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "includes", display_name)
                if aspect == "dep_incl" and has_dep:
                    raw = dep_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "dependency", display_name)

                if has_incl and has_dep:
                    raw_i = incl_svg_p.read_text(encoding="utf-8")
                    raw_d = dep_svg_p.read_text(encoding="utf-8")
                    wrap_i = self.wrap_svg(self.clean_svg(raw_i), target, "includes", display_name)
                    wrap_d = self.wrap_svg(self.clean_svg(raw_d), target, "dependency", display_name)
                    wrap_i_ind = "\n".join("    " + l if l.strip() else "" for l in wrap_i.splitlines())
                    wrap_d_ind = "\n".join("    " + l if l.strip() else "" for l in wrap_d.splitlines())
                    return f'=== "Include Dependencies"\n\n{wrap_i_ind}\n\n=== "Included By (Dependents)"\n\n{wrap_d_ind}'

                if has_incl:
                    raw = incl_svg_p.read_text(encoding="utf-8")
                    return self.wrap_svg(self.clean_svg(raw), target, "includes", display_name)

            # Enum resolution
            if kind == "enum":
                parent_ref = ent.get("parent_refid", "")
                enum_svg = self.render_enum_svg(ent["short_name"], parent_ref)
                if enum_svg:
                    return self.wrap_svg(enum_svg, target, "enumeration", display_name)

        # 5. File candidates in snippets directory fallback
        candidates = [
            f"uml-{target}.md",
            f"uml_{target}.md",
            f"uml-{target.replace('_', '-')}.md",
            f"uml_{target.replace('-', '_')}.md",
            f"{target}.md",
            f"system_{target}.md",
        ]
        for c in candidates:
            p = self.snippets_dir / c
            if p.exists():
                return p.read_text(encoding="utf-8").strip()

        return None


# Global singleton catalog instance
_DOXYGEN_CATALOG = None

def get_doxygen_catalog(root_dir: Path) -> DoxygenGraphvizCatalog:
    global _DOXYGEN_CATALOG
    if _DOXYGEN_CATALOG is None:
        _DOXYGEN_CATALOG = DoxygenGraphvizCatalog(root_dir)
    return _DOXYGEN_CATALOG


def replace_uml_tags(markdown: str, root_dir: Path) -> str:
    """
    Dynamically expands UML diagram references in markdown files:
      <!-- @@uml-diag:complete -->
      <!-- @@uml-diag:<class-name> -->  (e.g., sip2json, sipmessage, HeaderKeySet)
      <!-- @@uml-diag:inheritance:<class-name> -->
      <!-- @@uml-diag:collaboration:<class-name> -->
      <!-- @@uml-diag:file:<file-name> -->
      <!-- @@uml-diag:enum:<enum-name> -->
      <!-- @@uml-diagram:namespace -->
      <!-- @@uml-diag:structure -->
      <!-- @@uml-diag:control-flow -->
      <!-- @@uml-diag:source-table -->
      {{ @@uml-diag:... }}
    Uses Doxygen + GraphViz output directly and shields code blocks / backticks.
    """
    catalog = get_doxygen_catalog(root_dir)

    # 1. Shield code blocks and inline backticks so markdown tables and syntax examples are preserved
    rendered_blocks = {}
    def _store_rendered(target: str, inline: bool = False) -> str:
        content = catalog.resolve(target)
        if content is None:
            return None
        key = f"__UML_RENDERED_{len(rendered_blocks)}__"
        rendered_blocks[key] = content if inline else f"\n\n{content}\n\n"
        return key

    # 1. Shield code blocks and inline backticks so markdown tables and syntax examples are preserved
    code_stash = {}
    def _stash_code(m):
        key = f"__UML_CODE_STASH_{len(code_stash)}__"
        code_stash[key] = m.group(0)
        return key

    # Stash triple backticks first, then inline backticks
    markdown = re.sub(r"```[\s\S]*?```", _stash_code, markdown)
    markdown = re.sub(r"`[^`\n]+`", _stash_code, markdown)

    def _replace_block(m):
        tag_name = m.group(1)
        key = _store_rendered(tag_name, inline=False)
        return key if key else m.group(0)

    def _replace_inline(m):
        tag_name = m.group(1)
        key = _store_rendered(tag_name, inline=True)
        return key if key else m.group(0)

    # 2. Standard HTML comment blocks (100% W3C HTML5 & CommonMark compliant)
    html_comment_line = re.compile(
        r"(?m)^[ \t]*<!--[ \t]*(?:@@uml-diagram:|@@uml-diag:|@@uml:|uml:)[ \t]*([a-zA-Z0-9_\-:]+)[ \t]*-->[ \t]*$"
    )
    markdown = html_comment_line.sub(_replace_block, markdown)

    # 3. Markdown invisible link comments: [//]: # (@@uml-diagram:complete)
    md_comment_line = re.compile(
        r'(?m)^[ \t]*\[//\]:[ \t]*#[ \t]*\([ \t]*(?:@@uml-diagram:|@@uml-diag:|@@uml:|uml:)[ \t]*([a-zA-Z0-9_\-:]+)[ \t]*\)[ \t]*$'
    )
    markdown = md_comment_line.sub(_replace_block, markdown)

    # 4. Custom XML/HTML elements: <uml-diagram target="sipmessage" />
    html_tag_line = re.compile(
        r'(?m)^[ \t]*<uml-diagram[^>]*?target=["\'](?:@@uml-diagram:|@@uml-diag:|@@uml:|uml:)?([a-zA-Z0-9_\-:]+)["\'][^>]*?(?:/>|>(?:[ \t]*</uml-diagram>)?)[ \t]*$'
    )
    markdown = html_tag_line.sub(_replace_block, markdown)

    # 5. Standalone bare lines or bracketed lines: @@uml-diagram:complete, {{ @@uml-diagram:complete }}, [@@uml-diagram:complete]
    bare_line = re.compile(
        r"(?m)^[ \t]*(?:\{\{[ \t]*|\[[ \t]*)?(?:@@uml-diagram:|@@uml-diag:|@@uml:|uml:)[ \t]*([a-zA-Z0-9_\-:]+)(?:[ \t]*\}\}|[ \t]*\])?[ \t]*$"
    )
    markdown = bare_line.sub(_replace_block, markdown)

    # 6. Inline HTML comments: <!-- @@uml-diagram:tag --> anywhere on a line
    inline_html_comment = re.compile(
        r"<!--[ \t]*(?:@@uml-diagram:|@@uml-diag:|@@uml:|uml:)[ \t]*([a-zA-Z0-9_\-:]+)[ \t]*-->"
    )
    markdown = inline_html_comment.sub(_replace_inline, markdown)

    # 7. Inline template braces: {{ @@uml-diagram:tag }}
    inline_template = re.compile(
        r"\{\{\s*(?:@@uml-diagram:|@@uml-diag:|@@uml:|uml:)\s*([a-zA-Z0-9_\-:]+)\s*\}\}"
    )
    markdown = inline_template.sub(_replace_inline, markdown)

    # 8. Restore rendered blocks (guarantees no re-matching of generated HTML/SVG)
    for key, val in rendered_blocks.items():
        markdown = markdown.replace(key, val)

    # 9. Restore stashed code blocks
    for key, val in code_stash.items():
        markdown = markdown.replace(key, val)

    return markdown


def fold_filepath_code(inner: str) -> str:
    """
    Folds a filepath code block on its right-most '/' separator so that when
    a column or container is narrow, it wraps cleanly as:
      dir_path/
      filename.ext
    Both segments have white-space: nowrap to prevent breaking mid-token.
    """
    if "filepath-dir" in inner or "filepath-name" in inner:
        return inner
    if "/" not in inner or inner.endswith("/"):
        return inner
    # Avoid JSON pointers, mime types, protocol versions
    if inner.startswith(("/", "\"", "SIP/")):
        return inner

    # Handle #include <path/to/header.hpp>
    if inner.startswith("#include") and "/" in inner:
        r_idx = inner.rfind("/")
        dir_part = inner[:r_idx + 1]
        name_part = inner[r_idx + 1:]
        if dir_part and name_part:
            return f'<span class="filepath-dir">{dir_part}</span><wbr><span class="filepath-name">{name_part}</span>'

    r_idx = inner.rfind("/")
    dir_part = inner[:r_idx + 1]
    name_part = inner[r_idx + 1:]
    if not name_part or not dir_part:
        return inner

    # Must have a file extension or multiple directory levels
    has_ext = bool(re.search(r"\.[a-zA-Z0-9_-]+>?$", name_part))
    has_multiple_dirs = dir_part.count("/") >= 1 and bool(re.search(r"[a-zA-Z0-9_.-]+/[a-zA-Z0-9_.-]+", inner))
    if not (has_ext or (has_multiple_dirs and len(inner) > 18)):
        return inner

    return f'<span class="filepath-dir">{dir_part}</span><wbr><span class="filepath-name">{name_part}</span>'

def on_page_content(html, page, config, files):
    """
    Post-processes page HTML to apply filepath folding rules:
    Code blocks containing directory/file paths fold on the right-most '/'
    when rendered in constrained columns or tables.
    """
    def _replace_code(match):
        inner = match.group(1)
        folded = fold_filepath_code(inner)
        return f"<code>{folded}</code>"

    # Match inline <code>...</code> blocks (avoiding multi-line code blocks)
    return re.sub(r"<code>([^<>\n]+)</code>", _replace_code, html)

