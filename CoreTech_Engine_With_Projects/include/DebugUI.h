#pragma once
#include "NavMesh.h"
#include "PickupManager.h"
#include <btBulletDynamicsCommon.h>

// Optional ImGui debug UI. Define IMGUI_ENABLED to compile with ImGui hooks.
namespace DebugUI {
    void showPCGControls(unsigned int& seed, bool& regenRequested);
    void showNavMesh(const NavMesh& nm);
    void showPickupLog(const PickupManager& pm);
}
