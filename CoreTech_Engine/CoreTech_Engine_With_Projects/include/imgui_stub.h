
#pragma once
// Minimal ImGui stub for builds without real ImGui available.
// If you have real ImGui, replace this file with the actual ImGui headers and disable this stub.
#ifndef IMGUI_ENABLED
#define IMGUI_ENABLED 0
#endif
#if IMGUI_ENABLED
// include real imgui in your project
#include <imgui.h>
#else
#include <cstdarg>
#include <string>
namespace ImGui {
    inline void InputScalar(const char*, int, unsigned int*) {}
    inline bool Button(const char*) { return false; }
    inline bool CollapsingHeader(const char*) { return false; }
    inline void Text(const char*, ...) {}
    inline bool InputFloat(const char*, float*) { return false; }
    inline bool InputInt(const char*, int*) { return false; }
    inline bool InputText(const char*, char*, size_t) { return false; }
    inline bool SmallButton(const char*) { return false; }
    inline bool Begin(const char*, bool*, int) { return false; }
    inline void End() {}
    inline void Columns(int) {}
    inline void BeginChild(const char*, const struct ImVec2&, bool) {}
    inline void NextColumn() {}
    inline bool IsItemHovered() { return false; }
    inline bool IsMouseClicked(int) { return false; }
    inline bool IsMouseDragging(int) { return false; }
    inline bool IsMouseReleased(int) { return false; }
    inline ImVec2 GetCursorScreenPos() { return ImVec2(); }
    inline struct ImDrawList* GetWindowDrawList() { return nullptr; }
    struct ImVec2 { float x=0,y=0; ImVec2(){} ImVec2(float _x,float _y):x(_x),y(_y){} };
    struct ImDrawList { void AddRectFilled(ImVec2, ImVec2, unsigned int, float=0){} void AddText(ImVec2, unsigned int, const char*){} void AddCircleFilled(ImVec2, float, unsigned int){} void AddBezierCurve(ImVec2, ImVec2, ImVec2, ImVec2, unsigned int, float){} };
    inline void OpenPopup(const char*) {}
    inline bool BeginPopup(const char*) { return false; }
    inline void EndPopup() {}
    inline bool MenuItem(const char*) { return false; }
    inline bool IsItemActive() { return false; }
    inline ImVec2 GetIO_MousePos() { return ImVec2(); }
    inline struct ImGuiIO { ImVec2 MousePos; ImVec2 MouseDelta; } GetIO() { return ImGuiIO(); }
}
#endif
