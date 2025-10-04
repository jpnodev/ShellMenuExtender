@echo off
setlocal enabledelayedexpansion

:: Simple installer for pre-built releases
:: Does NOT build - uses existing binaries

echo.
echo ========================================
echo Shell Menu Extender - Installer
echo ========================================
echo.
echo This will install the Shell Menu Extender
echo to your Windows 11 File Explorer context menu.
echo.
echo Requirements:
echo   - Windows 11
echo   - Developer Mode enabled
echo   - Administrator privileges
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

:: Store the project root path (script is in release root)
set "PROJECT_ROOT=%~dp0"
set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

:: Check if DLL exists
if not exist "%PROJECT_ROOT%\build\ShellMenuExtender.dll" (
    echo.
    echo ========================================
    echo ERROR: Installation files not found!
    echo ========================================
    echo.
    echo Could not find: build\ShellMenuExtender.dll
    echo.
    echo Please ensure you extracted all files from the release.
    echo.
    pause
    exit /b 1
)

:: Step 1: Uninstall previous version if exists
echo.
echo [1/2] Checking for previous installation...
powershell -NoProfile -ExecutionPolicy Bypass -Command "if (Get-AppxPackage ShellMenuExtender) { Write-Host '    Found existing installation. Uninstalling...'; Get-AppxPackage ShellMenuExtender | Remove-AppxPackage; if ($?) { Write-Host '    Uninstalled successfully!' } else { Write-Host '    Warning: Uninstall may have failed' } } else { Write-Host '    No previous installation found.' }"

:: Step 2: Install
echo.
echo [2/2] Installing Shell Menu Extender...
powershell -NoProfile -ExecutionPolicy Bypass -Command "try { Add-AppxPackage -Register '%PROJECT_ROOT%\package\AppxManifest.xml' -ExternalLocation '%PROJECT_ROOT%'; exit 0 } catch { Write-Host 'Error: '$_.Exception.Message -ForegroundColor Red; exit 1 }"
set "INSTALL_RESULT=!ERRORLEVEL!"

if !INSTALL_RESULT! EQU 0 (
    echo.
    echo ========================================
    echo Installation Successful!
    echo ========================================
    echo.
    echo Installed from:
    echo   Location: %PROJECT_ROOT%
    echo   DLL:      build\ShellMenuExtender.dll
    echo   Config:   config\menu_config.json
    echo.
    echo What's next:
    echo   1. Right-click any folder or file in File Explorer
    echo   2. Your custom menu items will appear
    echo   3. Edit config\menu_config.json to customize
    echo      ^(changes apply instantly, no reinstall needed^)
    echo.
    echo To uninstall, run: uninstall.bat
    echo ========================================
    echo.
    echo Restarting Windows Explorer to apply changes...
    timeout /t 2 >nul
    taskkill /f /im explorer.exe >nul 2>&1
    start explorer.exe
    echo.
    echo Done! Your context menu extension is ready.
    echo.
    pause
    exit /b 0
)

:: If we get here, installation failed
echo.
echo ========================================
echo Installation Failed!
echo ========================================
echo.
echo Common issues:
echo   - Not running as Administrator
echo   - Windows 11 required
echo   - Developer mode not enabled
echo.
echo Solutions:
echo   1. Right-click this script and "Run as administrator"
echo   2. Enable Developer Mode in Windows Settings:
echo      Settings ^> Privacy ^& Security ^> For developers
echo   3. Ensure you're running Windows 11
echo.
echo If the problem persists, check the Microsoft Store
echo for any pending updates.
echo ========================================
echo.
pause
exit /b 1
