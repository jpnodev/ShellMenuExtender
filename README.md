# Shell Menu Extender

A customizable Windows 11 context menu extension that adds custom menu items to File Explorer.

## Features

- ✨ Appears in Windows 11 modern context menu (not in "Show more options")
- 🚀 Fast performance with optimized COM implementation
- 🎨 Fully configurable via JSON with instant hot-reload
- 📁 Works on files, folders, and background clicks
- 🔧 Support for multiple menu items with custom commands
- 🎯 Path placeholders for Windows and WSL paths
- 🔄 No re-registration needed after config changes

## Configuration

**✨ INSTANT UPDATES:** Configuration changes apply immediately! Just edit `config/menu_config.json` and right-click - no need to re-register the package.

Edit `config/menu_config.json` to customize your menu items:

```json
{
  "menuItems": [
    {
      "id": "open_ubuntu",
      "title": "Open in Ubuntu",
      "icon": "shell32.dll,-4",
      "command": "wt.exe",
      "args": "-p \"Ubuntu\" -- wsl.exe --cd \"{wsl_path}\"",
      "workingDir": "",
      "showWindow": "normal",
      "subItems": []
    }
  ]
}
```

### Configuration Fields

| Field | Description | Example |
|-------|-------------|---------|
| `id` | Unique identifier (required) | `"open_ubuntu"` |
| `title` | Menu item text (required) | `"Open in Ubuntu"` |
| `icon` | Icon path or system icon | `"powershell.exe,0"`, `"./resources/icons/myicon.ico"`, `"shell32.dll,-4"` |
| `command` | Executable to run | `"wt.exe"`, `"cmd.exe"`, `"notepad.exe"` |
| `args` | Command arguments | Use placeholders (see below) |
| `workingDir` | Working directory | Leave empty to use selected folder |
| `showWindow` | Window state | `"normal"`, `"hidden"`, `"minimized"`, `"maximized"` |
| `subItems` | Submenu items | Array of menu items (reserved for future) |

### Icon Paths

Icons can be specified in multiple ways:

- **Relative paths**: `"./resources/icons/myicon.ico"` (relative to DLL location)
- **System icons**: `"shell32.dll,-4"`, `"imageres.dll,-109"`
- **Executable icons**: `"powershell.exe,0"`, `"C:\\Program Files\\App\\app.exe,0"`
- **Absolute paths**: `"C:\\path\\to\\icon.ico"`

### Placeholders

Use these in the `args` field:

- `{path}` - Windows path (e.g., `C:\Users\John\Documents`)
- `{wsl_path}` - WSL path (e.g., `/mnt/c/Users/John/Documents`)

### Example Configurations

**Open in PowerShell:**
```json
{
  "id": "open_powershell",
  "title": "Open in PowerShell",
  "icon": "powershell.exe,0",
  "command": "powershell.exe",
  "args": "-NoExit -Command \"Set-Location '{path}'\"",
  "workingDir": "",
  "showWindow": "normal",
  "subItems": []
}
```

**Open in VS Code:**
```json
{
  "id": "open_vscode",
  "title": "Open in VS Code",
  "icon": "C:\\Program Files\\Microsoft VS Code\\Code.exe,0",
  "command": "C:\\Program Files\\Microsoft VS Code\\Code.exe",
  "args": "\"{path}\"",
  "workingDir": "",
  "showWindow": "normal",
  "subItems": []
}
```

**Git Bash:**
```json
{
  "id": "open_gitbash",
  "title": "Open Git Bash Here",
  "icon": "C:\\Program Files\\Git\\git-bash.exe,0",
  "command": "C:\\Program Files\\Git\\git-bash.exe",
  "args": "--cd=\"{path}\"",
  "workingDir": "{path}",
  "showWindow": "normal",
  "subItems": []
}
```

## Quick Start

### For End Users (Pre-built Release)

**Requirements:**
- Windows 11 (version 21H2 or later)
- Developer Mode enabled in Windows Settings

**Installation Steps:**

