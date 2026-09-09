#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "Regenerating API documentation from Doxygen XML..."
python3 "$ROOT_DIR/scripts/generate_api_docs.py"

# Check for mkdocs installation (system PATH or project venv)
MKDOCS_CMD=""
if command -v mkdocs &> /dev/null; then
  MKDOCS_CMD="mkdocs"
elif [ -x "$ROOT_DIR/venv/bin/mkdocs" ]; then
  MKDOCS_CMD="$ROOT_DIR/venv/bin/mkdocs"
fi

if [ -z "$MKDOCS_CMD" ]; then
  echo "mkdocs is not installed; skipping MkDocs site build."
  exit 0
fi

echo "Building MkDocs Material site using $MKDOCS_CMD..."
"$MKDOCS_CMD" build --config-file "$ROOT_DIR/mkdocs.yml"
