# Documentation & MkDocs System

`sip2json` uses [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) with dynamic Python hooks for generating, validating, and publishing developer and user documentation.

---

## 1. Local Python Environment Setup

Always create a dedicated Python virtual environment to manage documentation dependencies:

```bash
# 1. Create Python virtual environment
python3 -m venv venv

# 2. Activate virtual environment
# On macOS / Linux:
source venv/bin/activate
# On Windows PowerShell:
# .\venv\Scripts\Activate.ps1

# 3. Upgrade pip and install documentation requirements
pip install --upgrade pip
pip install -r docs/requirements.txt ghp-import
```

`docs/requirements.txt` contains:
```text
mkdocs>=1.6.0
mkdocs-material>=9.5.0
pymdown-extensions>=10.0
```

---

## 2. Running the Live Preview Server (`mkdocs serve`)

Start the local development server with live reload enabled:

```bash
mkdocs serve
```

Open **`http://127.0.0.1:8000`** in your browser. Any edits made to files in `docs/` will automatically recompile and refresh the page instantly.

---

## 3. Strict Validation Build (`mkdocs build --strict`)

Before committing changes or opening a PR, always execute a strict build to verify that there are no broken links, malformed markdown syntax, or missing navigation items:

```bash
mkdocs build --strict
```

If any link is broken or any file listed in `mkdocs.yml` is missing, `--strict` mode will exit with an error code and display the exact line number.

---

## 4. How the Dynamic Build Hook (`docs/hooks.py`) Works

`mkdocs.yml` configures a pre-build lifecycle hook:

```yaml
hooks:
  - docs/hooks.py
```

When `mkdocs build` or `mkdocs serve` starts, `docs/hooks.py` performs three automated tasks:

1. **Dynamic Version Resolution (`on_config`)**:
   - Executes `git describe --tags --always --dirty` or queries GitVersion environment variables (`GitVersion_SemVer`).
   - Injects the resolved version string into `config['extra']['version']`, dynamically populating all `{ version }` and `{{ version }}` template variables across pages.

2. **Automated Dependency Generation**:
   - Executes `scripts/generate_dependencies_md.py --root .` to inspect `CMakeLists.txt` and generate Mermaid dependency diagrams and Markdown tables (`docs/integration/dependencies.md`).

3. **Dynamic Benchmark Matrix Injection**:
   - Executes `scripts/publish_benchmarks.py --root . --skip-build --skip-exec` to inject live multi-platform benchmark tables into `docs/features/benchmarks.md` between `<!-- PIPELINE_BENCHMARKS_START -->` and `<!-- PIPELINE_BENCHMARKS_END -->`.

---

## 5. Manual Documentation Deployment (`gh-pages`)

While documentation publication is automated via Azure Pipelines CI/CD, maintainers with repository push permissions can manually deploy documentation to GitHub Pages using `ghp-import`:

```bash
# 1. Activate venv and build site
source venv/bin/activate
mkdocs build --strict

# 2. Deploy site directory to gh-pages branch
ghp-import -n -p -f site -b gh-pages -m "Deploy documentation [manual release]"
```
