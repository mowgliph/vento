@echo off
echo ===================================================================
echo Vento Project - CMake Configuration Script
echo ===================================================================
echo.

REM Check if VS Build Tools are available
set "VS_DEVENV=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if not exist "%VS_DEVENV%" (
    echo ERROR: Visual Studio Build Tools not found at expected location.
    echo Please install Visual Studio Build Tools 2022 with C++ tools.
    pause
    exit /b 1
)

echo Setting up Visual Studio x64 environment...
call "%VS_DEVENV%"

echo.
echo Configuring CMake with NMake generator...
cmake -G "NMake Makefiles" .

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===================================================================
    echo SUCCESS! CMake configuration completed.
    echo ===================================================================
    echo.
    echo To build the project:
    echo   nmake
    echo.
    echo Or for Release build:
    echo   nmake config=release
) else (
    echo.
    echo ===================================================================
    echo ERROR! CMake configuration failed.
    echo ===================================================================
    echo.
    echo This is usually caused by missing Windows SDK components.
    echo.
    echo SOLUTIONS:
    echo 1. Install Windows 10 SDK:
    echo    https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
    echo.
    echo 2. Install Visual Studio 2022 Community:
    echo    https://visualstudio.microsoft.com/vs/community/
    echo    (Select "Desktop development with C++" during installation)
    echo.
    echo 3. See CMAKE_SETUP.md for detailed instructions.
    echo.
)

pause
