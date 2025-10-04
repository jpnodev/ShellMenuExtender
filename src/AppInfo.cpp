#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <string>
#include <fstream>
#include <sstream>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Control IDs
#define ID_CLOSE_BUTTON 101

// Modern colors
#define COLOR_BG RGB(243, 243, 243)

std::wstring LoadConfigPath() {
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    
    // Remove filename
    WCHAR* lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    
    // Remove "build" directory
    lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    
    std::wstring configPath = exePath;
    configPath += L"\\config\\menu_config.json";
    return configPath;
}

int CountMenuItems() {
    std::wstring configPath = LoadConfigPath();
    std::wifstream file(configPath);
    if (!file.is_open()) return 0;
    
    std::wstringstream buffer;
    buffer << file.rdbuf();
    std::wstring json = buffer.str();
    
    int count = 0;
    size_t pos = 0;
    while ((pos = json.find(L"\"id\":", pos)) != std::wstring::npos) {
        count++;
        pos += 5;
    }
    return count;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Enable modern styling
            SetWindowTheme(hwnd, L"Explorer", NULL);
            
            // Apply rounded corners (Windows 11)
            DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
            DwmSetWindowAttribute(hwnd, 33, &corner, sizeof(corner));
            
            // Title - using large system font
            CreateWindowExW(0, L"STATIC", L"Shell Menu Extender",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 30, 500, 35,
                hwnd, NULL, NULL, NULL);
            
            // Set title font
            HWND hTitle = GetDlgItem(hwnd, -1);
            SendMessage(GetWindow(hwnd, GW_CHILD), WM_SETFONT, 
                (WPARAM)CreateFontW(28, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            // Version
            HWND hVersion = CreateWindowExW(0, L"STATIC", L"Version 2.0.0",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 75, 300, 25,
                hwnd, NULL, NULL, NULL);
            SendMessage(hVersion, WM_SETFONT, 
                (WPARAM)CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            // Description
            HWND hDesc = CreateWindowExW(0, L"STATIC", 
                L"Customizable context menu extension for Windows 11 File Explorer",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 110, 540, 40,
                hwnd, NULL, NULL, NULL);
            SendMessage(hDesc, WM_SETFONT, 
                (WPARAM)CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            // Group box for features
            HWND hGroup = CreateWindowExW(0, L"BUTTON", L"Instant Updates",
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                20, 170, 560, 140,
                hwnd, NULL, NULL, NULL);
            SendMessage(hGroup, WM_SETFONT, 
                (WPARAM)CreateFontW(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            // Feature items
            const wchar_t* features[] = {
                L"\u2022 Edit config\\menu_config.json - changes apply instantly",
                L"\u2022 No re-registration needed after config changes",
                L"\u2022 Multiple items create submenu automatically"
            };
            
            int yPos = 200;
            for (int i = 0; i < 3; i++) {
                HWND hFeature = CreateWindowExW(0, L"STATIC", features[i],
                    WS_CHILD | WS_VISIBLE | SS_LEFT,
                    40, yPos, 520, 25,
                    hwnd, NULL, NULL, NULL);
                SendMessage(hFeature, WM_SETFONT, 
                    (WPARAM)CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
                yPos += 30;
            }
            
            // Stats
            std::wstring stats = L"Active menu items: " + std::to_wstring(CountMenuItems());
            HWND hStats = CreateWindowExW(0, L"STATIC", stats.c_str(),
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 330, 540, 25,
                hwnd, NULL, NULL, NULL);
            SendMessage(hStats, WM_SETFONT, 
                (WPARAM)CreateFontW(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            // Config path
            std::wstring configInfo = L"Configuration: " + LoadConfigPath();
            HWND hConfig = CreateWindowExW(0, L"STATIC", configInfo.c_str(),
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 365, 540, 40,
                hwnd, NULL, NULL, NULL);
            SendMessage(hConfig, WM_SETFONT, 
                (WPARAM)CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            // Close button
            HWND hButton = CreateWindowExW(0, L"BUTTON", L"Close",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
                490, 420, 90, 32,
                hwnd, (HMENU)ID_CLOSE_BUTTON, NULL, NULL);
            SendMessage(hButton, WM_SETFONT, 
                (WPARAM)CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI"), TRUE);
            
            return 0;
        }
        
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(32, 32, 32));
            SetBkColor(hdcStatic, COLOR_BG);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        
        case WM_CTLCOLORBTN: {
            return (LRESULT)GetStockObject(WHITE_BRUSH);
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_CLOSE_BUTTON) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Enable DPI awareness for proper scaling
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    // Load icon from resources/icons folder
    WCHAR iconPath[MAX_PATH];
    GetModuleFileNameW(NULL, iconPath, MAX_PATH);
    WCHAR* lastSlash = wcsrchr(iconPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    lastSlash = wcsrchr(iconPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    wcscat_s(iconPath, L"\\resources\\icons\\logo.ico");
    
    // Register window class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(COLOR_BG);
    wc.lpszClassName = L"ShellMenuExtenderAbout";
    wc.hIcon = (HICON)LoadImageW(NULL, iconPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    if (!wc.hIcon) {
        // Fallback to default application icon
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    wc.hIconSm = wc.hIcon;
    
    if (!RegisterClassExW(&wc)) {
        return 0;
    }
    
    // Get DPI scaling
    HDC hdc = GetDC(NULL);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(NULL, hdc);
    float dpiScale = dpi / 96.0f;
    
    // Scale window size based on DPI
    int windowWidth = (int)(650 * dpiScale);
    int windowHeight = (int)(520 * dpiScale);
    
    // Calculate centered position
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    
    // Create window with proper styles
    HWND hwnd = CreateWindowExW(
        0,
        L"ShellMenuExtenderAbout",
        L"About - Shell Menu Extender",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) {
        return 0;
    }
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}
