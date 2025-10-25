#pragma once

#include <string>

namespace Launcher {
    // Show ImGui launcher. If ImGui is not available, falls back to console launcher.
    // cfgPath is the path to the engine config file to write selected settings.
    bool showImGui(const std::string& cfgPath);
}
