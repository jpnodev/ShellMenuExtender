@echo off
setlocal

echo.
echo ========================================
echo Shell Menu Extender - Uninstaller
echo ========================================
echo.
echo This will remove the Shell Menu Extender
echo from your Windows 11 File Explorer.
echo.
echo Press any key to continue or Ctrl+C to cancel...
pause >nul

:: Check if running as administrator
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo ERROR: Not running as Administrator
    echo ========================================
    echo.
    echo Please right-click this script and select
    echo "Run as administrator"
    echo.
    pause
    exit /b 1
)

echo.
echo Uninstalling Shell Menu Extender...

:: Unregister the package
powershell -NoProfile -ExecutionPolicy Bypass -Command "if (Get-AppxPackage ShellMenuExtender) { Get-AppxPackage ShellMenuExtender | Remove-AppxPackage; Write-Host 'Uninstalled successfully!' } else { Write-Host 'Package not found - may already be uninstalled.' }"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Uninstallation Complete!
    echo ========================================
    echo.
    echo The Shell Menu Extender has been removed.
    echo.
    echo Note: Your configuration file remains at:
    echo   config\menu_config.json
    echo.
    echo You can safely delete this folder if you
    echo no longer need the application.
    echo.
    echo Restarting Windows Explorer...
    timeout /t 2 >nul
    taskkill /f /im explorer.exe >nul 2>&1
    start explorer.exe
    echo.
    echo Done!
) else (
    echo.
    echo ========================================
    echo Uninstallation Failed!
    echo ========================================
    echo.
    echo Please ensure you're running this script
    echo as Administrator.
    echo.
)

echo.
pause
