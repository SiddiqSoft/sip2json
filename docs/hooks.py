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
    }

    for ph, val in placeholders.items():
        if ph in markdown:
            markdown = markdown.replace(ph, val)

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

