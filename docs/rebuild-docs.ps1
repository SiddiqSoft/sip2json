$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir

Write-Host "Regenerating API documentation from Doxygen XML..."
python "$RootDir\scripts\generate_api_docs.py"

# Check for mkdocs installation (system PATH or project venv)
$mkdocsCmd = Get-Command mkdocs -ErrorAction SilentlyContinue
if (-not $mkdocsCmd -and (Test-Path "$RootDir\venv\Scripts\mkdocs.exe")) {
    $mkdocsCmd = "$RootDir\venv\Scripts\mkdocs.exe"
}

if (-not $mkdocsCmd) {
    Write-Host "mkdocs is not installed; skipping MkDocs site build."
    exit 0
}

Write-Host "Building MkDocs Material site..."
& $mkdocsCmd build --config-file "$RootDir\mkdocs.yml"
