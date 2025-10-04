# Shell Menu Extender

A customizable Windows 11 context menu extension that adds custom menu items to File Explorer.

## ✨ Features

- 🎯 Appears in Windows 11 modern context menu (not in "Show more options")
- 🚀 Fast performance with optimized COM implementation
- 🎨 Fully configurable via JSON with instant hot-reload
- 📁 Works on files, folders, and background clicks
- 🔧 Support for multiple menu items with custom commands
- 🌐 Path placeholders for Windows and WSL paths
- ⚡ No re-registration needed after config changes
- 👤 Per-user configuration in Documents folder

---

## 🚀 Quick Start

### For End Users (Pre-built Release)

**Requirements:**
- Windows 11 (21H2 or later)
- Developer Mode enabled in Windows Settings

**Installation:**

1. **Download** the latest release from [GitHub Releases](https://github.com/jpnodev/ShellMenuExtender/releases)
2. **Extract** the ZIP to a permanent location (e.g., `C:\Program Files\ShellMenuExtender`)
3. **Right-click** `install.bat` → **Run as administrator**
4. **Done!** Right-click any file/folder to see your custom menu

**Customize:**
- Edit `%USERPROFILE%\Documents\ShellMenuExtender\menu_config.json`
- Changes apply instantly - no reinstall needed!

**Uninstall:**
- Right-click `uninstall.bat` → Run as administrator

---

### For Developers (Build from Source)

**Requirements:**
- Windows 11 (21H2 or later)
- Visual Studio 2022 Build Tools (Desktop development with C++)
- Windows SDK 10.0.26100.0 or later
- Developer Mode enabled
- Administrator privileges for installation

**Quick Build:**

```powershell
# Clone the repository
git clone https://github.com/jpnodev/ShellMenuExtender.git
cd ShellMenuExtender

# Build (one command)
.\build.bat

# Choose Y to install, or N to skip
# If N, you can install later from: build\app\install.bat
```

**What it does:**
1. Compiles ShellMenuExtender.dll and AppInfo.exe
2. Creates portable app package in `build\app\`
3. Asks if you want to install now

**Build outputs:**
- `build\ShellMenuExtender.dll` - Main context menu extension
- `build\AppInfo.exe` - About dialog
- `build\app\` - Complete portable app package
- `logs\build.log` - Build log

**Uninstall:**
```powershell
.\uninstall.bat
```

---

## ⚙️ Configuration

**📁 Config Location:** `%USERPROFILE%\Documents\ShellMenuExtender\menu_config.json`

**Auto-setup:** On first run, the extension:
1. Creates `Documents\ShellMenuExtender\` folder
2. Copies default config from installation
3. Loads your personal configuration

**✨ Hot-reload:** Edit config and changes apply instantly - just right-click!

### Configuration Example

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
| `icon` | Icon path or system icon | `"powershell.exe,0"`, `"./resources/icons/myicon.ico"` |
| `command` | Executable to run | `"wt.exe"`, `"cmd.exe"`, `"notepad.exe"` |
| `args` | Command arguments | Use `{path}` or `{wsl_path}` placeholders |
| `workingDir` | Working directory | Leave empty to use selected folder |
| `showWindow` | Window state | `"normal"`, `"hidden"`, `"minimized"`, `"maximized"` |
| `subItems` | Submenu items | Array of menu items (future use) |

### Icon Paths

- **System icons**: `"shell32.dll,-4"`, `"imageres.dll,-109"`, `"powershell.exe,0"`
- **Relative paths**: `"./resources/icons/myicon.ico"` (from DLL location)
- **Absolute paths**: `"C:\\path\\to\\icon.ico"`

**Common system icons:**
- `shell32.dll,-4` - Console/Terminal
- `shell32.dll,-16` - Folder
- `imageres.dll,-109` - Command Prompt
- `imageres.dll,-5301` - Modern Terminal

### Placeholders

- `{path}` - Windows path (e.g., `C:\Users\John\Documents`)
- `{wsl_path}` - WSL path (e.g., `/mnt/c/Users/John/Documents`)

### Example Menu Items

**PowerShell:**
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

**VS Code:**
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

---

## 📁 Project Structure

```
ShellMenuExtender/
├── build.bat                  # ⭐ Build everything (one command)
├── uninstall.bat              # Uninstall from system
│
├── src/                       # Source code
│   ├── ShellMenuExtender.cpp  # Main COM implementation
│   ├── ShellMenuExtender.h    # COM class definitions
│   ├── MenuConfig.cpp         # Configuration parser
│   ├── MenuConfig.h           # Config class
│   └── AppInfo.cpp            # About dialog
│
├── resources/                 # Resource files
│   ├── icons/                 # Icon assets
│   │   ├── logo.png
│   │   ├── logo_44.png
│   │   ├── logo_150.png
│   │   └── ubuntu_favicon-32x32.ico
│   ├── ShellMenuExtender.rc   # Resource script
│   └── ShellMenuExtender.def  # Export definitions
│
├── build/                     # Build outputs (gitignored)
│   ├── ShellMenuExtender.dll  # Main DLL
│   ├── AppInfo.exe            # About dialog
│   └── package/               # Portable package (auto-created)
│       ├── install.bat
│       ├── uninstall.bat
│       ├── build/
│       ├── resources/
│       ├── package/
│       ├── config/
│       └── README.md
│
├── package/                   # MSIX package manifests
│   ├── AppxManifest.xml       # Package manifest
│   └── AppInfo.exe.manifest   # App manifest
│
├── config/                    # Default configuration template
│   └── menu_config.json       # Copied to user's Documents on first use
│
├── scripts/                   # Helper scripts
│   ├── install.bat            # End-user installer (copied to packages)
│   ├── uninstall.bat          # End-user uninstaller (copied to packages)
│   ├── create_release.bat     # GitHub release automation
│   └── create_icon.ps1        # Icon generation utility
│
├── logs/                      # Build logs (gitignored)
│   └── build.log
│
└── README.md                  # This file
```

---

## 🔨 Development Guide

### Daily Development Workflow

```powershell
# 1. Make code changes in src/
# 2. Build
.\build.bat
# Choose Y to install

# 3. Test in File Explorer
# Right-click to see your menu

# 4. Repeat!
```

### Build Without Installing

```powershell
.\build.bat
# Choose: n

# Package is ready in: build\app\
# Install later: build\app\install.bat
```

### Share with Testers

```powershell
# Build
.\build.bat
# Choose N (don't install)

# Copy build\app\ folder to tester
# Tester runs: install.bat as administrator
```

### Create GitHub Release

```powershell
# 1. Build first
.\build.bat

# 2. Create release
.\scripts\create_release.bat
# Enter version: 2.0.0

# 3. Upload release\ShellMenuExtender-v2.0.0.zip to GitHub
```

### Modify Configuration (No Rebuild Needed)

```powershell
# Edit config
notepad %USERPROFILE%\Documents\ShellMenuExtender\menu_config.json

# Save and right-click in Explorer
# Changes apply instantly!
```

---

## 🛠️ Troubleshooting

### Menu Not Showing

1. Verify package is registered:
   ```powershell
   Get-AppxPackage -Name "ShellMenuExtender"
   ```
2. Check config has valid items with `id` and `title`
3. Restart Explorer:
   ```powershell
   Stop-Process -Name explorer -Force
   ```

### DLL Locked During Build

```powershell
# Find and stop process
Get-Process | Where-Object {$_.Modules.FileName -like '*ShellMenuExtender.dll*'} | Stop-Process -Force

# Or uninstall first
.\uninstall.bat

# Then rebuild
.\build.bat
```

### Icons Not Displaying

- For system icons, use format: `"dll-name.dll,-resourceId"`
- For relative paths, use: `"./resources/icons/filename.ico"`
- Ensure icon files exist at specified path

### Force Clean Rebuild

```powershell
# Uninstall
.\uninstall.bat

# Delete build folder
Remove-Item -Recurse -Force build

# Build fresh
.\build.bat
```

---

##  Technical Details

### Architecture

- **COM Server**: Implements `IExplorerCommand` interface
- **MSIX Sparse Package**: For Windows 11 integration
- **Hot-reload**: Config loaded on every menu open
- **Multi-item Support**: Parent menu with subitems (when multiple items configured)
- **Path Processing**: Automatic Windows ↔ WSL path conversion

### Performance

- Embedded icons for fast loading
- Config cached after first load
- Optimized COM reference counting
- Minimal memory footprint

### Configuration Storage

- **User config**: `%USERPROFILE%\Documents\ShellMenuExtender\menu_config.json`
- **Per-user**: Each Windows user has their own config
- **Survives updates**: Config persists through reinstalls
- **Hot-reload**: No restart needed for config changes

---

## 📝 Version History

**1.0.0** - Production Release
- ✅ Full JSON configuration support
- ✅ Hot-reload without re-registration
- ✅ Multiple menu items
- ✅ Relative icon paths
- ✅ Escaped quotes in arguments
- ✅ Per-user configuration in Documents folder
- ✅ Simplified build system (single `build.bat`)
- ✅ Modern Windows 11 Fluent Design UI

---

## 📄 License

MIT License - See repository for details

---

## 🤝 Contributing

1. Fork the repository
2. Make changes in `src/` directory
3. Test thoroughly with various configurations
4. Build: `.\build.bat`
5. Submit pull request with clear description

---

## 🆘 Support

- **Documentation**: This README
- **Issues**: [GitHub Issues](https://github.com/jpnodev/ShellMenuExtender/issues)
- **Discussions**: [GitHub Discussions](https://github.com/jpnodev/ShellMenuExtender/discussions)

---

**Note:** This extension modifies the Windows shell. Always test in a safe environment before deploying to production systems.

**Note:** The Readme was prompt-generated by Claude Sonnet 4.5