#pragma once
#include <string>
#include "../include/VisualScripting.h"

namespace EditorInspector {
    // Show inspector for a node using ImGui if available. Returns true if UI was shown.
    bool showNodeInspector(VisualScripting::Graph& graph, const std::string& nodeId);
}
