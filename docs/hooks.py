import os
import sys
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
        print(f"[docs/hooks.py] Warning: Failed to generate dependencies.md: {e}")

    # 4. Automatically execute benchmarks and publish reports during site build
    try:
        root_dir = Path(__file__).resolve().parent.parent
        bench_script = root_dir / "scripts" / "publish_benchmarks.py"
        if bench_script.exists():
            subprocess.run([sys.executable, str(bench_script), "--root", str(root_dir), "--skip-build", "--skip-exec"], check=True)
    except Exception as e:
        print(f"[docs/hooks.py] Warning: Failed to generate benchmark reports during publication: {e}")

    # 5. Inject version into MkDocs extra configuration
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

