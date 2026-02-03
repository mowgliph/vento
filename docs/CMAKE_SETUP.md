# Vento Project - CMake Configuration Guide

## Problem
The CMake configuration is failing because Visual Studio Build Tools 2022 is installed but the Windows SDK components are missing or incomplete.

## Error Analysis
1. **Visual Studio Build Tools Found**: ✅ Located at `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools`
2. **C++ Compiler Found**: ✅ MSVC 19.44.35220.0 available
3. **Windows SDK Issues**: ❌ Missing `kernel32.lib`, RC (Resource Compiler), and MT (Manifest Tool)

## Solutions

### Option A: Install Complete Windows SDK (Recommended)
1. Download Windows 10 SDK from: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
2. Install the latest Windows 10 SDK (includes libraries, headers, and tools)
3. Run `configure.bat` again

### Option B: Install Visual Studio 2022 Community (Alternative)
1. Download Visual Studio 2022 Community from: https://visualstudio.microsoft.com/vs/community/
2. During installation, select:
   - Desktop development with C++
   - Windows 10/11 SDK
   - C++ CMake tools for Windows
3. Run `configure.bat` again

### Option C: Use MinGW-w64 (Alternative Toolchain)
1. Download MSYS2 from: https://www.msys2.org/
2. Install MinGW-w64 toolchain:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
   ```
3. Use MinGW-w64 shell to configure and build

## Current Status
- CMakeLists.txt has been modified to handle missing Windows SDK tools
- Configuration script (`configure.bat`) sets up VS Build Tools environment
- Project structure is ready for compilation once SDK issues are resolved

## Next Steps
1. Choose one of the solutions above
2. Install the required components
3. Run `configure.bat` to test the configuration
4. If successful, build with `nmake`

## Files Modified
- `CMakeLists.txt`: Added Windows SDK detection and manifest handling
- `configure.bat`: Created configuration script with VS environment setup
