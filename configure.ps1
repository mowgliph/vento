# Vento Project - PowerShell Configuration Script
# Alternative to configure.bat for PowerShell users

Write-Host "===================================================================" -ForegroundColor Cyan
Write-Host "Vento Project - CMake Configuration Script (PowerShell)" -ForegroundColor Cyan
Write-Host "===================================================================" -ForegroundColor Cyan
Write-Host ""

# Check if VS Build Tools are available
$VSDevEnv = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if (-not (Test-Path $VSDevEnv)) {
    Write-Host "ERROR: Visual Studio Build Tools not found at expected location." -ForegroundColor Red
    Write-Host "Please install Visual Studio Build Tools 2022 with C++ tools." -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "Setting up Visual Studio x64 environment..." -ForegroundColor Green
& cmd.exe /c "call `"$VSDevEnv`" && set" | ForEach-Object {
    if ($_ -match '^(.+?)=(.*)$') {
        [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
    }
}

Write-Host ""
Write-Host "Configuring CMake with NMake generator..." -ForegroundColor Green

try {
    cmake -G "NMake Makefiles" .
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "===================================================================" -ForegroundColor Green
        Write-Host "SUCCESS! CMake configuration completed." -ForegroundColor Green
        Write-Host "===================================================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "To build the project:" -ForegroundColor Cyan
        Write-Host "  nmake" -ForegroundColor White
        Write-Host ""
        Write-Host "Or for Release build:" -ForegroundColor Cyan
        Write-Host "  nmake config=release" -ForegroundColor White
    } else {
        throw "CMake configuration failed"
    }
} catch {
    Write-Host ""
    Write-Host "===================================================================" -ForegroundColor Red
    Write-Host "ERROR! CMake configuration failed." -ForegroundColor Red
    Write-Host "===================================================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "This is usually caused by missing Windows SDK components." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "SOLUTIONS:" -ForegroundColor Cyan
    Write-Host "1. Install Windows 10 SDK:" -ForegroundColor White
    Write-Host "   https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/" -ForegroundColor Gray
    Write-Host ""
    Write-Host "2. Install Visual Studio 2022 Community:" -ForegroundColor White
    Write-Host "   https://visualstudio.microsoft.com/vs/community/" -ForegroundColor Gray
    Write-Host "   (Select 'Desktop development with C++' during installation)" -ForegroundColor Gray
    Write-Host ""
    Write-Host "3. See CMAKE_SETUP.md for detailed instructions." -ForegroundColor White
    Write-Host ""
}

Read-Host "Press Enter to exit"
