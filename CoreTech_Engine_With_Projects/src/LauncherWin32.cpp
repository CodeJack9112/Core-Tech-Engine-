
#ifdef _WIN32
#include "../include/Launcher.h"
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

static const char* g_renderOptions[] = { "OpenGL", "DirectX11" };
static const char* g_qualityOptions[] = { "Low", "Medium", "High" };

// Helper to write config
bool Launcher::writeConfig(const std::string& cfgPath, const std::string& renderAPI, int width, int height, bool vsync, int quality){
    std::ofstream cfg(cfgPath);
    if(!cfg) return false;
    cfg << "render_api=" << renderAPI << std::endl;
    cfg << "width=" << width << std::endl;
    cfg << "height=" << height << std::endl;
    cfg << "vsync=" << (vsync ? "1" : "0") << std::endl;
    cfg << "quality=" << quality << std::endl;
    cfg.close();
    return true;
}

bool showDialogModal(HWND hwndParent, const std::string& cfgPath){
    // Create a simple window with controls
    HINSTANCE hInst = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[]  = L"CoreTechLauncherClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc   = DefWindowProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"CoreTech Launcher", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 240, hwndParent, NULL, hInst, NULL);
    if(!hwnd) return false;

    // Create controls: static text, combo box for render API, resolution inputs, vsync checkbox, quality combo, and buttons
    HWND hRenderLabel = CreateWindowW(L"STATIC", L"Renderer:", WS_VISIBLE | WS_CHILD, 10, 10, 100, 20, hwnd, NULL, hInst, NULL);
    HWND hRenderCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 120, 10, 150, 200, hwnd, NULL, hInst, NULL);
    SendMessageW(hRenderCombo, CB_ADDSTRING, 0, (LPARAM)L"OpenGL");
    SendMessageW(hRenderCombo, CB_ADDSTRING, 0, (LPARAM)L"DirectX11");
    SendMessageW(hRenderCombo, CB_SETCURSEL, 0, 0);

    HWND hResLabel = CreateWindowW(L"STATIC", L"Resolution (W x H):", WS_VISIBLE | WS_CHILD, 10, 50, 150, 20, hwnd, NULL, hInst, NULL);
    HWND hWidth = CreateWindowW(L"EDIT", L"1280", WS_CHILD | WS_VISIBLE | WS_BORDER, 170, 50, 80, 22, hwnd, NULL, hInst, NULL);
    HWND hHeight = CreateWindowW(L"EDIT", L"720", WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 50, 80, 22, hwnd, NULL, hInst, NULL);

    HWND hVsync = CreateWindowW(L"BUTTON", L"VSync", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 90, 100, 22, hwnd, NULL, hInst, NULL);
    SendMessageW(hVsync, BM_SETCHECK, BST_CHECKED, 0);

    HWND hQualityLabel = CreateWindowW(L"STATIC", L"Quality:", WS_VISIBLE | WS_CHILD, 10, 130, 100, 20, hwnd, NULL, hInst, NULL);
    HWND hQualityCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 120, 130, 150, 200, hwnd, NULL, hInst, NULL);
    SendMessageW(hQualityCombo, CB_ADDSTRING, 0, (LPARAM)L"Low");
    SendMessageW(hQualityCombo, CB_ADDSTRING, 0, (LPARAM)L"Medium");
    SendMessageW(hQualityCombo, CB_ADDSTRING, 0, (LPARAM)L"High");
    SendMessageW(hQualityCombo, CB_SETCURSEL, 2, 0);

    HWND hBtnOk = CreateWindowW(L"BUTTON", L"OK", WS_VISIBLE | WS_CHILD, 80, 170, 100, 30, hwnd, NULL, hInst, NULL);
    HWND hBtnCancel = CreateWindowW(L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 200, 170, 100, 30, hwnd, NULL, hInst, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Simple message loop; handle button clicks
    bool done = false; bool ok = false;
    MSG msg;
    while(!done && GetMessage(&msg, NULL, 0, 0)){
        if(msg.message == WM_COMMAND){
            WPARAM wp = msg.wParam;
            HWND sender = (HWND)msg.lParam;
            if(sender == hBtnOk){
                // read values and write config
                int sel = (int)SendMessageW(hRenderCombo, CB_GETCURSEL, 0, 0);
                wchar_t buf[64];
                GetWindowTextW(hWidth, buf, 64); int width = _wtoi(buf);
                GetWindowTextW(hHeight, buf, 64); int height = _wtoi(buf);
                LRESULT vs = SendMessageW(hVsync, BM_GETCHECK, 0, 0);
                int qsel = (int)SendMessageW(hQualityCombo, CB_GETCURSEL, 0, 0);
                std::wstring renderW;
                if(sel==0) renderW = L"OpenGL"; else renderW = L"DirectX11";
                // convert to std::string UTF-8 simple
                std::wstring ws = renderW;
                std::string renderAPI(ws.begin(), ws.end());
                Launcher::writeConfig(cfgPath, renderAPI, width, height, vs==BST_CHECKED, qsel);
                ok = true; done = true;
            } else if(sender == hBtnCancel){
                done = true; ok = false;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DestroyWindow(hwnd);
    UnregisterClass(CLASS_NAME, hInst);
    return ok;
}

bool Launcher::showGUI(const std::string& cfgPath){
    // initialize COM for WIC etc.
    CoInitialize(NULL);
    bool res = showDialogModal(NULL, cfgPath);
    CoUninitialize();
    return res;
}

void Launcher::showConsole(const std::string& cfgPath){
    std::cout << "CoreTech Launcher (Console)\n";
    std::cout << "1) OpenGL\n2) DirectX11\nChoose renderer (1/2) or Enter to keep current: ";
    std::string choice; std::getline(std::cin, choice);
    std::string api = "OpenGL";
    if(choice == "2") api = "DirectX11";
    // default settings
    Launcher::writeConfig(cfgPath, api, 1280, 720, true, 2);
}

#else // non-Windows stubs

#include "../include/Launcher.h"
#include <iostream>
#include <fstream>

bool Launcher::showGUI(const std::string& cfgPath){
    // Not supported - fallback to console
    showConsole(cfgPath);
    return true;
}

void Launcher::showConsole(const std::string& cfgPath){
    std::cout << "CoreTech Launcher (Console)\n";
    std::cout << "1) OpenGL\n2) DirectX11\nChoose renderer (1/2) or Enter to keep current: ";
    std::string choice; std::getline(std::cin, choice);
    std::string api = "OpenGL";
    if(choice == "2") api = "DirectX11";
    Launcher::writeConfig(cfgPath, api, 1280, 720, true, 2);
}

bool Launcher::writeConfig(const std::string& cfgPath, const std::string& renderAPI, int width, int height, bool vsync, int quality){
    std::ofstream cfg(cfgPath);
    if(!cfg) return false;
    cfg << "render_api=" << renderAPI << std::endl;
    cfg << "width=" << width << std::endl;
    cfg << "height=" << height << std::endl;
    cfg << "vsync=" << (vsync ? "1" : "0") << std::endl;
    cfg << "quality=" << quality << std::endl;
    return true;
}

#endif
