@echo off
echo ========================================
echo Registering Shell Menu Extender
echo ========================================
echo.

:: Store the project root path (parent of scripts directory)
set "PROJECT_ROOT=%~dp0.."

REM Check if DLL exists
if not exist "%PROJECT_ROOT%\build\ShellMenuExtender.dll" (
    echo ERROR: build\ShellMenuExtender.dll not found!
    echo Please run scripts\build_all.bat first.
    echo.
    pause
    exit /b 1
)

REM Check if AppInfo.exe exists
if not exist "%PROJECT_ROOT%\build\AppInfo.exe" (
    echo ERROR: build\AppInfo.exe not found!
    echo Please run scripts\build_all.bat first.
    echo.
    pause
    exit /b 1
)

echo Unregistering old package (if exists)...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Get-AppxPackage ShellMenuExtender | Remove-AppxPackage" 2>nul

echo.
echo Registering sparse package...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Add-AppxPackage -Register '%PROJECT_ROOT%\package\AppxManifest.xml' -ExternalLocation '%PROJECT_ROOT%'"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Package registered successfully!
    echo ========================================
    echo.
    echo Files registered from:
    echo   DLL: %PROJECT_ROOT%\build\ShellMenuExtender.dll
    echo   Config: %PROJECT_ROOT%\config\menu_config.json
    echo.
    echo You can now:
    echo   - Right-click folders or files in File Explorer
    echo   - Your menu items will appear in the modern context menu
    echo.
    echo To modify menu items:
    echo   - Edit config\menu_config.json
    echo   - Changes apply immediately (no re-registration needed)
    echo.
    echo To uninstall: scripts\unregister_package.bat
    echo ========================================
    echo.
    echo Restarting Windows Explorer...
    taskkill /f /im explorer.exe
    start explorer.exe
    echo.
    echo Explorer restarted! Your menu should now appear.
) else (
    echo.
    echo ========================================
    echo ERROR: Failed to register package
    echo ========================================
    echo.
    echo Try running this script as Administrator:
    echo Right-click and select "Run as administrator"
    echo.
    echo If the package is already registered, uninstall it first:
    echo   Get-AppxPackage ShellMenuExtender ^| Remove-AppxPackage
    echo ========================================
)

echo.
pause
