#pragma once
// ToonIntegration: helper functions to register toon shaders and expose ImGui controls.
// This module is optional — it's conditionally added to the build only if your engine exposes
// a Renderer.h header and ImGui is available.

#include <string>

namespace coretech {
  // Initialize toon materials and outline pass. Should be called after renderer initialization.
  // If renderer is not available at compile time, this function is a no-op (stubbed).
  void InitializeToonMaterials();

  // Call every frame to show ImGui controls to tweak toon parameters (levels, outline, etc.).
  void ToonImGuiControls();

  // Debug helper: reload toon shaders from disk (useful during development).
  void ReloadToonShaders();
}
