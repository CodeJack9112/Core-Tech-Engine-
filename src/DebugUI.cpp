#include "../include/DebugUI.h"
#ifdef IMGUI_ENABLED
#include "../include/imgui_stub.h"
#endif
#include <iostream>

namespace DebugUI {

void showPCGControls(unsigned int& seed, bool& regenRequested){
#ifdef IMGUI_ENABLED
    ImGui::InputScalar("Seed", ImGuiDataType_U32, &seed);
    if(ImGui::Button("Regenerate")) regenRequested = true;
#else
    (void)seed; (void)regenRequested;
#endif
}

void showNavMesh(const NavMesh& nm){
#ifdef IMGUI_ENABLED
    if(ImGui::CollapsingHeader("NavMesh")){
        ImGui::Text("Nodes: %d", (int)nm.nodes().size());
    }
#else
    std::cout << "NavMesh nodes: " << nm.nodes().size() << std::endl;
#endif
}

void showPickupLog(const PickupManager& pm){
#ifdef IMGUI_ENABLED
    if(ImGui::CollapsingHeader("Pickups")){
        ImGui::Text("Collected: %d", pm.collected());
    }
#else
    std::cout << "Pickups collected: " << pm.collected() << std::endl;
#endif
}

} // namespace DebugUI
