# Deployment Guide

This guide explains how to create release builds and optionally implement user-specific configuration.

## Creating a Release Build

### Quick Release (Current Directory Config)

1. **Build the project:**
   ```powershell
   .\scripts\build_all.bat
   ```

2. **Create release bundle:**
   ```powershell
   .\scripts\create_release.bat
   ```

3. **Enter version** when prompted (e.g., `2.0.0`)

4. **Upload to GitHub:**
   - The release ZIP will be in `release\ShellMenuExtender-v2.0.0.zip`
   - Create a new release on GitHub
   - Upload the ZIP file
   - Include installation instructions

### Release Package Contents

```
ShellMenuExtender-v2.0.0.zip
├── build/
│   ├── ShellMenuExtender.dll    # Main extension
│   └── AppInfo.exe               # About dialog
├── resources/
│   └── icons/                    # Icon files
├── package/
│   ├── AppxManifest.xml          # MSIX manifest
│   └── AppInfo.exe.manifest      # App manifest
├── scripts/
│   ├── install.bat               # All-in-one installer
│   ├── register_sparse_package.bat
│   └── unregister_package.bat
├── config/
│   └── menu_config.json          # Configuration file
├── README.md                     # Documentation
└── INSTALL.txt                   # Quick start guide
```

## User Documents Configuration (Optional)

To store configuration in each user's Documents folder (`%USERPROFILE%\Documents\ShellMenuExtender\menu_config.json`):

### 1. Modify MenuConfig.cpp

Add to the top:
```cpp
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
```

Replace the config path logic in `ShellMenuExtender::ShellMenuExtender()`:

```cpp
ShellMenuExtender::ShellMenuExtender() : m_refCount(1) {
    // Get user's Documents folder
    WCHAR documentsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath))) {
        std::wstring configDir = documentsPath;
        configDir += L"\\ShellMenuExtender";
        
        // Create directory if it doesn't exist
        CreateDirectoryW(configDir.c_str(), NULL);
        
        std::wstring configPath = configDir + L"\\menu_config.json";
        
        // Copy default config if it doesn't exist
        if (GetFileAttributesW(configPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            // Get installation directory
            WCHAR dllPath[MAX_PATH];
            GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
            
            // Remove filename
            WCHAR* lastSlash = wcsrchr(dllPath, L'\\');
            if (lastSlash) *lastSlash = L'\0';
            
            // Remove "build" directory
            lastSlash = wcsrchr(dllPath, L'\\');
            if (lastSlash) *lastSlash = L'\0';
            
            std::wstring defaultConfig = dllPath;
            defaultConfig += L"\\config\\menu_config.json";
            
            // Copy default config to user's Documents
            CopyFileW(defaultConfig.c_str(), configPath.c_str(), FALSE);
        }
        
        MenuConfig::Instance().Load(configPath);
    }
}
```

### 2. Update AppInfo.cpp

Update `LoadConfigPath()` function:

```cpp
std::wstring LoadConfigPath() {
    WCHAR documentsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath))) {
        std::wstring configPath = documentsPath;
        configPath += L"\\ShellMenuExtender\\menu_config.json";
        return configPath;
    }
    return L"";
}
```

### 3. Benefits of User Documents Config

**Advantages:**
- ✅ Each user has their own configuration
- ✅ Users can easily find and edit config (in Documents)
- ✅ Installation directory remains clean
- ✅ Survives app reinstalls
- ✅ Works with non-admin users

**Disadvantages:**
- ❌ Config not shared across users
- ❌ Slight overhead copying default config on first run

### 4. Hybrid Approach (Recommended)

Check both locations in this order:
1. User Documents folder (user-specific)
2. Installation config folder (system-wide default)

```cpp
std::wstring GetConfigPath() {
    // Try user's Documents folder first
    WCHAR documentsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath))) {
        std::wstring userConfig = documentsPath;
        userConfig += L"\\ShellMenuExtender\\menu_config.json";
        
        if (GetFileAttributesW(userConfig.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return userConfig;  // User config exists
        }
    }
    
    // Fall back to installation directory
    WCHAR dllPath[MAX_PATH];
    GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
    // ... get installation path
    return installConfig;
}
```

## GitHub Release Workflow

### 1. Prepare Release

```powershell
# Update version in code if needed
# Build project
.\scripts\build_all.bat

# Create release bundle
.\scripts\create_release.bat
# Enter version: 2.0.0
```

### 2. Create Git Tag

```powershell
git tag v2.0.0
git push origin v2.0.0
```

### 3. Create GitHub Release

1. Go to GitHub → Releases → "Create a new release"
2. Choose tag: `v2.0.0`
3. Release title: `Shell Menu Extender v2.0.0`
4. Description:
   ```markdown
   ## What's New
   - Customizable Windows 11 context menu extension
   - Instant hot-reload configuration
   - Modern Fluent Design about dialog
   
   ## Installation
   1. Download `ShellMenuExtender-v2.0.0.zip`
   2. Extract to a permanent location
   3. Enable Developer Mode in Windows Settings
   4. Right-click `scripts\install.bat` → Run as administrator
   
   ## Requirements
   - Windows 11 (21H2 or later)
   - Developer Mode enabled
   ```

5. Upload `release\ShellMenuExtender-v2.0.0.zip`
6. Publish release

### 4. Release Checklist

- [ ] Version number updated
- [ ] All code builds without errors
- [ ] Tested installation on clean system
- [ ] README.md is up to date
- [ ] License file included
- [ ] Git tag created and pushed
- [ ] GitHub release created
- [ ] ZIP file uploaded
- [ ] Installation instructions clear

## Build Server / CI/CD

For automated builds with GitHub Actions:

Create `.github/workflows/release.yml`:

```yaml
name: Build Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v1
    
    - name: Build
      run: .\scripts\build_all.bat
      
    - name: Create Release Bundle
      run: |
        $version = $env:GITHUB_REF -replace 'refs/tags/v', ''
        .\scripts\create_release.bat
      
    - name: Upload Release Asset
      uses: actions/upload-release-asset@v1
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        upload_url: ${{ github.event.release.upload_url }}
        asset_path: ./release/ShellMenuExtender-v*.zip
        asset_name: ShellMenuExtender-v${{ github.ref_name }}.zip
        asset_content_type: application/zip
```

## Versioning

Use semantic versioning (MAJOR.MINOR.PATCH):

- **MAJOR**: Breaking changes (e.g., 2.0.0)
- **MINOR**: New features, backward compatible (e.g., 2.1.0)
- **PATCH**: Bug fixes (e.g., 2.0.1)

Update version in:
- `AppInfo.cpp` (version string)
- `package\AppxManifest.xml` (Version attribute)
- Git tag
- GitHub release

## Distribution Channels

1. **GitHub Releases** (Primary)
   - Full source code
   - Pre-built binaries
   - Installation instructions

2. **Microsoft Store** (Future)
   - Requires MSIX packaging
   - App certification process
   - Automatic updates

3. **Direct Download**
   - Host ZIP on your website
   - Include signature/hash for verification
