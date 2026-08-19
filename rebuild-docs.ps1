$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir

Write-Host "Building MkDocs Material site..."
mkdocs build --config-file "$RootDir\mkdocs.yml"
