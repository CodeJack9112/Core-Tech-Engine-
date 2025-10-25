#include "EnhancedVisualScripting.h"
#include <algorithm>
#include <queue>
#include <imgui.h>
#include <fstream>
#include <sstream>

namespace CoreTech { namespace VSEnhanced {

static std::unordered_map<std::string, NodeDef> g_nodeRegistry;
static std::unordered_map<NodeId, NodeInstance> g_nodes;
static NodeId g_nextNodeId = 1;

// --- Helpers for simple JSON writing/reading (no external deps) ---
static std::string EscapeString(const std::string &s) {
    std::string out;
    for (char c: s) {
        if (c=='\\') out += "\\\\";
        else if (c=='\"') out += "\\\"";
        else if (c=='\n') out += "\\n";
        else out += c;
    }
    return out;
}

// --- Init / Shutdown ---
void Init() {
    // register basic nodes
    RegisterNodeType("ConstInt", NodeDef{"ConstInt", 0, 1, [](const std::vector<Variant>& inputs, std::vector<Variant>& outputs){ outputs.resize(1); outputs[0]=int64_t(1); }});
    RegisterNodeType("Add", NodeDef{"Add", 2, 1, [](const std::vector<Variant>& inputs, std::vector<Variant>& outputs){
        int64_t a=0,b=0;
        if (inputs.size()>0 && std::holds_alternative<int64_t>(inputs[0].v)) a=std::get<int64_t>(inputs[0].v);
        if (inputs.size()>1 && std::holds_alternative<int64_t>(inputs[1].v)) b=std::get<int64_t>(inputs[1].v);
        outputs.resize(1);
        outputs[0]=int64_t(a+b);
    }});
    RegisterNodeType("Print", NodeDef{"Print",1,0,[](const std::vector<Variant>& inputs, std::vector<Variant>& outputs){ if (inputs.size()>0) { printf("[VS Print] %s\n", inputs[0].ToString().c_str()); } }});
}

void Shutdown() {
    g_nodeRegistry.clear(); g_nodes.clear(); g_nextNodeId=1;
}

bool RegisterNodeType(const std::string& type, const NodeDef& def) {
    if (g_nodeRegistry.find(type)!=g_nodeRegistry.end()) return false;
    g_nodeRegistry[type]=def; return true;
}
bool UnregisterNodeType(const std::string& type) { return g_nodeRegistry.erase(type)>0; }

NodeId CreateNode(const std::string& type) {
    if (g_nodeRegistry.find(type)==g_nodeRegistry.end()) return 0;
    NodeId id = g_nextNodeId++;
    NodeInstance inst; inst.id=id; inst.type=type; inst.inputs.clear();
    // default position
    inst.posX = 150.0f + (id%5)*160.0f; inst.posY = 100.0f + (id%3)*120.0f;
    g_nodes[id]=inst; return id;
}

bool RemoveNode(NodeId id) {
    if (g_nodes.erase(id)==0) return false;
    for (auto &p: g_nodes) {
        auto &inputs = p.second.inputs;
        inputs.erase(std::remove(inputs.begin(), inputs.end(), id), inputs.end());
    }
    return true;
}

bool Connect(NodeId from, NodeId to) {
    if (g_nodes.find(from)==g_nodes.end()) return false;
    if (g_nodes.find(to)==g_nodes.end()) return false;
    auto &toNode = g_nodes[to];
    if (std::find(toNode.inputs.begin(), toNode.inputs.end(), from)!=toNode.inputs.end()) return false;
    toNode.inputs.push_back(from); return true;
}

bool Disconnect(NodeId from, NodeId to) {
    if (g_nodes.find(to)==g_nodes.end()) return false;
    auto &inputs = g_nodes[to].inputs;
    auto it = std::remove(inputs.begin(), inputs.end(), from);
    if (it==inputs.end()) return false;
    inputs.erase(it, inputs.end()); return true;
}

// --- Compiler (topological sort) ---
CompileResult ValidateAndCompile() {
    CompileResult res; res.success=false;
    if (g_nodes.empty()) { res.success=true; return res; }
    std::unordered_map<NodeId,int> indeg; std::unordered_map<NodeId,std::vector<NodeId>> adj;
    for (auto &kv: g_nodes) indeg[kv.first]=0;
    for (auto &kv: g_nodes) {
        NodeId nid = kv.first;
        for (auto in: kv.second.inputs) { adj[in].push_back(nid); indeg[nid]++; }
    }
    std::queue<NodeId> q; for (auto &kv: indeg) if (kv.second==0) q.push(kv.first);
    while (!q.empty()) { NodeId n=q.front(); q.pop(); res.orderedNodes.push_back(g_nodes[n]); for (auto m: adj[n]){ indeg[m]--; if (indeg[m]==0) q.push(m);} }
    if (res.orderedNodes.size()!=g_nodes.size()) { res.errors.push_back("Graph contains cycles or disconnected components."); return res; }
    // Simple bytecode: sequence of node ids (4 bytes each) terminated by 0
    for (auto &ni: res.orderedNodes) {
        uint32_t id = ni.id;
        res.bytecode.push_back((id>>0)&0xFF);
        res.bytecode.push_back((id>>8)&0xFF);
        res.bytecode.push_back((id>>16)&0xFF);
        res.bytecode.push_back((id>>24)&0xFF);
    }
    // terminator
    res.bytecode.push_back(0); res.bytecode.push_back(0); res.bytecode.push_back(0); res.bytecode.push_back(0);
    res.success=true; return res;
}

int ExecuteBytecode(const std::vector<uint8_t>& bytecode, int maxSteps) {
    if (bytecode.size()<4) return -1;
    std::unordered_map<NodeId, Variant> tempVals;
    size_t pc=0; int steps=0;
    while (pc+4<=bytecode.size() && steps<maxSteps) {
        uint32_t id = (uint32_t)bytecode[pc] | ((uint32_t)bytecode[pc+1]<<8) | ((uint32_t)bytecode[pc+2]<<16) | ((uint32_t)bytecode[pc+3]<<24);
        pc+=4; if (id==0) break;
        steps++;
        auto it = g_nodes.find(id); if (it==g_nodes.end()) continue;
        NodeInstance &ni = it->second;
        auto defIt = g_nodeRegistry.find(ni.type); if (defIt==g_nodeRegistry.end()) continue;
        NodeDef &def = defIt->second;
        // gather inputs
        std::vector<Variant> inputs; for (auto src: ni.inputs) { auto itv = tempVals.find(src); if (itv!=tempVals.end()) inputs.push_back(itv->second); else inputs.push_back(Variant()); }
        std::vector<Variant> outputs;
        def.execute(inputs, outputs);
        if (!outputs.empty()) tempVals[ni.id]=outputs[0];
    }
    return 0;
}

// --- Serialization (simple JSON writer/reader) ---
bool SaveGraphJSON(const std::string& path) {
    std::ofstream f(path, std::ios::out);
    if (!f.is_open()) return false;
    f<<"{\n  \"nodes\": [\n";
    bool first=true;
    for (auto &kv: g_nodes) {
        if (!first) f<<",\n"; first=false;
        const NodeInstance &n = kv.second;
        f<<"    {\n      \"id\": "<<n.id<<",\n      \"type\": \""<<EscapeString(n.type)<<"\",\n      \"pos\": ["<<n.posX<<","<<n.posY<<"],\n      \"inputs\": [";
        for (size_t i=0;i<n.inputs.size();++i) { if (i) f<<","; f<<n.inputs[i]; }
        f<<"]\n    }";
    }
    f<<"\n  ]\n}\n";
    f.close(); return true;
}

bool LoadGraphJSON(const std::string& path) {
    std::ifstream f(path); if (!f.is_open()) return false;
    std::string data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    // Very small robust parser for our saved graph format
    try {
        // find "nodes" array
        size_t nodesPos = data.find("\"nodes\""); if (nodesPos==std::string::npos) return false;
        size_t arrStart = data.find('[', nodesPos); if (arrStart==std::string::npos) return false;
        size_t arrEnd = data.find(']', arrStart); if (arrEnd==std::string::npos) return false;
        std::string arr = data.substr(arrStart+1, arrEnd-arrStart-1);
        // split objects by '}' followed by optional comma
        g_nodes.clear(); g_nextNodeId=1;
        size_t pos = 0;
        while (pos < arr.size()) {
            size_t objStart = arr.find('{', pos);
            if (objStart==std::string::npos) break;
            size_t objEnd = arr.find('}', objStart);
            if (objEnd==std::string::npos) break;
            std::string obj = arr.substr(objStart+1, objEnd-objStart-1);
            NodeInstance cur; cur.inputs.clear();
            // parse simple fields
            size_t idPos = obj.find("\"id\""); if (idPos!=std::string::npos) {
                size_t colon = obj.find(':', idPos); if (colon!=std::string::npos) {
                    std::string num; size_t i=colon+1; while (i<obj.size() && (obj[i]==' '||obj[i]=='\t')) i++;
                    while (i<obj.size() && ((obj[i]>='0'&&obj[i]<='9'))) { num.push_back(obj[i]); i++; }
                    if (!num.empty()) cur.id = (NodeId)std::stoul(num);
                }
            }
            size_t typePos = obj.find("\"type\""); if (typePos!=std::string::npos) {
                size_t q1 = obj.find('"', typePos+7); if (q1!=std::string::npos) {
                    size_t q2 = obj.find('"', q1+1); if (q2!=std::string::npos) cur.type = obj.substr(q1+1, q2-q1-1);
                }
            }
            size_t posPos = obj.find("\"pos\""); if (posPos!=std::string::npos) {
                size_t l = obj.find('[', posPos); size_t r = obj.find(']', posPos);
                if (l!=std::string::npos && r!=std::string::npos && r>l) {
                    std::string inside = obj.substr(l+1, r-l-1);
                    size_t comma = inside.find(','); if (comma!=std::string::npos) {
                        cur.posX = std::stof(inside.substr(0,comma)); cur.posY = std::stof(inside.substr(comma+1));
                    }
                }
            }
            size_t inputsPos = obj.find("\"inputs\""); if (inputsPos!=std::string::npos) {
                size_t l = obj.find('[', inputsPos); size_t r = obj.find(']', inputsPos);
                if (l!=std::string::npos && r!=std::string::npos && r>l) {
                    std::string inside = obj.substr(l+1, r-l-1);
                    std::stringstream ss(inside); std::string token;
                    while (std::getline(ss, token, ',')) { auto tok = token; // trim
                        size_t a = tok.find_first_not_of(" \t\r\n"); size_t b = tok.find_last_not_of(" \t\r\n"); if (a!=std::string::npos && b!=std::string::npos) tok = tok.substr(a, b-a+1);
                        if (!tok.empty()) cur.inputs.push_back((NodeId)std::stoul(tok));
                    }
                }
            }
            if (cur.id==0) { pos = objEnd+1; continue; }
            g_nodes[cur.id] = cur; if (cur.id>=g_nextNodeId) g_nextNodeId = cur.id+1;
            pos = objEnd+1;
        }
    } catch(...) { return false; }
    return true;
}


// --- Simple ImGui canvas UI ---
static bool s_openEditor=true;
static NodeId s_selectedNode=0;
static NodeId s_linkStart=0;
// --- ImGui canvas UI with drag-to-connect ---
static bool s_openEditor=true;
static NodeId s_selectedNode=0;
static NodeId s_linkStart=0;
static bool s_isDraggingLink=false;
static ImVec2 s_dragStartPos(0,0);

void DrawEditor(bool* p_open) {
    if (p_open) s_openEditor = *p_open;
    if (!s_openEditor) return;
    ImGui::Begin("Visual Scripting - Canvas", p_open);
    ImGui::Columns(2);
    ImGui::BeginChild("palette", ImVec2(200,0), true);
    ImGui::Text("Palette");
    for (auto &kv: g_nodeRegistry) { if (ImGui::Button(kv.first.c_str())) CreateNode(kv.first); }
    if (ImGui::Button("Save Graph")) { SaveGraphJSON("visual_graph_saved.json"); }
    ImGui::SameLine(); if (ImGui::Button("Load Graph")) { LoadGraphJSON("visual_graph.json"); }
    if (ImGui::Button("Validate & Run")) {
        auto res = ValidateAndCompile();
        if (!res.success) { for (auto &e: res.errors) ImGui::TextColored(ImVec4(1,0.4f,0.4f,1), "Err: %s", e.c_str()); }
        else { ImGui::Text("Running..."); ExecuteBytecode(res.bytecode); }
    }
    ImGui::EndChild();
    ImGui::NextColumn();

    ImGui::BeginChild("canvas", ImVec2(0,0), false);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    draw->AddRectFilled(canvasPos, ImVec2(canvasPos.x+canvasSize.x, canvasPos.y+canvasSize.y), IM_COL32(20,20,20,255));

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 mousePos = io.MousePos;

    // render nodes
    for (auto &kv: g_nodes) {
        NodeInstance &n = kv.second;
        ImVec2 p(canvasPos.x + n.posX, canvasPos.y + n.posY);
        ImVec2 sz(140,60);
        ImU32 col = (s_selectedNode==n.id)?IM_COL32(80,140,220,255):IM_COL32(70,70,70,255);
        draw->AddRectFilled(p, ImVec2(p.x+sz.x, p.y+sz.y), col, 6.0f);
        draw->AddText(ImVec2(p.x+6, p.y+6), IM_COL32(255,255,255,255), n.type.c_str());
        draw->AddText(ImVec2(p.x+6, p.y+26), IM_COL32(200,200,200,255), (std::to_string(n.inputs.size()) + " inputs").c_str());

        ImGui::SetCursorScreenPos(p);
        ImGui::InvisibleButton((std::string("node_")+std::to_string(n.id)).c_str(), sz);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) { s_selectedNode = n.id; }
        if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            n.posX += delta.x; n.posY += delta.y;
        }

