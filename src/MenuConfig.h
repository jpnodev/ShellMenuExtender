#pragma once
#include <string>
#include <vector>
#include <windows.h>

struct MenuItem {
    std::wstring id;
    std::wstring title;
    std::wstring icon;
    std::wstring command;
    std::wstring args;
    std::wstring workingDir;
    int showWindow;
    std::vector<MenuItem> subItems;
};

class MenuConfig {
public:
    static MenuConfig& Instance();
    bool Load(const std::wstring& configPath);
    void Reload();  // Force reload from disk
    const std::vector<MenuItem>& GetMenuItems() const { return menuItems; }
    std::wstring ProcessArgs(const std::wstring& args, const std::wstring& windowsPath);
    std::wstring ResolveIconPath(const std::wstring& iconPath);

private:
    MenuConfig() = default;
    std::vector<MenuItem> menuItems;
    std::wstring lastConfigPath;
    std::wstring ConvertToWSLPath(const std::wstring& windowsPath);
};
