@echo off
chcp 65001 >nul
title CS2 External SkinChanger Build (LTO Optimized)
color 0A

echo ========================================
echo CS2 External SkinChanger - LTO BUILD
echo ========================================
echo.

REM --- Configuration ---
set PROJECT_DIR=%CD%
set SRC_DIR=%PROJECT_DIR%\src
set EXT_DIR=%PROJECT_DIR%\ext


REM --- Run dumper before build ---
echo [0/5] Running offset dumper...

if exist "cs2-dumper.exe" (
    echo Found cs2-dumper.exe, updating offsets...
    cs2-dumper.exe

    if %ERRORLEVEL% neq 0 (
        echo WARNING! Dumper failed, compiling with possibly outdated offsets.
    ) else (
        echo Offsets updated successfully.
    )
) else (
    echo WARNING! compiling with old offsets
)

echo.

REM --- Clean previous build ---
if exist "ext-cs2-skin-changer.exe" del "ext-cs2-skin-changer.exe"
del *.obj *.pdb *.ilk *.exp 2>nul

echo [1/5] Setting up compiler environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 (
    echo ERROR: Visual Studio vcvarsall.bat not found.
    pause
    exit /b 1
)

echo [2/5] Compiling with Whole Program Optimization (/GL)...
cl /std:c++20 /EHsc /O2 /MT /GL /Gw /DNDEBUG /wd4244 /I"%SRC_DIR%" /I"%SRC_DIR%\include" /I"%SRC_DIR%\SDK" /I"%EXT_DIR%" /c "%SRC_DIR%\*.cpp"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo [3/5] Linking with LTCG and ICF...
link /OUT:ext-cs2-skin-changer.exe *.obj d3d11.lib d3dcompiler.lib dxgi.lib user32.lib dwmapi.lib gdi32.lib gdiplus.lib ole32.lib  /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Final linking failed!
    pause
    exit /b 1
)

echo [4/5] Cleaning up temporary files...
del *.obj *.ilk *.exp 2>nul

echo.
echo ========================================
echo LTO BUILD SUCCESSFUL!
echo ========================================
echo.
pause
