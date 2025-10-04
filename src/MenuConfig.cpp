#include "MenuConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// External symbol for current module base
extern "C" IMAGE_DOS_HEADER __ImageBase;

// Simple JSON parser for our specific needs
class SimpleJSON {
public:
    static std::wstring GetString(const std::wstring& json, const std::wstring& key) {
        std::wstring searchKey = L"\"" + key + L"\":";
        size_t pos = json.find(searchKey);
        if (pos == std::wstring::npos) return L"";
        
        // Find the opening quote of the value
        pos = json.find(L'"', pos + searchKey.length());
        if (pos == std::wstring::npos) return L"";
        
        // Find the closing quote, skipping escaped quotes
        size_t endPos = pos + 1;
        while (endPos < json.length()) {
            if (json[endPos] == L'"') {
                // Check if it's escaped by counting preceding backslashes
                int backslashCount = 0;
                size_t checkPos = endPos - 1;
                while (checkPos > pos && json[checkPos] == L'\\') {
                    backslashCount++;
                    checkPos--;
                }
                // If even number of backslashes (or zero), the quote is not escaped
                if (backslashCount % 2 == 0) {
                    break;  // Found the real closing quote
                }
            }
            endPos++;
        }
        
        if (endPos >= json.length()) return L"";
        
        // Extract the string and unescape it
        std::wstring result = json.substr(pos + 1, endPos - pos - 1);
        
        // Unescape: replace \" with "
        size_t escapePos = 0;
        while ((escapePos = result.find(L"\\\"", escapePos)) != std::wstring::npos) {
            result.replace(escapePos, 2, L"\"");
            escapePos += 1;
        }
        
        return result;
    }
};

MenuConfig& MenuConfig::Instance() {
    static MenuConfig instance;
    return instance;
}

bool MenuConfig::Load(const std::wstring& configPath) {
    lastConfigPath = configPath;
    std::wifstream file(configPath);
    if (!file.is_open()) return false;
    
    std::wstringstream buffer;
    buffer << file.rdbuf();
    std::wstring json = buffer.str();
    
    menuItems.clear();
    
    // Find the menuItems array
    size_t menuItemsPos = json.find(L"\"menuItems\"");
    if (menuItemsPos == std::wstring::npos) return false;
    
    // Find the opening bracket of the array
    size_t arrayStart = json.find(L'[', menuItemsPos);
    if (arrayStart == std::wstring::npos) return false;
    
    // Find the MATCHING closing bracket (with depth tracking)
    size_t arrayEnd = arrayStart;
    int bracketDepth = 0;
    bool inStringBracket = false;
    wchar_t prevCharBracket = 0;
    
    for (size_t i = arrayStart; i < json.length(); i++) {
        wchar_t c = json[i];
        
        if (c == L'"' && prevCharBracket != L'\\') {
            inStringBracket = !inStringBracket;
        }
        
        if (!inStringBracket) {
            if (c == L'[') {
                bracketDepth++;
            } else if (c == L']') {
                bracketDepth--;
                if (bracketDepth == 0) {
                    arrayEnd = i;
                    break;
                }
            }
        }
        
        prevCharBracket = c;
    }
    
    if (arrayEnd == arrayStart) return false;
    
    // Extract array content
    std::wstring arrayContent = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
    
    // Parse individual items
    int braceDepth = 0;
    size_t itemStart = 0;
    bool inString = false;
    wchar_t prevChar = 0;
    
    for (size_t i = 0; i < arrayContent.length(); i++) {
        wchar_t c = arrayContent[i];
        
        if (c == L'"' && prevChar != L'\\') {
            inString = !inString;
        }
        
        if (!inString) {
            if (c == L'{') {
                if (braceDepth == 0) {
                    itemStart = i;
                }
                braceDepth++;
            }
            else if (c == L'}') {
                braceDepth--;
                if (braceDepth == 0) {
                    std::wstring itemJson = arrayContent.substr(itemStart, i - itemStart + 1);
                    
                    MenuItem item;
                    item.id = SimpleJSON::GetString(itemJson, L"id");
                    item.title = SimpleJSON::GetString(itemJson, L"title");
                    
                    // Get icon and expand relative paths
                    std::wstring iconPath = SimpleJSON::GetString(itemJson, L"icon");
                    if (!iconPath.empty() && iconPath[0] == L'.') {
                        // Relative path - resolve from DLL directory
                        WCHAR dllPath[MAX_PATH];
                        GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
                        WCHAR* lastSlash = wcsrchr(dllPath, L'\\');
                        if (lastSlash) {
                            *(lastSlash + 1) = L'\0';
                            std::wstring fullPath = dllPath;
                            // Remove ./ or .\\ prefix
                            if (iconPath.length() >= 2 && (iconPath.substr(0, 2) == L"./" || iconPath.substr(0, 2) == L".\\")) {
                                iconPath = iconPath.substr(2);
                            }
                            // Replace / with backslash
                            for (size_t j = 0; j < iconPath.length(); j++) {
                                if (iconPath[j] == L'/') iconPath[j] = L'\\';
                            }
                            fullPath += iconPath;
                            item.icon = fullPath;
                        } else {
                            item.icon = iconPath;
                        }
                    } else {
                        item.icon = iconPath;
                    }
                    
                    item.command = SimpleJSON::GetString(itemJson, L"command");
                    item.args = SimpleJSON::GetString(itemJson, L"args");
                    item.workingDir = SimpleJSON::GetString(itemJson, L"workingDir");
                    
                    // Parse showWindow
                    std::wstring showWin = SimpleJSON::GetString(itemJson, L"showWindow");
                    if (showWin == L"hidden") item.showWindow = SW_HIDE;
                    else if (showWin == L"minimized") item.showWindow = SW_MINIMIZE;
                    else if (showWin == L"maximized") item.showWindow = SW_MAXIMIZE;
                    else item.showWindow = SW_SHOWNORMAL;
                    
                    if (!item.title.empty() && !item.id.empty()) {
                        menuItems.push_back(item);
                    }
                }
            }
        }
        
        prevChar = c;
    }
    
    return !menuItems.empty();
}

