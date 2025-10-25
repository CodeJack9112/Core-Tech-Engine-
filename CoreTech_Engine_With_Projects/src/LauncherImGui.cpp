
#include "../include/LauncherImGui.h"
#include "../include/Launcher.h"
#include "../include/ConfigLoader.h"
#include <iostream>
#include <fstream>
#include <string>

// If IMGUI_ENABLED is defined and real ImGui is available, this will use it.
// Otherwise it will print a console fallback and write config.
#include "../include/imgui_stub.h"

bool Launcher::showImGui(const std::string& cfgPath){
#ifdef IMGUI_ENABLED
    // Real ImGui UI path (user must integrate a backend: GLFW/SDL + OpenGL/DirectX). 
    // This function assumes a running ImGui frame loop is available in the application and will present settings.
    // Since we don't own the main loop here, we simulate a single-frame modal dialog for configuration.
    unsigned int seed = 0;
    // load existing config
    ConfigLoader cfg; cfg.load(cfgPath);
    std::string cur = cfg.get("render_api","OpenGL");
    int sel = (cur == "DirectX11") ? 1 : 0;
    int width = std::stoi(cfg.get("width","1280"));
    int height = std::stoi(cfg.get("height","720"));
    int vsync = std::stoi(cfg.get("vsync","1"));
    int quality = std::stoi(cfg.get("quality","2"));

    // Simulate ImGui controls using the stub API (no-op) so app compiles without real ImGui.
    ImGui::Text("CoreTech ImGui Launcher (stub)"); 
    // Inputs (no-op with stub)
    ImGui::Text("Renderer: %s", sel==0? "OpenGL":"DirectX11");
    ImGui::Text("Resolution: %d x %d", width, height);
    ImGui::Text("VSync: %d", vsync);
    ImGui::Text("Quality: %d", quality);
    // In real ImGui code you'd provide interactive widgets here and apply changes.
    // For now, keep existing config unchanged.
    return true;
#else
    // Fallback to console launcher
    std::cout << "ImGui not enabled; falling back to console launcher." << std::endl;
    Launcher::showConsole(cfgPath);
    return true;
#endif
}
