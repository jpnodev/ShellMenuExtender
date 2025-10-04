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
copy "scripts\install.bat" "release\%RELEASE_NAME%\scripts\" >nul
copy "scripts\register_sparse_package.bat" "release\%RELEASE_NAME%\scripts\" >nul
copy "scripts\unregister_package.bat" "release\%RELEASE_NAME%\scripts\" >nul

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
echo INSTALLATION:
echo.
echo 1. Enable Developer Mode in Windows Settings
echo    ^(Settings ^> Privacy ^& Security ^> For developers^)
echo.
echo 2. Right-click scripts\install.bat
echo.
echo 3. Select "Run as administrator"
echo.
echo 4. Follow on-screen prompts
echo.
echo ========================================
echo.
echo CUSTOMIZATION:
echo.
echo Edit config\menu_config.json to customize menu items.
echo Changes apply instantly - no need to reinstall!
echo.
echo ========================================
echo.
echo UNINSTALL:
echo.
echo Right-click scripts\unregister_package.bat
echo and run as administrator.
echo.
echo ========================================
) > "release\%RELEASE_NAME%\INSTALL.txt"

:: Create ZIP archive
echo.
echo Creating ZIP archive...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path 'release\%RELEASE_NAME%\*' -DestinationPath 'release\%RELEASE_NAME%.zip' -Force"

if %ERRORLEVEL% EQU 0 (
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
    echo   - scripts\install.bat
    echo   - scripts\unregister_package.bat
    echo   - config\menu_config.json
    echo   - README.md
    echo   - INSTALL.txt
    echo.
    echo Ready to upload to GitHub Releases!
    echo ========================================
) else (
    echo.
    echo ERROR: Failed to create ZIP archive!
    pause
    exit /b 1
)

echo.
pause
