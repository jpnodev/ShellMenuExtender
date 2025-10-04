#include "ShellMenuExtender.h"
#include "MenuConfig.h"
#include <shobjidl_core.h>
#include <shlguid.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <exdisp.h>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

// External symbol for current module base
extern "C" IMAGE_DOS_HEADER __ImageBase;

// {12345678-1234-1234-1234-123456789012} - Replace with your own GUID
static const CLSID CLSID_ShellMenuExtender = 
    { 0x12345678, 0x1234, 0x1234, { 0x12, 0x34, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12 } };

static LONG g_refModule = 0;

ShellMenuExtender::ShellMenuExtender() : m_refCount(1) {
    // Get user's Documents folder
    WCHAR documentsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath))) {
        std::wstring configDir = documentsPath;
        configDir += L"\\ShellMenuExtender";
        
        // Create directory if it doesn't exist
        CreateDirectoryW(configDir.c_str(), NULL);
        
        std::wstring configPath = configDir + L"\\menu_config.json";
        
        // Copy default config if user config doesn't exist
        if (GetFileAttributesW(configPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            // Get installation directory
            WCHAR dllPath[MAX_PATH];
            GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
            
            // Remove DLL filename
            WCHAR* lastSlash = wcsrchr(dllPath, L'\\');
            if (lastSlash) *lastSlash = L'\0';
            
            // Remove "build" directory to get project root
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

HRESULT __stdcall ShellMenuExtender::QueryInterface(REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IExplorerCommand) {
        *ppv = static_cast<IExplorerCommand*>(this);
        AddRef();
        return S_OK;
    }
    *ppv = nullptr;
    return E_NOINTERFACE;
}

ULONG __stdcall ShellMenuExtender::AddRef() { return InterlockedIncrement(&m_refCount); }
ULONG __stdcall ShellMenuExtender::Release() {
    ULONG res = InterlockedDecrement(&m_refCount);
    if (res == 0) delete this;
    return res;
}

HRESULT __stdcall ShellMenuExtender::GetTitle(IShellItemArray*, PWSTR* title) {
    // Reload config on each menu open for instant updates
    MenuConfig::Instance().Reload();
    
    const auto& items = MenuConfig::Instance().GetMenuItems();
    
    // DEBUG: Show item count in title
    std::wstring menuTitle;
    if (items.size() > 1) {
        menuTitle = L"Shell Menu Extender (" + std::to_wstring(items.size()) + L" items)";
    } else if (items.size() == 1) {
        menuTitle = items[0].title + L" (1 item)";
    } else {
        menuTitle = L"Shell Menu Extender (0 items - config failed)";
    }
    
    size_t len = menuTitle.length() + 1;
    *title = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * len);
    if (*title) {
        wcscpy_s(*title, len, menuTitle.c_str());
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT __stdcall ShellMenuExtender::GetIcon(IShellItemArray*, PWSTR* icon) {
    const auto& items = MenuConfig::Instance().GetMenuItems();
    std::wstring iconRef;
    
    // For multiple items (parent menu), use logo.png; for single item, use its icon
    if (items.size() > 1) {
        // Parent menu - use logo from resources
        WCHAR dllPath[MAX_PATH];
        GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
        
        // First, remove the DLL filename
        WCHAR* lastSlash = wcsrchr(dllPath, L'\\');
        if (lastSlash) {
            *lastSlash = L'\0';  // Null terminate to remove filename
        }
        
        // Now remove "build" directory to get to project root
        lastSlash = wcsrchr(dllPath, L'\\');
        if (lastSlash) {
            *lastSlash = L'\0';  // Null terminate to remove "build"
        }
        
        iconRef = dllPath;
        iconRef += L"\\resources\\icons\\logo.png";
    } else if (!items.empty() && !items[0].icon.empty()) {
        iconRef = items[0].icon;
    } else {
        // Fallback to embedded icon
        WCHAR dllPath[MAX_PATH];
        GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
        iconRef = dllPath;
        iconRef += L",-1";
    }
    
    size_t len = iconRef.length() + 1;
    *icon = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * len);
    if (*icon) {
        wcscpy_s(*icon, len, iconRef.c_str());
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT __stdcall ShellMenuExtender::GetToolTip(IShellItemArray*, PWSTR* tip) {
    *tip = nullptr;
    return E_NOTIMPL;
}

HRESULT __stdcall ShellMenuExtender::GetCanonicalName(GUID* guid) {
    return E_NOTIMPL;
}

HRESULT __stdcall ShellMenuExtender::GetState(IShellItemArray* items, BOOL fOkToBeSlow, EXPCMDSTATE* cmdState) {
    // Fast path: always enable without checking items
    *cmdState = ECS_ENABLED;
    return S_OK;
}

HRESULT __stdcall ShellMenuExtender::Invoke(IShellItemArray* items, IBindCtx*) {
    // If only one item, execute it directly
    const auto& menuItems = MenuConfig::Instance().GetMenuItems();
    if (menuItems.size() == 1) {
        std::wstring targetPath;
        
        // Get target path (same logic as before)
        if (items) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(items->GetItemAt(0, &item))) {
                PWSTR pszPath = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)) && pszPath) {
                    targetPath = pszPath;
                    CoTaskMemFree(pszPath);
                }
                item->Release();
            }
        }
        
        if (targetPath.empty()) {
            IShellWindows* psw = nullptr;
            if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&psw)))) {
                VARIANT v;
                V_VT(&v) = VT_I4;
                V_I4(&v) = 0;
                
                IDispatch* pdisp = nullptr;
                if (SUCCEEDED(psw->Item(v, &pdisp))) {
                    IWebBrowserApp* pwba = nullptr;
                    if (SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pwba)))) {
                        BSTR bstr = nullptr;
                        if (SUCCEEDED(pwba->get_LocationURL(&bstr)) && bstr) {
                            WCHAR szPath[MAX_PATH];
                            DWORD cchPath = MAX_PATH;
                            if (SUCCEEDED(PathCreateFromUrlW(bstr, szPath, &cchPath, 0))) {
                                targetPath = szPath;
                            }
                            SysFreeString(bstr);
                        }
                        pwba->Release();
                    }
                    pdisp->Release();
                }
                psw->Release();
            }
        }
        
        if (!targetPath.empty()) {
            const MenuItem& item = menuItems[0];
            std::wstring processedArgs = MenuConfig::Instance().ProcessArgs(item.args, targetPath);
            const WCHAR* workDir = item.workingDir.empty() ? nullptr : item.workingDir.c_str();
            ShellExecuteW(nullptr, L"open", item.command.c_str(), processedArgs.c_str(), 
                          workDir, item.showWindow);
        }
    }
    // If multiple items, they're handled via subcommands - this shouldn't be called
    return S_OK;
}

