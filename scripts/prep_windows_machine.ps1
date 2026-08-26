#Requires -RunAsAdministrator
<#
.SYNOPSIS
    Prepares a Windows development or CI build agent machine for sip2json and CTRE builds.

.DESCRIPTION
    This script performs system configuration required for building sip2json and CTRE on Windows:
    1. Enables Windows Extended Long Paths (LongPathsEnabled = 1 in Registry) to prevent MAX_PATH truncation.
    2. Configures Git globally to enable long path support (git config --global core.longpaths true).
    3. Sets CPM_SOURCE_CACHE environment variable to a short root directory (C:\cpmcache) to avoid nested path errors.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File .\scripts\prep_windows_machine.ps1
#>

[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"

Write-Host "==========================================================================" -ForegroundColor Cyan
Write-Host " Preparing Windows Environment for sip2json & CTRE Builds                 " -ForegroundColor Cyan
Write-Host "==========================================================================" -ForegroundColor Cyan
Write-Host ""

# 1. Check Administrator Privileges
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object Security.Principal.WindowsPrincipal($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Error "This script must be executed in an elevated PowerShell session (Run as Administrator)."
    exit 1
}

# 2. Enable Windows Long Paths in Registry
Write-Host "[1/3] Enabling Windows Registry Extended Long Paths (LongPathsEnabled)..." -ForegroundColor Yellow
try {
    $regPath = "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem"
    $currentVal = (Get-ItemProperty -Path $regPath -Name "LongPathsEnabled" -ErrorAction SilentlyContinue).LongPathsEnabled

    if ($currentVal -eq 1) {
        Write-Host "  -> LongPathsEnabled is already enabled (1)." -ForegroundColor Green
    } else {
        Set-ItemProperty -Path $regPath -Name "LongPathsEnabled" -Value 1 -Type DWord -Force
        Write-Host "  -> Successfully set HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem\LongPathsEnabled = 1" -ForegroundColor Green
    }
} catch {
    Write-Warning "Failed to set Registry LongPathsEnabled: $_"
}

# 3. Configure Git Global Long Paths
Write-Host "[2/3] Configuring Git Global Core Long Paths..." -ForegroundColor Yellow
$gitCmd = Get-Command "git" -ErrorAction SilentlyContinue
if ($gitCmd) {
    try {
        & git config --global core.longpaths true
        $gitVal = & git config --global --get core.longpaths
        Write-Host "  -> Successfully configured git config --global core.longpaths = $gitVal" -ForegroundColor Green
    } catch {
        Write-Warning "Failed to set git core.longpaths: $_"
    }
} else {
    Write-Warning "Git executable not found in PATH. Please run 'git config --global core.longpaths true' manually after installing Git."
}

# 4. Set CPM_SOURCE_CACHE Environment Variable
Write-Host "[3/3] Setting CPM_SOURCE_CACHE Environment Variable..." -ForegroundColor Yellow
try {
    $cpmPath = "C:\cpmcache"
    if (-not (Test-Path $cpmPath)) {
        New-Item -ItemType Directory -Force -Path $cpmPath | Out-Null
    }
    [Environment]::SetEnvironmentVariable("CPM_SOURCE_CACHE", $cpmPath, [EnvironmentVariableTarget]::Machine)
    Write-Host "  -> Successfully set CPM_SOURCE_CACHE = $cpmPath (Machine Environment)" -ForegroundColor Green
} catch {
    Write-Warning "Failed to set CPM_SOURCE_CACHE environment variable: $_"
}

Write-Host ""
Write-Host "==========================================================================" -ForegroundColor Green
Write-Host " Windows Environment Preparation Completed Successfully!                 " -ForegroundColor Green
Write-Host " Note: Restart any open PowerShell/Command Prompt sessions for changes to apply. " -ForegroundColor Cyan
Write-Host "==========================================================================" -ForegroundColor Green
