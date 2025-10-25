#pragma once
// EnhancedVisualScripting.h - upgraded with Variant, serialization, and canvas UI metadata
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <cstdint>
#include <optional>
#include "Variant.h"

namespace CoreTech { namespace VSEnhanced {

using NodeId = uint32_t;

struct NodeDef {
    std::string name;
    uint32_t inputCount;
    uint32_t outputCount;
    std::function<void(const std::vector<Variant>& inputs, std::vector<Variant>& outputs)> execute;
};

struct NodeInstance {
    NodeId id;
    std::string type;
    std::vector<NodeId> inputs;
    std::vector<std::string> metadata; // arbitrary metadata, e.g., position "x,y"
    float posX=100.0f;
    float posY=100.0f;
};

struct CompileResult {
    bool success;
    std::vector<std::string> errors;
    std::vector<uint8_t> bytecode; // opaque bytecode
    std::vector<NodeInstance> orderedNodes;
};

void Init();
void Shutdown();

bool RegisterNodeType(const std::string& type, const NodeDef& def);
bool UnregisterNodeType(const std::string& type);

NodeId CreateNode(const std::string& type);
bool RemoveNode(NodeId id);
bool Connect(NodeId from, NodeId to);
bool Disconnect(NodeId from, NodeId to);

CompileResult ValidateAndCompile();
int ExecuteBytecode(const std::vector<uint8_t>& bytecode, int maxSteps = 100000);

// Editor UI (ImGui canvas + simple node boxes)
void DrawEditor(bool* p_open=nullptr);

// Serialization
bool SaveGraphJSON(const std::string& path);
bool LoadGraphJSON(const std::string& path);

} } // namespace
