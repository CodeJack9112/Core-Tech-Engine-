#pragma once
#include <string>

// Simple launcher interface. On Windows a Win32 GUI dialog will be shown; otherwise fall back to console launcher.
namespace Launcher {
    // Show GUI launcher; returns true if user made a choice and config updated.
    bool showGUI(const std::string& cfgPath);

    // Show console launcher as fallback.
    void showConsole(const std::string& cfgPath);

    // helper to write config
    bool writeConfig(const std::string& cfgPath, const std::string& renderAPI, int width, int height, bool vsync, int quality);
}
