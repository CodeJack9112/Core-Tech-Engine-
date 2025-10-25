// ProfilerOverlay.cpp
// Lightweight ImGui profiler overlay stub.
// Drop this into your build (example: add to your CMake target sources) and hook
// DebugUI::RegisterOverlay / call ProfilerOverlay::Draw() from your debug rendering pass.
//
// This is intentionally self-contained and minimal — it does not change other engine files.

#include "imgui.h"
#include <chrono>
#include <vector>
#include <string>
#include <atomic>

namespace CoreTechTools {

struct Sample {
    const char* name;
    double ms;
};

static std::vector<Sample> g_samples;
static std::atomic<double> g_frame_time_ms(0.0);

void UpdateFrameTime(double ms) {
    g_frame_time_ms.store(ms, std::memory_order_relaxed);
}

void AddSample(const char* name, double ms) {
    g_samples.push_back({name, ms});
}

void ClearSamples() {
    g_samples.clear();
}

void DrawProfilerOverlay(bool* p_open) {
    if (!ImGui::Begin("CoreTech Profiler", p_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }

    double frame_ms = g_frame_time_ms.load(std::memory_order_relaxed);
    ImGui::Text("Frame: %.2f ms (%.1f FPS)", frame_ms, frame_ms > 0.0 ? 1000.0/frame_ms : 0.0f);
    if (ImGui::CollapsingHeader("Samples")) {
        for (auto &s: g_samples) {
            ImGui::BulletText("%s: %.3f ms", s.name, s.ms);
        }
    }

    if (ImGui::Button("Clear Samples")) ClearSamples();
    ImGui::End();
}

} // namespace CoreTechTools