1. **Download** the latest release from [GitHub Releases](https://github.com/jpnodev/ShellMenuExtender/releases)
2. **Extract** the ZIP file to a permanent location (e.g., `C:\Program Files\ShellMenuExtender`)
3. **Right-click** `scripts\install.bat` → **Run as administrator**
4. **Done!** Right-click any file/folder to see your custom menu

**To customize:**
- Edit `config\menu_config.json` - changes apply instantly!
- No need to reinstall or restart Explorer

**To uninstall:**
- Right-click `scripts\unregister_package.bat` → Run as administrator

---

### For Developers (Build from Source)

**Build Requirements:**

- **Windows 11** (21H2 or later)
- **Visual Studio 2022 Build Tools** (or full VS 2022)
  - Desktop development with C++ workload
  - Windows 11 SDK (10.0.26100.0 or later)
  - MSVC v143 compiler
- **Windows Developer Mode** enabled
- **Administrator privileges** for installation

**Quick Build:**

```powershell
# Clone the repository
git clone https://github.com/jpnodev/ShellMenuExtender.git
cd ShellMenuExtender

# Build everything
.\scripts\build_all.bat

# Install (requires Administrator)
.\scripts\install.bat
```

**Build outputs:**
- `build\ShellMenuExtender.dll` - Main context menu extension
- `build\AppInfo.exe` - About dialog application
- `logs\build.log` - Detailed build log

**Manual steps:**

```powershell
# Build only
.\scripts\build_all.bat

# Register package (install)
.\scripts\register_sparse_package.bat

# Unregister package (uninstall)
.\scripts\unregister_package.bat
```

---

### Creating a Portable Release Bundle

To create a clean, redistributable package with config in user's Documents folder:

**Option 1: User Documents Config (Recommended for Multi-User)**

This approach stores the config in `%USERPROFILE%\Documents\ShellMenuExtender\menu_config.json` so each user has their own configuration.

1. **Modify `MenuConfig.cpp`** to look for config in Documents:
   ```cpp
   // In ShellMenuExtender constructor:
   WCHAR documentsPath[MAX_PATH];
   SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath);
   std::wstring configPath = documentsPath;
   configPath += L"\\ShellMenuExtender\\menu_config.json";
   
   // Create directory if it doesn't exist
   CreateDirectoryW((std::wstring(documentsPath) + L"\\ShellMenuExtender").c_str(), NULL);
   
   // Copy default config if it doesn't exist
   if (GetFileAttributesW(configPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
       // Copy from installation directory
       WCHAR dllPath[MAX_PATH];
       GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
       // ... extract path and copy default config
   }
   ```

2. **Update installer** to copy default config to Documents folder

3. **Package structure:**
   ```
   ShellMenuExtender-Release/
   ├── build/
   │   ├── ShellMenuExtender.dll
   │   └── AppInfo.exe
   ├── resources/
   │   └── icons/
   ├── package/
   │   └── AppxManifest.xml
   ├── scripts/
   │   ├── install.bat
   │   └── unregister_package.bat
   ├── config/
   │   └── menu_config.json (default template)
   └── README.md
   ```

**Option 2: Installation Directory Config (Current)**

Keep config in installation directory for system-wide configuration.

**To create release bundle:**

```powershell
# Create release directory
mkdir ShellMenuExtender-Release

# Copy required files
xcopy /E /I build ShellMenuExtender-Release\build
xcopy /E /I resources ShellMenuExtender-Release\resources
xcopy /E /I package ShellMenuExtender-Release\package
xcopy /E /I config ShellMenuExtender-Release\config
xcopy /E /I scripts ShellMenuExtender-Release\scripts
copy README.md ShellMenuExtender-Release\

# Zip it up
Compress-Archive -Path ShellMenuExtender-Release\* -DestinationPath ShellMenuExtender-v2.0.0.zip
```

**For GitHub Release:**

1. Tag your version: `git tag v2.0.0`
2. Push tag: `git push origin v2.0.0`
3. Create release on GitHub
4. Upload `ShellMenuExtender-v2.0.0.zip`
5. Include installation instructions in release notes

## Project Structure

```
ShellMenuExtender/
├── src/                          # Source code
│   ├── ShellMenuExtender.cpp     # Main COM implementation
│   ├── ShellMenuExtender.h       # COM class definitions
│   ├── MenuConfig.cpp            # Configuration parser
│   ├── MenuConfig.h              # Config class definition
│   └── AppInfo.cpp               # About dialog
├── resources/                    # Resource files
│   ├── ShellMenuExtender.rc      # Resource script
│   ├── ShellMenuExtender.def     # Export definitions
│   └── icons/                    # Icon assets
│       ├── logo.png
│       └── ubuntu_favicon-32x32.ico
├── build/                        # Build output (gitignored)
│   ├── ShellMenuExtender.dll     # Main DLL
│   └── AppInfo.exe               # About dialog executable
├── logs/                         # Log files (gitignored)
│   └── build.log                 # Build output log
├── scripts/                      # Build and deployment
│   ├── install.bat               # 🌟 All-in-one installer
│   ├── build_all.bat             # Build script
│   ├── register_sparse_package.bat  # Register/update script
│   ├── unregister_package.bat    # Uninstall script
│   └── create_icon.ps1           # Icon generation
├── package/                      # MSIX package files
│   ├── AppxManifest.xml          # Package manifest
│   └── AppInfo.exe.manifest      # App manifest
├── config/                       # Configuration
│   └── menu_config.json          # Menu items configuration
├── .vscode/                      # VSCode settings
├── .gitignore                    # Git ignore file
└── README.md                     # This file
```

## System Icons

Use these for the `icon` field for fast, built-in icons:

- `shell32.dll,-4` - Console/Terminal
- `shell32.dll,-16` - Folder
- `shell32.dll,-222` - Settings/Gear
- `imageres.dll,-109` - Command Prompt
- `imageres.dll,-5301` - Modern Terminal

## After Configuration Changes

**No action needed!** Changes to `config/menu_config.json` are applied instantly:

1. Edit `config/menu_config.json`
2. Right-click in File Explorer
3. See updated menu immediately!

**Note:** If you need to rebuild the DLL for code changes:
1. Stop Explorer processes: `Stop-Process -Name explorer -Force`
2. OR run `scripts\unregister_package.bat`
3. Build: `scripts\build_all.bat`
4. Register: `scripts\register_sparse_package.bat`

## Troubleshooting

### DLL is locked during rebuild

If you get "file in use" errors during build:

```powershell
# Find and stop the process holding the DLL
Get-Process | Where-Object {$_.Modules.FileName -like '*ShellMenuExtender.dll*'} | Stop-Process -Force

# Then rebuild
cd scripts
.\build_all.bat
```

### Menu not showing

1. Verify package is registered: `Get-AppxPackage -Name "ShellMenuExtender"`
2. Check `config/menu_config.json` has valid items with `id` and `title`
3. Restart Explorer: `Stop-Process -Name explorer -Force`

### Icons not displaying

- For relative paths, use `./resources/icons/filename.ico`
- Ensure icon files exist at the specified path
- For system icons, use format: `"dll-name.dll,-resourceId"`

## Requirements

- Windows 11 (Build 22000 or later)
- Visual Studio 2022 Build Tools (with C++ desktop development)
- Windows SDK 10.0.26100.0 or later
- PowerShell 5.1 or later (for build scripts)

## Technical Details

### Architecture

- **COM Server**: Implements `IExplorerCommand` interface
- **MSIX Sparse Package**: For Windows 11 modern context menu integration
- **Hot-reload**: Configuration loaded on every menu open
- **Multi-item Support**: Parent menu with subitems when multiple items configured
- **Path Processing**: Automatic conversion between Windows and WSL paths

### Performance

- Embedded icons for fast loading
- No disk I/O during menu display (config cached after first load)
- Optimized COM reference counting
- Minimal memory footprint

## Version

**2.0.0** - Production Release
- ✅ Full JSON configuration support
- ✅ Hot-reload without re-registration
- ✅ Multiple menu items
- ✅ Relative icon paths
- ✅ Escaped quotes in arguments
- ✅ Clean project structure

## License

MIT License - See repository for details

## Contributing

1. Fork the repository
2. Make changes in the `src/` directory
3. Test thoroughly with various configurations
4. Submit pull request with clear description

---

**Note**: This extension modifies the Windows shell. Always test in a safe environment before deploying to production systems.
