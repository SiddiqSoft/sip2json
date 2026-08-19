#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "Building MkDocs Material site..."
mkdocs build --config-file "$ROOT_DIR/mkdocs.yml"
