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

REM --- ACTUAL LIB PATHS ---
set CURL_LIB_DIR=%PROJECT_DIR%\packages\curl-vc140-static-32_64.7.53.0\lib\native\libs\x64\static\release
set OPENSSL_LIB_DIR=%SRC_DIR%\include\openssl-native.1.0.1\build\native\lib\x64

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
cl /std:c++20 /EHsc /O2 /MT /GL /Gw /DNDEBUG /DCURL_STATICLIB /wd4244 /I"%SRC_DIR%" /I"%SRC_DIR%\include" /I"%SRC_DIR%\SDK" /I"%EXT_DIR%" /c "%SRC_DIR%\main.cpp"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo [3/5] Linking with LTCG and ICF...
:: Added gdi32.lib to the list below
link /OUT:ext-cs2-skin-changer.exe *.obj d3d11.lib d3dcompiler.lib dxgi.lib user32.lib dwmapi.lib gdi32.lib gdiplus.lib ole32.lib /LIBPATH:"%OPENSSL_LIB_DIR%" libssl.lib libcrypto.lib ws2_32.lib wldap32.lib crypt32.lib normaliz.lib /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG

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
