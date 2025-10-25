\
#include "../include/VisualScripting.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <ctime>

namespace VisualScripting {

// Simple JSON helpers (limited) reused from earlier implementation
static std::string escape(const std::string &s){
    std::string out; for(char c: s){ if(c=='\"') out += "\\\\\""; else if(c=='\n') out += "\\\\n"; else out += c; } return out;
}

// Deserialize simplified JSON motivated for demo (not robust)
static std::string extractString(const std::string &s, size_t &pos){
    if(pos>=s.size() || s[pos] != '\"') return std::string(); pos++; std::string out;
    while(pos < s.size()){ char c = s[pos++]; if(c=='\\\"'){ out.push_back('\"'); } else if(c=='\\\\'){ if(pos<s.size()){ char n=s[pos++]; if(n=='n') out.push_back('\\n'); else out.push_back(n); } } else if(c=='\"') break; else out.push_back(c); } return out;
}
static void skipWhitespace(const std::string &s, size_t &pos){ while(pos < s.size() && isspace((unsigned char)s[pos])) pos++; }

// naive parse to get editable nodes only, then compile will topologically sort
Graph Graph::deserialize(const std::string& jsonStr){
    Graph g; size_t p=0; auto &s=jsonStr; skipWhitespace(s,p);
    // very limited parsing: find "nodes" array and "links" array
    size_t knodes = s.find("\"nodes\"");
    if(knodes!=std::string::npos){ p = s.find('[', knodes)+1;
        while(p < s.size()){
            skipWhitespace(s,p);
            if(p>=s.size()||s[p]==']') break;
            if(s[p]=='{'){ p++; auto n = std::make_shared<Node>(); while(p<s.size() && s[p] != '}'){
                    skipWhitespace(s,p);
                    if(s[p]=='\"'){
                        std::string key = extractString(s,p); skipWhitespace(s,p); if(s[p]==':') p++; skipWhitespace(s,p);
                        if(key=="id"){ if(s[p]=='\"') n->id = extractString(s,p); }
                        else if(key=="type"){ if(s[p]=='\"') n->type = extractString(s,p); n->title = n->type; }
                        else if(key=="inputs" && s[p]=='{'){ p++; skipWhitespace(s,p);
                            while(p<s.size() && s[p] != '}'){
                                if(s[p]=='\"'){ std::string inName = extractString(s,p); skipWhitespace(s,p); if(s[p]==':') p++; skipWhitespace(s,p);
                                    // value object { "type":n, "f":.., "s":".." }
                                    if(s[p]=='{'){ p++; int typ=0; float f=0.0f; int ii=0; bool bb=false; std::string ss;
                                        while(p<s.size() && s[p] != '}'){
                                            if(s[p]=='\"'){ std::string k = extractString(s,p); skipWhitespace(s,p); if(s[p]==':') p++; skipWhitespace(s,p);
                                                if(k=="type"){ std::string num; while(p<s.size() && (isdigit((unsigned char)s[p])||s[p]=='-')){ num.push_back(s[p++]); } typ = atoi(num.c_str()); }
                                                else if(k=="f"){ std::string num; while(p<s.size() && (isdigit((unsigned char)s[p])||s[p]=='.'||s[p]=='-'||s[p]=='e')){ num.push_back(s[p++]); } f = (float)atof(num.c_str()); }
                                                else if(k=="s"){ if(s[p]=='\"') ss = extractString(s,p); }
                                            } else p++;
                                            skipWhitespace(s,p); if(s[p]==',') { p++; skipWhitespace(s,p); }
                                        }
                                        Value v; v.type = (Value::Type)typ; v.f = f; v.s = ss; n->inputs.emplace_back(inName, v);
                                        if(s[p]=='}') p++;
                                    }
                                } else p++;
                                skipWhitespace(s,p); if(s[p]==',') { p++; skipWhitespace(s,p); }
                            }
                            if(s[p]=='}') p++;
                        }
                        else if(key=="outputs" && s[p]=='{'){ p++; skipWhitespace(s,p);
                            while(p<s.size() && s[p] != '}'){
                                if(s[p]=='\"'){ std::string outName = extractString(s,p); skipWhitespace(s,p); if(s[p]==':') p++; skipWhitespace(s,p);
                                    if(s[p]=='{'){ p++; int typ=0; float f=0.0f; std::string ss; while(p<s.size() && s[p] != '}'){ if(s[p]=='\"'){ std::string k = extractString(s,p); skipWhitespace(s,p); if(s[p]==':') p++; skipWhitespace(s,p); if(k==\"type\"){ std::string num; while(p<s.size() && (isdigit((unsigned char)s[p])||s[p]=='-')){ num.push_back(s[p++]); } typ = atoi(num.c_str()); } else if(k==\"f\"){ std::string num; while(p<s.size() && (isdigit((unsigned char)s[p])||s[p]=='.'||s[p]=='-'||s[p]=='e')){ num.push_back(s[p++]); } f=(float)atof(num.c_str()); } else if(k==\"s\"){ if(s[p]=='\"') ss = extractString(s,p); } } else p++; skipWhitespace(s,p); if(s[p]==','){ p++; skipWhitespace(s,p); } } Value v; v.type=(Value::Type)typ; v.f=f; v.s=ss; n->outputs.emplace_back(outName, v); if(s[p]=='}') p++; } } else p++; skipWhitespace(s,p); if(s[p]==',') { p++; skipWhitespace(s,p); } } if(s[p]=='}') p++; }
                    }
                    skipWhitespace(s,p); if(s[p]==',') p++;
                }
                g.nodesEditable.push_back(n);
            } else break;
        }
    }
    // parse links array similarly
    size_t klinks = s.find("\"links\"");
    if(klinks!=std::string::npos){ p = s.find('[', klinks)+1;
        while(p<s.size()){
            skipWhitespace(s,p);
            if(p>=s.size()||s[p]==']') break;
            if(s[p]=='{'){ p++; Link L; std::string fromNode, fromOutput, toNode, toInput; while(p<s.size() && s[p] != '}'){ skipWhitespace(s,p); if(s[p]=='\"'){ std::string key = extractString(s,p); skipWhitespace(s,p); if(s[p]==':') p++; skipWhitespace(s,p); if(key==\"fromNode\" && s[p]=='\"'){ fromNode = extractString(s,p); } else if(key==\"fromOutput\" && s[p]=='\"'){ fromOutput = extractString(s,p); } else if(key==\"toNode\" && s[p]=='\"'){ toNode = extractString(s,p); } else if(key==\"toInput\" && s[p]=='\"'){ toInput = extractString(s,p); } } else p++; skipWhitespace(s,p); if(s[p]==',') { p++; skipWhitespace(s,p); } } p++; // find indices mapping later in compile
            // temporarily store as link with node id strings in toInputIndex==-1, etc. We'll store info in links vector later.
            // Use fromNodeIndex as hash of node id stored in i (this will be recalculated)
            Link tmp; tmp.fromNodeIndex = (size_t)-1; tmp.toNodeIndex = (size_t)-1; tmp.fromOutputIndex = -1; tmp.toInputIndex = -1;
            // We'll use a simplistic approach: store special map by encoding strings using addresses is complex here; instead we'll store links in a temp file in Graph name (skip)
            g.links.push_back(tmp);
        }
    }
    return g;
}

// Topological sort based compile implementation
bool Graph::compile(){
    // Build name->index map from nodesEditable
    nodesCompiled.clear(); links.clear();
    std::unordered_map<std::string, size_t> nameToIndex;
    for(size_t i=0;i<nodesEditable.size();++i){ nodesCompiled.push_back(nodesEditable[i]); nameToIndex[nodesEditable[i]->id] = i; }
    // naive approach: we won't reorder nodes now, but we will build link indices by resolving names in existing links (which were placeholders)
    // For demo, simply map links that match by names found in node outputs/inputs by scanning nodesEditable for matching names in a best-effort manner.
    // In production, you'd store link text during deserialize and then map to indices here.
    // For now, do nothing more.
    return true;
}

// Fast resolve input by index (linear search across links for demo)
// In optimized version this would use adjacency lists; keep simple for now.
Value ExecutionContext::resolveInputIndex(size_t nodeIndex, int inputIndex) const {
    if(!graph) return Value::nil();
    if(nodeIndex >= graph->nodesCompiled.size()) return Value::nil();
    auto &node = *graph->nodesCompiled[nodeIndex];
    if(inputIndex < 0 || inputIndex >= (int)node.inputs.size()) return Value::nil();
    // find link that points to this node.inputIndex
    for(const auto &l : graph->links){
        if(l.toNodeIndex == nodeIndex && l.toInputIndex == inputIndex){
            if(l.fromNodeIndex < graph->nodesCompiled.size()){
                auto &src = *graph->nodesCompiled[l.fromNodeIndex];
                if(l.fromOutputIndex >=0 && l.fromOutputIndex < (int)src.outputs.size()) return src.outputs[l.fromOutputIndex].second;
            }
        }
    }
    return node.inputs[inputIndex].second;
}

// Execute compiled graph: iterate in compiled order and call exec
bool executeCompiledGraph(const Graph& graph, ExecutionContext& ctx){
    if(!ctx.log) ctx.log = [](const std::string&){ };
    ctx.graph = &graph;
    for(size_t i=0;i<graph.nodesCompiled.size();++i){
        auto &nptr = graph.nodesCompiled[i];
        if(nptr && nptr->exec){
            // prepare a lightweight context view with resolveInputIndex bound
            nptr->exec(*nptr, const_cast<ExecutionContext&>(ctx));
        }
    }
    return true;
}

// Utility node factories
std::shared_ptr<Node> makePrintNode(const std::string& message){
    auto n = std::make_shared<Node>();
    n->id = "print_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(rand());
    n->type = "Print"; n->title = "Print"; n->inputs.emplace_back("Text", Value::makeString(message));
    n->exec = [](Node& node, ExecutionContext& ctx){ auto it = std::find_if(node.inputs.begin(), node.inputs.end(), [](auto &p){ return p.first=="Text"; }); if(it!=node.inputs.end()){ if(ctx.log) ctx.log(it->second.s); } };
    return n;
}

std::shared_ptr<Node> makeAddNode(float a, float b){
    auto n = std::make_shared<Node>(); n->id = "add_" + std::to_string(rand()); n->type="Add"; n->title="Add"; n->inputs.emplace_back("A", Value::makeFloat(a)); n->inputs.emplace_back("B", Value::makeFloat(b)); n->outputs.emplace_back("Result", Value::makeFloat(0.0f));
    n->exec = [](Node& node, ExecutionContext& ctx){ float va = node.inputs[0].second.f; float vb = node.inputs[1].second.f; node.outputs[0].second = Value::makeFloat(va+vb); };
    return n;
}

std::shared_ptr<Node> makeDelayNode(float seconds){
    auto n = std::make_shared<Node>(); n->id = "delay_" + std::to_string(rand()); n->type="Delay"; n->title="Delay"; n->inputs.emplace_back("Seconds", Value::makeFloat(seconds));
    n->exec = [seconds](Node& node, ExecutionContext& ctx){ if(ctx.schedule){ ctx.schedule([=](){ if(ctx.log) ctx.log(std::string(\"Delay finished: \") + std::to_string(seconds)); }, seconds); } else { if(ctx.log) ctx.log(std::string(\"Delay (no scheduler) \")+ std::to_string(seconds)); } };
    return n;
}

// Event and play sound nodes
std::shared_ptr<Node> makeEventEmitNode(const std::string& eventName){
    auto n = std::make_shared<Node>(); n->id = "emit_" + eventName + "_" + std::to_string(rand()); n->type="EventEmit"; n->title="EventEmit"; n->inputs.emplace_back("Payload", Value::makeString(""));
    n->exec = [eventName](Node& node, ExecutionContext& ctx){ std::string payload=\"\"; auto it = std::find_if(node.inputs.begin(), node.inputs.end(), [](auto &p){ return p.first==\"Payload\"; }); if(it!=node.inputs.end()) payload = it->second.s; if(ctx.emitEvent) ctx.emitEvent(eventName, payload); if(ctx.log) ctx.log(std::string(\"EventEmit: \")+eventName+\" payload=\"+payload); };
    return n;
}

std::shared_ptr<Node> makeEventListenerNode(const std::string& eventName){
    auto n = std::make_shared<Node>(); n->id = "listen_" + eventName + "_" + std::to_string(rand()); n->type="EventListener"; n->title="EventListener"; n->outputs.emplace_back("Payload", Value::makeString(""));
    n->exec = [eventName](Node& node, ExecutionContext& ctx){ if(ctx.log) ctx.log(std::string(\"EventListener (runtime) for: \")+eventName); };
    return n;
}

std::shared_ptr<Node> makePlaySoundNode(const std::string& soundPath){
    auto n = std::make_shared<Node>(); n->id = "playsound_" + std::to_string(rand()); n->type="PlaySound"; n->title="PlaySound"; n->inputs.emplace_back("Path", Value::makeString(soundPath));
    n->exec = [soundPath](Node& node, ExecutionContext& ctx){ if(ctx.log) ctx.log(std::string(\"PlaySound: \")+soundPath); };
    return n;
}

} // namespace VisualScripting