HRESULT __stdcall ShellMenuExtender::GetFlags(EXPCMDFLAGS* flags) {
    const auto& items = MenuConfig::Instance().GetMenuItems();
    // If multiple items, show as a dropdown
    *flags = (items.size() > 1) ? ECF_HASSUBCOMMANDS : ECF_DEFAULT;
    return S_OK;
}

HRESULT __stdcall ShellMenuExtender::EnumSubCommands(IEnumExplorerCommand** enumCommands) {
    const auto& items = MenuConfig::Instance().GetMenuItems();
    
    // Only provide subcommands if we have multiple items
    if (items.size() > 1) {
        SubCommandEnumerator* enumerator = new (std::nothrow) SubCommandEnumerator((int)items.size());
        if (!enumerator) return E_OUTOFMEMORY;
        
        *enumCommands = enumerator;
        return S_OK;
    }
    
    *enumCommands = nullptr;
    return E_NOTIMPL;
}

// Class Factory
class ClassFactory : public IClassFactory {
    LONG m_refCount;
public:
    ClassFactory() : m_refCount(1) { InterlockedIncrement(&g_refModule); }
    ~ClassFactory() { InterlockedDecrement(&g_refModule); }

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG __stdcall AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG __stdcall Release() override {
        ULONG res = InterlockedDecrement(&m_refCount);
        if (res == 0) delete this;
        return res;
    }

    HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) override {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        ShellMenuExtender* pObj = new (std::nothrow) ShellMenuExtender();
        if (!pObj) return E_OUTOFMEMORY;
        HRESULT hr = pObj->QueryInterface(riid, ppv);
        pObj->Release();
        return hr;
    }

    HRESULT __stdcall LockServer(BOOL fLock) override {
        if (fLock) InterlockedIncrement(&g_refModule);
        else InterlockedDecrement(&g_refModule);
        return S_OK;
    }
};

// COM DLL exports
extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    if (rclsid == CLSID_ShellMenuExtender) {
        ClassFactory* pFactory = new (std::nothrow) ClassFactory();
        if (!pFactory) return E_OUTOFMEMORY;
        HRESULT hr = pFactory->QueryInterface(riid, ppv);
        pFactory->Release();
        return hr;
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" HRESULT __stdcall DllCanUnloadNow() {
    return (g_refModule == 0) ? S_OK : S_FALSE;
}

extern "C" HRESULT __stdcall DllRegisterServer() {
    // TODO: Add registry entries for shell extension
    return S_OK;
}

extern "C" HRESULT __stdcall DllUnregisterServer() {
    // TODO: Remove registry entries for shell extension
    return S_OK;
}

// ============================================================================
// MenuItemCommand Implementation (Individual menu items)
// ============================================================================

MenuItemCommand::MenuItemCommand(int itemIndex) : m_refCount(1), m_itemIndex(itemIndex) {
    InterlockedIncrement(&g_refModule);
}

HRESULT __stdcall MenuItemCommand::QueryInterface(REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IExplorerCommand) {
        *ppv = static_cast<IExplorerCommand*>(this);
        AddRef();
        return S_OK;
    }
    *ppv = nullptr;
    return E_NOINTERFACE;
}

ULONG __stdcall MenuItemCommand::AddRef() { return InterlockedIncrement(&m_refCount); }
ULONG __stdcall MenuItemCommand::Release() {
    ULONG res = InterlockedDecrement(&m_refCount);
    if (res == 0) {
        InterlockedDecrement(&g_refModule);
        delete this;
    }
    return res;
}

HRESULT __stdcall MenuItemCommand::GetTitle(IShellItemArray*, PWSTR* title) {
    const auto& items = MenuConfig::Instance().GetMenuItems();
    if (m_itemIndex < 0 || m_itemIndex >= (int)items.size()) return E_FAIL;
    
    const MenuItem& item = items[m_itemIndex];
    size_t len = item.title.length() + 1;
    *title = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * len);
    if (*title) {
        wcscpy_s(*title, len, item.title.c_str());
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT __stdcall MenuItemCommand::GetIcon(IShellItemArray*, PWSTR* icon) {
    const auto& items = MenuConfig::Instance().GetMenuItems();
    if (m_itemIndex < 0 || m_itemIndex >= (int)items.size()) return E_FAIL;
    
    const MenuItem& item = items[m_itemIndex];
    std::wstring iconRef = item.icon.empty() ? L"shell32.dll,-4" : MenuConfig::Instance().ResolveIconPath(item.icon);
    
    size_t len = iconRef.length() + 1;
    *icon = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * len);
    if (*icon) {
        wcscpy_s(*icon, len, iconRef.c_str());
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

HRESULT __stdcall MenuItemCommand::GetToolTip(IShellItemArray*, PWSTR* tip) {
    *tip = nullptr;
    return E_NOTIMPL;
}

HRESULT __stdcall MenuItemCommand::GetCanonicalName(GUID* guid) {
    return E_NOTIMPL;
}

HRESULT __stdcall MenuItemCommand::GetState(IShellItemArray*, BOOL, EXPCMDSTATE* cmdState) {
    *cmdState = ECS_ENABLED;
    return S_OK;
}

HRESULT __stdcall MenuItemCommand::Invoke(IShellItemArray* items, IBindCtx*) {
    const auto& menuItems = MenuConfig::Instance().GetMenuItems();
    if (m_itemIndex < 0 || m_itemIndex >= (int)menuItems.size()) return E_FAIL;
    
    std::wstring targetPath;
    
    // Get target path
    if (items) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(items->GetItemAt(0, &item))) {
            PWSTR pszPath = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)) && pszPath) {
                targetPath = pszPath;
                CoTaskMemFree(pszPath);
            }
            item->Release();
        }
    }
    
    if (targetPath.empty()) {
        IShellWindows* psw = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&psw)))) {
            VARIANT v;
            V_VT(&v) = VT_I4;
            V_I4(&v) = 0;
            
            IDispatch* pdisp = nullptr;
            if (SUCCEEDED(psw->Item(v, &pdisp))) {
                IWebBrowserApp* pwba = nullptr;
                if (SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pwba)))) {
                    BSTR bstr = nullptr;
                    if (SUCCEEDED(pwba->get_LocationURL(&bstr)) && bstr) {
                        WCHAR szPath[MAX_PATH];
                        DWORD cchPath = MAX_PATH;
                        if (SUCCEEDED(PathCreateFromUrlW(bstr, szPath, &cchPath, 0))) {
                            targetPath = szPath;
                        }
                        SysFreeString(bstr);
                    }
                    pwba->Release();
                }
                pdisp->Release();
            }
            psw->Release();
        }
    }
    
    if (targetPath.empty()) return E_FAIL;
    
    // Execute command
    const MenuItem& menuItem = menuItems[m_itemIndex];
    std::wstring processedArgs = MenuConfig::Instance().ProcessArgs(menuItem.args, targetPath);
    const WCHAR* workDir = menuItem.workingDir.empty() ? nullptr : menuItem.workingDir.c_str();
    ShellExecuteW(nullptr, L"open", menuItem.command.c_str(), processedArgs.c_str(), 
                  workDir, menuItem.showWindow);
    
    return S_OK;
}

