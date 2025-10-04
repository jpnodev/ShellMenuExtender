@echo off
setlocal enabledelayedexpansion

:: Navigate to project root
cd /d "%~dp0\.."

echo ========================================
echo Creating Release Bundle
echo ========================================
echo.

:: Get version from user
set /p "VERSION=Enter version (e.g., 2.0.0): "
if "%VERSION%"=="" (
    echo ERROR: Version is required!
    pause
    exit /b 1
)

set "RELEASE_NAME=ShellMenuExtender-v%VERSION%"
set "RELEASE_DIR=%~dp0..\release\%RELEASE_NAME%"

echo.
echo Creating release: %RELEASE_NAME%
echo.

:: Clean old release
if exist "release\%RELEASE_NAME%" (
    echo Removing old release directory...
    rmdir /s /q "release\%RELEASE_NAME%"
)

:: Create release directories
echo Creating directory structure...
mkdir "release\%RELEASE_NAME%"
mkdir "release\%RELEASE_NAME%\build"
mkdir "release\%RELEASE_NAME%\resources"
mkdir "release\%RELEASE_NAME%\resources\icons"
mkdir "release\%RELEASE_NAME%\package"
mkdir "release\%RELEASE_NAME%\scripts"
mkdir "release\%RELEASE_NAME%\config"

:: Copy build outputs
echo Copying build outputs...
if not exist "build\ShellMenuExtender.dll" (
    echo ERROR: ShellMenuExtender.dll not found! Build the project first.
    pause
    exit /b 1
)
copy "build\ShellMenuExtender.dll" "release\%RELEASE_NAME%\build\" >nul
copy "build\AppInfo.exe" "release\%RELEASE_NAME%\build\" >nul

:: Copy resources
echo Copying resources...
xcopy "resources\icons\*.*" "release\%RELEASE_NAME%\resources\icons\" /Y /Q >nul

:: Copy package files
echo Copying package manifests...
copy "package\AppxManifest.xml" "release\%RELEASE_NAME%\package\" >nul
copy "package\AppInfo.exe.manifest" "release\%RELEASE_NAME%\package\" >nul

:: Copy scripts (only install and uninstall)
echo Copying installation scripts...
copy "scripts\install.bat" "release\%RELEASE_NAME%\install.bat" >nul
copy "scripts\uninstall.bat" "release\%RELEASE_NAME%\uninstall.bat" >nul

:: Copy config
echo Copying configuration...
copy "config\menu_config.json" "release\%RELEASE_NAME%\config\" >nul

:: Copy documentation
echo Copying documentation...
copy "README.md" "release\%RELEASE_NAME%\" >nul
copy "LICENSE" "release\%RELEASE_NAME%\" >nul 2>nul

:: Create INSTALL.txt with quick instructions
echo Creating installation guide...
(
echo ========================================
echo Shell Menu Extender v%VERSION%
echo ========================================
echo.
echo Thank you for downloading Shell Menu Extender!
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
echo    - Follow the prompts
echo.
echo 3. Done!
echo    - Right-click any file or folder
echo    - Your custom menu items will appear
echo.
echo ========================================
echo.
echo CUSTOMIZATION:
echo.
echo Your personal config is automatically created at:
echo   %USERPROFILE%\Documents\ShellMenuExtender\menu_config.json
echo.
echo To customize your menu:
echo   1. Open: notepad %USERPROFILE%\Documents\ShellMenuExtender\menu_config.json
echo   2. Edit and save
echo   3. Right-click to see changes instantly!
echo.
echo Changes apply INSTANTLY - no reinstall needed!
echo.
echo Example menu items:
echo   - Open in PowerShell
echo   - Open in Ubuntu/WSL
echo   - Open in VS Code
echo   - Git Bash
echo   - And more...
echo.
echo See README.md for full configuration guide.
echo.
echo ========================================
echo.
echo UNINSTALL:
echo.
echo Simply run uninstall.bat as administrator.
echo.
echo ========================================
echo.
echo NEED HELP?
echo.
echo - Check README.md for detailed documentation
echo - Visit: https://github.com/jpnodev/ShellMenuExtender
echo - Report issues on GitHub
echo.
echo ========================================
) > "release\%RELEASE_NAME%\INSTALL.txt"

:: Create ZIP archive
echo.
echo Creating ZIP archive...
call powershell -NoProfile -ExecutionPolicy Bypass -Command "& {Compress-Archive -Path 'release\%RELEASE_NAME%\*' -DestinationPath 'release\%RELEASE_NAME%.zip' -Force -ErrorAction Stop; exit 0}"

if exist "release\%RELEASE_NAME%.zip" (
    echo.
    echo ========================================
    echo Release created successfully!
    echo ========================================
    echo.
    echo Location: release\%RELEASE_NAME%.zip
    echo.
    echo Contents:
    echo   - build\ShellMenuExtender.dll
    echo   - build\AppInfo.exe
    echo   - resources\icons\
    echo   - package\AppxManifest.xml
    echo   - config\menu_config.json
    echo   - install.bat ^(simplified installer^)
    echo   - uninstall.bat ^(uninstaller^)
    echo   - README.md
    echo   - INSTALL.txt
    echo.
    echo Users just need to:
    echo   1. Extract the ZIP
    echo   2. Right-click install.bat
    echo   3. Run as administrator
    echo.
    echo Ready to upload to GitHub Releases!
    echo ========================================
    goto :release_success
)

echo.
echo ERROR: Failed to create ZIP archive!
pause
exit /b 1

:release_success
echo.
pause
