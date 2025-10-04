# Shell Menu Extender - Project Organization

## Directory Structure

```
ShellMenuExtender/
├── .vscode/              # VS Code workspace settings
├── build/                # Build outputs (gitignored)
│   ├── ShellMenuExtender.dll
│   └── AppInfo.exe
├── config/               # Runtime configuration
│   └── menu_config.json  # Menu items configuration (hot-reload enabled)
├── logs/                 # Log files (gitignored)
│   └── build.log         # Build output log
├── package/              # MSIX package manifests
│   ├── AppxManifest.xml
│   └── AppInfo.exe.manifest
├── resources/            # Static resources
│   ├── icons/
│   │   ├── logo.png
│   │   ├── logo_44.png
│   │   ├── logo_150.png
│   │   └── ubuntu_favicon-32x32.ico
│   ├── ShellMenuExtender.rc    # Resource script
│   └── ShellMenuExtender.def   # Export definitions
├── scripts/              # Build and deployment automation
│   ├── install.bat
│   ├── build_all.bat
│   ├── create_release.bat
│   ├── register_sparse_package.bat
│   ├── unregister_package.bat
│   └── create_icon.ps1
├── src/                  # Source code
│   ├── AppInfo.cpp
│   ├── MenuConfig.cpp
│   ├── MenuConfig.h
│   ├── ShellMenuExtender.cpp
│   └── ShellMenuExtender.h
├── .gitignore
├── DEPLOYMENT.md
├── PROJECT_STRUCTURE.md
└── README.md
```

## File Organization by Purpose

### Source Code (`src/`)
- **ShellMenuExtender.cpp/h**: Main COM DLL implementation
  - Implements IExplorerCommand interface
  - Handles menu display and command execution
  - Manages multi-item menu structure

- **MenuConfig.cpp/h**: Configuration parser
  - JSON parsing with proper escape handling
  - Hot-reload functionality
  - Path placeholder processing ({path}, {wsl_path})

- **AppInfo.cpp**: About dialog executable
  - Shows current configuration
  - Displays version information

- **CodeGenerator.cpp/h**: Utility functions

### Resources (`resources/`)
- **icons/**: All image assets
  - logo.png: Parent menu icon (150x150)
  - logo_44.png: Small icon for Start Menu
  - ubuntu_favicon-32x32.ico: Ubuntu menu item icon

- **ShellMenuExtender.rc**: Resource compilation script
  - Embeds icons into DLL

- **ShellMenuExtender.def**: COM export definitions

### Configuration (`config/`)
- **menu_config.json**: Runtime menu configuration
  - Supports multiple menu items
  - Hot-reload: changes apply immediately
  - Relative icon paths supported

### Package (`package/`)
- **AppxManifest.xml**: MSIX sparse package manifest
  - Defines Windows 11 integration
  - Specifies COM registration
  - References build outputs

- **AppInfo.exe.manifest**: Application manifest for About dialog

### Scripts (`scripts/`)
- **build_all.bat**: Complete build automation
  - Compiles resources
  - Builds DLL and EXE
  - Outputs to build/ directory

- **register_sparse_package.bat**: Installation script
  - Registers MSIX sparse package
  - Sets up COM integration
  - Requires Administrator privileges

- **unregister_package.bat**: Uninstallation script

- **create_icon.ps1**: Icon generation utility

### Build Output (`build/`)
- **ShellMenuExtender.dll**: Main COM server
- **AppInfo.exe**: About dialog
- *.obj, *.lib, *.exp: Intermediate files (auto-cleaned)

## Key Features of Organization

### Separation of Concerns
- Source code separated from resources
- Build outputs isolated in build/
- Configuration externalized to config/
- Scripts centralized for easy access

### Build System
- All paths are relative to project root
- Scripts navigate to correct directories
- Build output goes to build/
- Intermediate files are cleaned up

### Configuration Loading
- DLL loads from: `<project-root>/config/menu_config.json`
- Icons load from: `<project-root>/resources/icons/`
- Relative paths in JSON resolve from DLL location

### MSIX Integration
- Package references files in build/ and resources/
- ExternalLocation allows files to stay in development folders
- No file copying needed during registration

## Development Workflow

### Initial Setup
1. Clone/download project
2. Run `scripts\build_all.bat`
3. Run `scripts\register_sparse_package.bat` (as Admin)

### Modifying Code
1. Edit files in `src/`
2. Run `scripts\build_all.bat`
3. Run `scripts\register_sparse_package.bat`

### Modifying Configuration
1. Edit `config\menu_config.json`
2. Right-click in Explorer (changes apply immediately)
3. No rebuild or re-registration needed

### Adding Icons
1. Add icon to `resources\icons/`
2. Reference in menu_config.json as `./resources/icons/filename.ico`
3. Hot-reload applies immediately

## Clean Project State

### Gitignored Files
- build/* (all build outputs)
- *.log (build logs)
- *.obj, *.res, *.lib, *.exp (intermediate files)
- microsoft.system.package.metadata/ (Windows metadata)

### Committed Files
- All source code (src/)
- All resources (resources/)
- All scripts (scripts/)
- Configuration template (config/)
- Package manifests (package/)
- Documentation (README.md)

## Benefits of This Structure

1. **Clear Separation**: Easy to find what you're looking for
2. **Scalability**: Easy to add new source files or resources
3. **Build Isolation**: Build artifacts don't pollute source
4. **Version Control**: Gitignore keeps repo clean
5. **Hot-Reload**: Config changes without rebuild
6. **Professional**: Industry-standard project organization
