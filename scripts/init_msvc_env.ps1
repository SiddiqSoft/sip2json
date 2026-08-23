<#
.SYNOPSIS
    Initializes the MSVC compiler environment (cl.exe, link.exe, CMAKE) in a PowerShell SSH session.

.DESCRIPTION
    Locates Visual Studio via vswhere.exe, invokes vcvarsall.bat, and imports all resulting
    environment variables into the current PowerShell process scope.

.PARAMETER Arch
    Target architecture (e.g. x64, x64_arm64, arm64). Default: x64

.EXAMPLE
    # Dot-source the script to load environment variables into current session:
    . .\scripts\init_msvc_env.ps1
    . .\scripts\init_msvc_env.ps1 -Arch x64_arm64
#>

param(
    [string]$Arch = "x64"
)

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $vswhere)) {
    Write-Error "[ERROR] vswhere.exe not found at '$vswhere'. Please ensure Visual Studio is installed."
    return
}

$vsPath = & $vswhere -latest -property installationPath
if (-not $vsPath) {
    Write-Error "[ERROR] Could not locate Visual Studio installation path."
    return
}

$vcvarsBat = Join-Path $vsPath "VC\Auxiliary\Build\vcvarsall.bat"
if (-not (Test-Path $vcvarsBat)) {
    Write-Error "[ERROR] vcvarsall.bat not found at '$vcvarsBat'."
    return
}

Write-Host "============================================================================" -ForegroundColor Cyan
Write-Host " Initializing MSVC Environment for PowerShell SSH Session                   " -ForegroundColor Cyan
Write-Host "============================================================================" -ForegroundColor Cyan
Write-Host "[INFO] Found Visual Studio at: $vsPath" -ForegroundColor Yellow
Write-Host "[INFO] Loading MSVC environment for architecture [$Arch]..." -ForegroundColor Yellow

$cmd = """$vcvarsBat"" $Arch && set"
$envVars = cmd.exe /c $cmd

foreach ($var in $envVars) {
    if ($var -match "^([^=]+)=(.*)$") {
        $name = $matches[1]
        $value = $matches[2]
        Set-Item -Path "env:$name" -Value $value
    }
}

$clCmd = Get-Command "cl.exe" -ErrorAction SilentlyContinue
if ($clCmd) {
    Write-Host "[SUCCESS] MSVC environment initialized successfully!" -ForegroundColor Green
    Write-Host "  -> cl.exe:    $($clCmd.Source)" -ForegroundColor Green
    $cmakeCmd = Get-Command "cmake.exe" -ErrorAction SilentlyContinue
    if ($cmakeCmd) { Write-Host "  -> cmake.exe: $($cmakeCmd.Source)" -ForegroundColor Green }
    $ninjaCmd = Get-Command "ninja.exe" -ErrorAction SilentlyContinue
    if ($ninjaCmd) { Write-Host "  -> ninja.exe: $($ninjaCmd.Source)" -ForegroundColor Green }
} else {
    Write-Warning "[WARNING] cl.exe was not found in PATH after initialization."
}
Write-Host "============================================================================" -ForegroundColor Cyan
