 #include "../include/EditorInspector.h"
#include "../include/VisualScripting.h"
#include "../include/LauncherImGui.h"
#include "../include/imgui_stub.h"
#include <vector>
#include <string>
#include <fstream>

// Simple ImGui node editor - very minimal: lists nodes, allows saving/loading graphs.
// Uses ImGui stub if ImGui not present; in that case, it will be a no-op but compile.

namespace VSEditor {
    bool showEditor(VisualScripting::Graph& graph){
#ifdef IMGUI_ENABLED
        ImGui::Text("Visual Scripting Editor (ImGui)");
        if(ImGui::Button("Add Print Node")){
            auto n = VisualScripting::createPrintNode("Hello from Editor");
            graph.nodes.push_back(n);
        }
        if(ImGui::Button("Save Graph")){
            std::string out = graph.serialize();
            std::ofstream f("demo/visual_graph.json"); f << out; f.close();
        }
        if(ImGui::Button("Load Graph")){
            std::ifstream f("demo/visual_graph.json"); if(f){ std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); graph = VisualScripting::Graph::deserialize(s); }
        }
        ImGui::Separator();
        if(ImGui::Button("Undo")){ /* placeholder */ }
        ImGui::SameLine(); if(ImGui::Button("Redo")){ /* placeholder */ }
        ImGui::Separator();

        for(auto &n : graph.nodes){
            if(ImGui::CollapsingHeader(n->title.c_str())){
                ImGui::Text("Type: %s", n->type.c_str());
            }
        }
        return true;
#else
        // If ImGui not enabled, do nothing and return false to indicate no UI shown.
        (void)graph; return false;
#endif
    }
}
