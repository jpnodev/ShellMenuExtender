@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

if not exist "build" mkdir build
if not exist "logs" mkdir logs

set "LOG_FILE=logs\build.log"

echo ======================================== > "%LOG_FILE%"
echo Shell Menu Extender Build Log >> "%LOG_FILE%"
echo Build started: %DATE% %TIME% >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo ========================================
echo Shell Menu Extender - Build
echo ========================================
echo.
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

cl.exe /nologo /EHsc /W4 /MD /Fo:build\ /Fe:build\AppInfo.exe src\AppInfo.cpp /link user32.lib gdi32.lib dwmapi.lib uxtheme.lib comctl32.lib shcore.lib shell32.lib >> "%LOG_FILE%" 2>&1

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

:: Create portable app package
echo.
echo Creating portable app package...
echo. >> "%LOG_FILE%"
echo [%TIME%] Creating portable app package... >> "%LOG_FILE%"

set "PKG_DIR=build\app"

:: Clean old package
if exist "%PKG_DIR%" (
    rmdir /s /q "%PKG_DIR%"
)

:: Create package directories
mkdir "%PKG_DIR%"
mkdir "%PKG_DIR%\build"
mkdir "%PKG_DIR%\resources"
mkdir "%PKG_DIR%\resources\icons"
mkdir "%PKG_DIR%\package"
mkdir "%PKG_DIR%\config"

:: Copy build outputs
copy "build\ShellMenuExtender.dll" "%PKG_DIR%\build\" >nul
copy "build\AppInfo.exe" "%PKG_DIR%\build\" >nul

:: Copy resources
xcopy "resources\icons\*.*" "%PKG_DIR%\resources\icons\" /Y /Q >nul

:: Copy package files
copy "package\AppxManifest.xml" "%PKG_DIR%\package\" >nul
copy "package\AppInfo.exe.manifest" "%PKG_DIR%\package\" >nul

:: Copy installation scripts
copy "scripts\install.bat" "%PKG_DIR%\install.bat" >nul
copy "scripts\uninstall.bat" "%PKG_DIR%\uninstall.bat" >nul

:: Copy default config template
copy "config\menu_config.json" "%PKG_DIR%\config\" >nul

:: Copy documentation
copy "README.md" "%PKG_DIR%\" >nul
copy "LICENSE" "%PKG_DIR%\" >nul 2>nul

:: Create INSTALL.txt
(
echo ========================================
echo Shell Menu Extender - Development Build
echo ========================================
echo.
echo This is a development build for testing.
echo.
echo QUICK START:
echo.
echo 1. Enable Developer Mode
echo    - Open Windows Settings
echo    - Go to: Privacy ^& Security ^> For developers
echo    - Turn ON "Developer Mode"
echo.
echo 2. Install
echo    - Right-click install.bat
echo    - Select "Run as administrator"
echo.
echo 3. Done!
echo    - Right-click any file or folder
echo    - Your custom menu items will appear
echo.
echo ========================================
echo.
echo CONFIGURATION:
echo.
echo Your personal config is at:
echo   %%USERPROFILE%%\Documents\ShellMenuExtender\menu_config.json
echo.
echo On first use, the default config will be copied there.
echo Edit it and changes apply instantly - no reinstall needed!
echo.
echo ========================================
echo.
echo UNINSTALL:
echo.
echo Simply run uninstall.bat as administrator.
echo.
echo ========================================
) > "%PKG_DIR%\INSTALL.txt"

echo. >> "%LOG_FILE%"
echo [%TIME%] App package created in build\app\ >> "%LOG_FILE%"

echo.
echo ========================================
echo Build Successful!
echo ========================================
echo.
echo Built:
echo   - build\ShellMenuExtender.dll
echo   - build\AppInfo.exe
echo   - build\app\ (portable app package)
echo.
echo Log saved to: logs\build.log
echo.

:: Ask user if they want to install
set "INSTALL_NOW="
set /p "INSTALL_NOW=Do you want to install now? (Y/n): "

:: Trim whitespace and convert to lowercase for comparison
set "INSTALL_NOW=%INSTALL_NOW: =%"
if /i "%INSTALL_NOW%"=="n" (
    echo.
    echo ========================================
    echo Installation Skipped
    echo ========================================
    echo.
    echo You can install later by running:
    echo   build\app\install.bat
    echo.
    echo Or create a GitHub release:
    echo   scripts\create_release.bat
    echo.
    echo ========================================
    pause
    exit /b 0
)
if /i "%INSTALL_NOW%"=="no" (
    echo.
    echo ========================================
    echo Installation Skipped
    echo ========================================
    echo.
    echo You can install later by running:
    echo   build\app\install.bat
    echo.
    echo Or create a GitHub release:
    echo   scripts\create_release.bat
    echo.
    echo ========================================
    pause
    exit /b 0
)

:: Install from the package
echo.
echo ========================================
echo Installing from package...
echo ========================================
echo.

cd /d "%~dp0build\app"
call install.bat
set INSTALL_RESULT=%ERRORLEVEL%

cd /d "%~dp0"

if %INSTALL_RESULT% NEQ 0 (
    echo. >> "%LOG_FILE%"
    echo [%TIME%] [ERROR] Installation failed! >> "%LOG_FILE%"
    echo.
    echo Installation failed!
    echo.
    echo You can retry by running:
    echo   build\app\install.bat
    echo.
    pause
    exit /b 1
)

echo. >> "%LOG_FILE%"
echo [%TIME%] Installation completed successfully >> "%LOG_FILE%"
echo.
echo ========================================
echo Build and Installation Complete!
echo ========================================
echo.
echo Configuration will be stored in:
echo   %USERPROFILE%\Documents\ShellMenuExtender\menu_config.json
echo.
echo Edit config and changes apply instantly!
echo ========================================
pause
exit /b 0
