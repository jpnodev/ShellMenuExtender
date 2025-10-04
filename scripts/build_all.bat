@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0\.."

if not exist "build" mkdir build
if not exist "logs" mkdir logs

set "LOG_FILE=logs\build.log"

echo ======================================== > "%LOG_FILE%"
echo Shell Menu Extender Build Log >> "%LOG_FILE%"
echo Build started: %DATE% %TIME% >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo Building Shell Menu Extender...
echo Building Shell Menu Extender... >> "%LOG_FILE%"

echo [%TIME%] Setting up Visual Studio environment... >> "%LOG_FILE%"
echo Setting up Visual Studio environment...

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to setup Visual Studio environment! >> "%LOG_FILE%"
    echo ERROR: Failed to setup Visual Studio environment!
    echo Check build.log for details.
    pause
    exit /b 1
)

echo [%TIME%] Visual Studio environment ready >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo [%TIME%] Building ShellMenuExtender.dll... >> "%LOG_FILE%"
echo Building ShellMenuExtender.dll...

echo Compiling resource file... >> "%LOG_FILE%"
rc.exe /nologo /fo build\ShellMenuExtender.res resources\ShellMenuExtender.rc >> "%LOG_FILE%" 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo [%TIME%] [ERROR] Resource compilation failed! Error code: %ERRORLEVEL% >> "%LOG_FILE%"
    echo ERROR: Resource compilation failed! Check build.log for details.
    pause
    exit /b 1
)

cl.exe /nologo /LD /EHsc /W4 /MD /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared" /Fo:build\ /Fe:build\ShellMenuExtender.dll src\ShellMenuExtender.cpp src\MenuConfig.cpp build\ShellMenuExtender.res /link /DEF:resources\ShellMenuExtender.def /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64" shell32.lib ole32.lib user32.lib shlwapi.lib oleaut32.lib >> "%LOG_FILE%" 2>&1

set BUILD_ERROR=%ERRORLEVEL%
if %BUILD_ERROR% NEQ 0 (
    echo [%TIME%] [ERROR] DLL build failed! Error code: %BUILD_ERROR% >> "%LOG_FILE%"
    echo ERROR: DLL build failed! Check build.log for details.
    pause
    exit /b 1
)

echo [%TIME%] ShellMenuExtender.dll built successfully >> "%LOG_FILE%"
echo   [OK] ShellMenuExtender.dll built successfully

echo. >> "%LOG_FILE%"
echo [%TIME%] Building AppInfo.exe... >> "%LOG_FILE%"
echo Building AppInfo.exe...

cl.exe /nologo /EHsc /W4 /MD /Fo:build\ /Fe:build\AppInfo.exe src\AppInfo.cpp /link user32.lib gdi32.lib dwmapi.lib uxtheme.lib comctl32.lib shcore.lib >> "%LOG_FILE%" 2>&1

set BUILD_ERROR=%ERRORLEVEL%
if %BUILD_ERROR% NEQ 0 (
    echo [%TIME%] [ERROR] AppInfo.exe build failed! Error code: %BUILD_ERROR% >> "%LOG_FILE%"
    echo ERROR: AppInfo.exe build failed! Check build.log for details.
    pause
    exit /b 1
)

echo [%TIME%] AppInfo.exe built successfully >> "%LOG_FILE%"
echo   [OK] AppInfo.exe built successfully

echo. >> "%LOG_FILE%"
echo [%TIME%] Cleaning up intermediate files... >> "%LOG_FILE%"
echo Cleaning up intermediate files...

del build\*.obj build\*.exp build\*.lib 2>nul
echo [%TIME%] Cleanup completed >> "%LOG_FILE%"

echo. >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo Build completed: %DATE% %TIME% >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"

echo.
echo ========================================
echo Build successful!
echo ========================================
echo Built:
echo   - build\ShellMenuExtender.dll
echo   - build\AppInfo.exe
echo.
echo Log saved to: logs\build.log
echo.
echo To register the package, run: scripts\register_sparse_package.bat
echo ========================================
pause
exit /b 0