HRESULT __stdcall MenuItemCommand::GetFlags(EXPCMDFLAGS* flags) {
    *flags = ECF_DEFAULT;
    return S_OK;
}

HRESULT __stdcall MenuItemCommand::EnumSubCommands(IEnumExplorerCommand** enumCommands) {
    *enumCommands = nullptr;
    return E_NOTIMPL;
}

// ============================================================================
// SubCommandEnumerator Implementation
// ============================================================================

SubCommandEnumerator::SubCommandEnumerator(int count) 
    : m_refCount(1), m_currentIndex(0), m_totalCount(count) {
    InterlockedIncrement(&g_refModule);
}

HRESULT __stdcall SubCommandEnumerator::QueryInterface(REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IEnumExplorerCommand) {
        *ppv = static_cast<IEnumExplorerCommand*>(this);
        AddRef();
        return S_OK;
    }
    *ppv = nullptr;
    return E_NOINTERFACE;
}

ULONG __stdcall SubCommandEnumerator::AddRef() { return InterlockedIncrement(&m_refCount); }
ULONG __stdcall SubCommandEnumerator::Release() {
    ULONG res = InterlockedDecrement(&m_refCount);
    if (res == 0) {
        InterlockedDecrement(&g_refModule);
        delete this;
    }
    return res;
}

HRESULT __stdcall SubCommandEnumerator::Next(ULONG celt, IExplorerCommand** pUICommand, ULONG* pceltFetched) {
    ULONG fetched = 0;
    
    while (fetched < celt && m_currentIndex < m_totalCount) {
        MenuItemCommand* cmd = new (std::nothrow) MenuItemCommand(m_currentIndex);
        if (!cmd) return E_OUTOFMEMORY;
        
        pUICommand[fetched] = cmd;
        fetched++;
        m_currentIndex++;
    }
    
    if (pceltFetched) *pceltFetched = fetched;
    return (fetched == celt) ? S_OK : S_FALSE;
}

HRESULT __stdcall SubCommandEnumerator::Skip(ULONG celt) {
    m_currentIndex += celt;
    return (m_currentIndex <= m_totalCount) ? S_OK : S_FALSE;
}

HRESULT __stdcall SubCommandEnumerator::Reset() {
    m_currentIndex = 0;
    return S_OK;
}

HRESULT __stdcall SubCommandEnumerator::Clone(IEnumExplorerCommand** ppenum) {
    SubCommandEnumerator* clone = new (std::nothrow) SubCommandEnumerator(m_totalCount);
    if (!clone) return E_OUTOFMEMORY;
    
    clone->m_currentIndex = m_currentIndex;
    *ppenum = clone;
    return S_OK;
}
