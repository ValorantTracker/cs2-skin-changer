@echo off
setlocal
title CS2 Skin Changer - Auto Setup

echo ==========================================
echo    CS2 Skin Changer - Environment Setup
echo ==========================================
echo.

cd /d "%~dp0"

echo [1/3] Checking for NuGet...
where nuget >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] NuGet not found in PATH.
    echo [*] Attempting to use bundled nuget or ask user to install...
    echo [*] Downloading NuGet...
    powershell -Command "Invoke-WebRequest -Uri https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -OutFile nuget.exe"
    if exist nuget.exe (
        echo [+] NuGet downloaded successfully.
    ) else (
        echo [!] Failed to download NuGet. Please install it manually or run this script as Admin.
        pause
        exit /b 1
    )
)

echo.
echo [2/3] Restoring NuGet Packages...
if exist nuget.exe (
    nuget.exe restore src\ext-cs2-skin-changer.sln
) else (
    nuget restore src\ext-cs2-skin-changer.sln
)

if %errorlevel% neq 0 (
    echo [!] Package restore failed.
    pause
    exit /b 1
) else (
    echo [+] Packages restored.
)

echo.
echo [3/3] Build Ready!
echo.
echo Custom Instructions:
echo 1. Open 'src\ext-cs2-skin-changer.sln' in Visual Studio 2022.
echo 2. Select 'Release' and 'x64'.
echo 3. Press Ctrl+Shift+B to build.
echo.
echo Scripts and fixes applied. You can close this window.
pause