void MenuConfig::Reload() {
    if (!lastConfigPath.empty()) {
        Load(lastConfigPath);
    }
}

std::wstring MenuConfig::ResolveIconPath(const std::wstring& iconPath) {
    // If it's an absolute path or a system reference (like "powershell.exe,0"), return as-is
    if (iconPath.empty() || 
        iconPath.find(L":") != std::wstring::npos ||  // Has drive letter
        iconPath.find(L".exe,") != std::wstring::npos || // System icon reference
        iconPath.find(L".dll,") != std::wstring::npos) { // DLL icon reference
        return iconPath;
    }
    
    // It's a relative path - resolve it from project root
    WCHAR dllPath[MAX_PATH];
    GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, MAX_PATH);
    
    // Remove DLL filename
    WCHAR* lastSlash = wcsrchr(dllPath, L'\\');
    if (lastSlash) {
        *lastSlash = L'\0';
    }
    
    // Remove "build" directory to get to project root
    lastSlash = wcsrchr(dllPath, L'\\');
    if (lastSlash) {
        *lastSlash = L'\0';
    }
    
    // Combine project root with relative icon path
    std::wstring absolutePath = dllPath;
    absolutePath += L"\\";
    absolutePath += iconPath;
    
    return absolutePath;
}

std::wstring MenuConfig::ProcessArgs(const std::wstring& args, const std::wstring& targetPath) {
    std::wstring result = args;
    
    // Replace {path} with target path
    size_t pos = 0;
    while ((pos = result.find(L"{path}", pos)) != std::wstring::npos) {
        result.replace(pos, 6, targetPath);
        pos += targetPath.length();
    }
    
    // Replace {wsl_path} with WSL-style path
    pos = 0;
    while ((pos = result.find(L"{wsl_path}", pos)) != std::wstring::npos) {
        std::wstring wslPath = ConvertToWSLPath(targetPath);
        result.replace(pos, 10, wslPath);
        pos += wslPath.length();
    }
    
    return result;
}

std::wstring MenuConfig::ConvertToWSLPath(const std::wstring& windowsPath) {
    std::wstring result = windowsPath;
    
    // Convert C:\Path to /mnt/c/Path
    if (result.length() >= 3 && result[1] == L':' && result[2] == L'\\') {
        wchar_t drive = towlower(result[0]);
        result = L"/mnt/" + std::wstring(1, drive) + L"/" + result.substr(3);
        
        // Replace \ with /
        for (size_t i = 0; i < result.length(); i++) {
            if (result[i] == L'\\') result[i] = L'/';
        }
    }
    
    return result;
}
