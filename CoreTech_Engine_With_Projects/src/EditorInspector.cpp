\
#include "../include/EditorInspector.h"
#include "../include/imgui_stub.h"
#include <iostream>
#include <cstring>

bool EditorInspector::showNodeInspector(VisualScripting::Graph& graph, const std::string& nodeId){
#ifdef IMGUI_ENABLED
    std::shared_ptr<VisualScripting::Node> nodePtr;
    for (auto &nptr : graph.nodesEditable) { if (nptr->id == nodeId) { nodePtr = nptr; break; } }
    if (!nodePtr) return false;
    if(ImGui::CollapsingHeader(nodePtr->title.c_str())){
        ImGui::Text("Type: %s", nodePtr->type.c_str());
        // show inputs
        for(auto &kv : nodePtr->inputs){
            if(kv.second.type == VisualScripting::Value::FLOAT){
                float v = kv.second.f;
                ImGui::InputFloat(kv.first.c_str(), &v);
                kv.second.f = v;
            } else if(kv.second.type == VisualScripting::Value::INT){
                int iv = kv.second.i;
                ImGui::InputInt(kv.first.c_str(), &iv);
                kv.second.i = iv;
            } else if(kv.second.type == VisualScripting::Value::STRING){
                // simple buffer
                char buf[256]; strncpy(buf, kv.second.s.c_str(), 255); buf[255]=0;
                ImGui::InputText(kv.first.c_str(), buf, 256);
                kv.second.s = std::string(buf);
            }
        }
    }
    return true;
#else
    // Not available - no UI
    (void)graph; (void)nodeId;
    return false;
#endif
}
