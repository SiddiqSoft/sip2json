@echo off
rem ============================================================================
rem init_msvc_env.bat - Initialize MSVC compiler environment for SSH CMD sessions
rem ============================================================================
rem Usage:
rem   scripts\init_msvc_env.bat [x64 | x64_arm64 | arm64 | x86]
rem
rem Default architecture: x64
rem Example:
rem   scripts\init_msvc_env.bat
rem   scripts\init_msvc_env.bat x64_arm64
rem ============================================================================

set "TARGET_ARCH=%~1"
if "%TARGET_ARCH%"=="" set "TARGET_ARCH=x64"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found at "%VSWHERE%". Is Visual Studio installed?
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -property installationPath`) do (
    set "VS_INSTALL_PATH=%%i"
)

if not defined VS_INSTALL_PATH (
    echo [ERROR] Visual Studio installation path could not be located.
    exit /b 1
)

set "VCVARS_BAT=%VS_INSTALL_PATH%\VC\Auxiliary\Build\vcvarsall.bat"

if not exist "%VCVARS_BAT%" (
    echo [ERROR] vcvarsall.bat not found at "%VCVARS_BAT%".
    exit /b 1
)

echo ============================================================================
echo  Initializing MSVC Environment for SSH Session
echo ============================================================================
echo [INFO] Found Visual Studio at: "%VS_INSTALL_PATH%"
echo [INFO] Configuring environment for target architecture [%TARGET_ARCH%]...

call "%VCVARS_BAT%" %TARGET_ARCH%

if errorlevel 1 (
    echo [ERROR] Failed to execute vcvarsall.bat.
    exit /b 1
)

echo [SUCCESS] MSVC environment initialized successfully!
where cl.exe 2>nul
where cmake.exe 2>nul
where ninja.exe 2>nul
echo ============================================================================
