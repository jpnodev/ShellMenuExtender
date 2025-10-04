@echo off
echo ========================================
echo Unregistering Shell Menu Extender
echo ========================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -Command "Get-AppxPackage ShellMenuExtender | Remove-AppxPackage"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Package unregistered successfully!
    echo ========================================
    echo.
    echo The Shell Menu Extender has been removed.
    echo.
    echo Restarting Windows Explorer...
    taskkill /f /im explorer.exe
    start explorer.exe
    echo.
    echo Explorer restarted!
) else (
    echo.
    echo ========================================
    echo ERROR: Failed to unregister package
    echo ========================================
    echo.
    echo The package may not be installed.
    echo Run this to check: Get-AppxPackage ShellMenuExtender
)

echo.
pause
