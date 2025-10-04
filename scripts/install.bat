@echo off
setlocal enabledelayedexpansion

:: Store the project root path (parent of scripts directory)
set "PROJECT_ROOT=%~dp0.."

echo.
echo ========================================
echo Shell Menu Extender - Installer
echo ========================================
echo.
echo This script will:
echo   1. Build the extension (if needed)
echo   2. Uninstall previous version (if exists)
echo   3. Install/Update the extension
echo.
echo Press any key to continue or Ctrl+C to cancel...
pause >nul

:: Step 1: Check if already built
echo.
echo [1/3] Checking build status...
if exist "%PROJECT_ROOT%\build\ShellMenuExtender.dll" (
    echo     Build found! Checking if rebuild needed...
    
    :: Ask user if they want to rebuild
    set /p "REBUILD=Rebuild from source? (y/n, default=n): "
    if /i "!REBUILD!"=="y" (
        echo     Building...
        cd /d "%PROJECT_ROOT%"
        call scripts\build_all.bat
        if !ERRORLEVEL! NEQ 0 (
            echo.
            echo ERROR: Build failed!
            pause
            exit /b 1
        )
    ) else (
        echo     Using existing build.
    )
) else (
    echo     No build found. Building now...
    cd /d "%PROJECT_ROOT%"
    call scripts\build_all.bat
    if !ERRORLEVEL! NEQ 0 (
        echo.
        echo ERROR: Build failed!
        pause
        exit /b 1
    )
)

:: Step 2: Uninstall old version
echo.
echo [2/3] Checking for previous installation...
powershell -NoProfile -ExecutionPolicy Bypass -Command "if (Get-AppxPackage ShellMenuExtender) { Write-Host '    Found existing installation. Uninstalling...'; Get-AppxPackage ShellMenuExtender | Remove-AppxPackage; if ($?) { Write-Host '    Uninstalled successfully!' } else { Write-Host '    Warning: Uninstall may have failed' } } else { Write-Host '    No previous installation found.' }"

:: Step 3: Install
echo.
echo [3/3] Installing Shell Menu Extender...
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
    echo To uninstall: scripts\unregister_package.bat
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
echo   2. Enable Developer Mode in Windows Settings
echo   3. Check logs\build.log for errors
echo ========================================
echo.
pause
exit /b 1
