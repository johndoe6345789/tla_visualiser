@echo off
REM Build script for TLA+ Visualiser (Windows)

setlocal enabledelayedexpansion

echo ===================================
echo TLA+ Visualiser Build Script
echo ===================================
echo.

REM Parse arguments
set BUILD_TYPE=Release
set CLEAN=false
set RUN_TESTS=true

:parse_args
if "%~1"=="" goto check_prereqs
if "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto parse_args
)
if "%~1"=="--clean" (
    set CLEAN=true
    shift
    goto parse_args
)
if "%~1"=="--no-tests" (
    set RUN_TESTS=false
    shift
    goto parse_args
)
if "%~1"=="--help" (
    echo Usage: build.bat [OPTIONS]
    echo.
    echo Options:
    echo   --debug      Build in Debug mode ^(default: Release^)
    echo   --clean      Clean build directory before building
    echo   --no-tests   Skip running tests
    echo   --help       Show this help message
    exit /b 0
)
echo Unknown option: %~1
echo Use --help for usage information
exit /b 1

:check_prereqs
echo Checking prerequisites...

where cmake >nul 2>&1
if errorlevel 1 (
    echo Error: cmake not found
    exit /b 1
)

where ninja >nul 2>&1
if errorlevel 1 (
    echo Error: ninja not found
    exit /b 1
)

where conan >nul 2>&1
if errorlevel 1 (
    echo Error: conan not found. Install with: pip install conan
    exit /b 1
)

echo All required tools found
echo.

REM Clean if requested
if "%CLEAN%"=="true" (
    echo Cleaning build directory...
    if exist build rmdir /s /q build
    echo Clean complete
    echo.
)

REM Detect Conan profile
if not exist "%USERPROFILE%\.conan2" if not exist "%USERPROFILE%\.conan" (
    echo Detecting Conan profile...
    conan profile detect
    echo Conan profile detected
    echo.
)

REM Install dependencies
echo Installing dependencies with Conan...
conan install . --output-folder=build --build=missing
if errorlevel 1 (
    echo Error: Conan install failed
    exit /b 1
)
echo Dependencies installed
echo.

REM Configure CMake
echo Configuring CMake ^(%BUILD_TYPE%^)...
cmake -B build -G Ninja ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)
echo Configuration complete
echo.

REM Build
echo Building project...
cmake --build build --config %BUILD_TYPE%
if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)
echo Build complete
echo.

REM Run tests if requested
if "%RUN_TESTS%"=="true" (
    echo Running tests...
    cd build
    ctest --output-on-failure
    if errorlevel 1 (
        echo Some tests failed
        cd ..
        exit /b 1
    )
    echo All tests passed
    cd ..
    echo.
)

REM Success message
echo ===================================
echo Build successful!
echo ===================================
echo.
echo To run the application:
echo   build\tla_visualiser.exe
echo.
echo Build artifacts are in: build\
echo Executable: build\tla_visualiser.exe

endlocal
