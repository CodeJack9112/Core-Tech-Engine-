\
#pragma once
// Minimal single-file ImGui-compatible stub (provides a subset of the API used by DebugUI)
// This is a lightweight implementation that allows the project to compile and the DebugUI calls to be no-ops.
// Replace with real Dear ImGui for full UI functionality.

#include <cstdint>

namespace ImGui {
    using ImGuiID = uint32_t;
    inline void InputScalar(const char* label, int type, unsigned int* data) { (void)label; (void)type; (void)data; }
    inline bool Button(const char* label) { (void)label; return false; }
    inline bool CollapsingHeader(const char* label) { (void)label; return false; }
    inline void Text(const char* fmt, ...) { (void)fmt; }
    inline void InputInt(const char* label, int* v) { (void)label; (void)v; }
}

