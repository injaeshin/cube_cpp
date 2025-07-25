@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

REM ===== Cube CPP Project Build Script =====
echo.
echo ========================================
echo        Cube CPP Project Build Script
echo ========================================
echo.

if "%1"=="" goto :show_help
if "%1"=="help" goto :show_help
if "%1"=="-h" goto :show_help
if "%1"=="--help" goto :show_help

REM Build type setting (default: Debug)
set BUILD_TYPE=Debug

REM Parse arguments
:parse_args
if "%2"=="release" set BUILD_TYPE=Release
if "%2"=="Release" set BUILD_TYPE=Release

goto :handle_command

:show_help
echo Usage:
echo   build.bat [command] [build_type]
echo.
echo Commands:
echo   clean       - Delete build folder and rebuild from scratch
echo   debug       - Debug build (default)
echo   release     - Release build
echo   help        - Show this help
echo.
echo Build types:
echo   debug       - Debug build (default)
echo   release     - Release build
echo.
echo Examples:
echo   build.bat clean           # Full rebuild (Debug)
echo   build.bat clean release   # Release build
echo   build.bat debug           # Debug build
echo.
goto :end

:handle_command
if "%1"=="clean" goto :clean_build
if "%1"=="debug" goto :debug_build
if "%1"=="release" goto :release_build

echo Unknown command: %1
echo Run 'build.bat help' to see available commands.
goto :end

:clean_build
echo [1/4] Cleaning existing build folder...
if exist "build" (
    rmdir /s /q "build"
    echo      * build folder deleted
) else (
    echo      - no build folder found
)

echo [2/4] Creating build folder...
mkdir build
cd build

echo [3/4] Running CMake configuration... (Build type: %BUILD_TYPE%)
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..

if errorlevel 1 (
    echo      X CMake configuration FAILED!
    goto :error_exit
)
echo      * CMake configuration completed

echo [4/4] Building project...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 (
    echo      X Build FAILED!
    goto :error_exit
)
echo      * Build completed successfully!

echo.
echo ========================================
echo          BUILD SUCCESS! (%BUILD_TYPE%)
echo ========================================
echo.
goto :show_output_files

:debug_build
set BUILD_TYPE=Debug
goto :clean_build

:release_build
set BUILD_TYPE=Release
goto :clean_build

:show_output_files
echo Generated files:
if exist "%BUILD_TYPE%\test_asio_build.exe" (
    echo   - .\build\%BUILD_TYPE%\test_asio_build.exe
)
if exist "..\ext\asio-1_34_2\include\asio.hpp" (
    echo   - ASIO headers: ..\ext\asio-1_34_2\include\asio.hpp
) else (
    echo   - ASIO headers: NOT FOUND (check ext directory)
)
echo.
echo To run:
echo   .\build\%BUILD_TYPE%\test_asio_build.exe
echo.
goto :end

:error_exit
echo.
echo ========================================
echo              BUILD FAILED!
echo ========================================
echo.
cd ..
exit /b 1

:end
cd ..
echo Done.