        // output port (right side)
        ImVec2 outP(p.x+sz.x, p.y+sz.y*0.5f);
        draw->AddCircleFilled(outP, 6.0f, IM_COL32(255,200,0,255));
        // input port (left side)
        ImVec2 inP(p.x, p.y+sz.y*0.5f);
        draw->AddCircleFilled(inP, 6.0f, IM_COL32(200,200,255,255));

        // start drag with left-click on output port
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hovered) {
            // determine if mouse over output circle
            ImVec2 rel = ImVec2(mousePos.x - outP.x, mousePos.y - outP.y);
            float dist2 = rel.x*rel.x + rel.y*rel.y;
            if (dist2 <= 36.0f) { s_linkStart = n.id; s_isDraggingLink = true; s_dragStartPos = outP; }
        }

        // release to connect if dragging and mouse over this node's input port
        if (s_isDraggingLink) {
            ImVec2 relIn = ImVec2(mousePos.x - inP.x, mousePos.y - inP.y);
            float dist2in = relIn.x*relIn.x + relIn.y*relIn.y;
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && dist2in <= 36.0f && s_linkStart!=0 && s_linkStart!=n.id) {
                Connect(s_linkStart, n.id);
                s_isDraggingLink = false; s_linkStart = 0;
            }
        }

        // right-click context menu
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup((std::string("node_ctx_")+std::to_string(n.id)).c_str());
        }
        if (ImGui::BeginPopup((std::string("node_ctx_")+std::to_string(n.id)).c_str())) {
            if (ImGui::MenuItem("Delete Node")) { RemoveNode(n.id); ImGui::EndPopup(); break; }
            if (ImGui::MenuItem("Start Link")) { s_linkStart = n.id; }
            if (ImGui::MenuItem("End Link")) { if (s_linkStart!=0) { Connect(s_linkStart, n.id); s_linkStart=0; } }
            ImGui::EndPopup();
        }
    }

    // draw existing links
    for (auto &kv: g_nodes) {
        NodeInstance &n = kv.second;
        ImVec2 toP(canvasPos.x + n.posX, canvasPos.y + n.posY + 30);
        for (auto src: n.inputs) {
            auto it = g_nodes.find(src); if (it==g_nodes.end()) continue;
            NodeInstance &s = it->second;
            ImVec2 fromP(canvasPos.x + s.posX + 140, canvasPos.y + s.posY + 30);
            ImVec2 c1 = ImVec2(fromP.x + 50, fromP.y);
            ImVec2 c2 = ImVec2(toP.x - 50, toP.y);
            draw->AddBezierCurve(fromP, c1, c2, toP, IM_COL32(200,200,100,200), 3.0f);
        }
    }

    // draw dragging link if active
    if (s_isDraggingLink) {
        ImVec2 cur = mousePos;
        ImVec2 fromP = s_dragStartPos;
        ImVec2 toP = cur;
        ImVec2 c1 = ImVec2(fromP.x + 50, fromP.y);
        ImVec2 c2 = ImVec2(toP.x - 50, toP.y);
        draw->AddBezierCurve(fromP, c1, c2, toP, IM_COL32(255,255,255,200), 3.0f);
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) { s_isDraggingLink = false; s_linkStart = 0; }
    }

    ImGui::EndChild();
    ImGui::End();
    if (p_open) *p_open = s_openEditor;
}

