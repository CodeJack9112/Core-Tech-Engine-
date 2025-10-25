Enhanced Visual Scripting - Notes
Generated: 2025-10-23T20:48:07.891580Z

Files added:
- src/visualscripting/EnhancedVisualScripting.h
- src/visualscripting/EnhancedVisualScripting.cpp

How to integrate:
1. Add these two files to your build target (e.g., in CMakeLists.txt add them to the sources).
2. Call CoreTech::VSEnhanced::Init() during engine initialization (where VisualScripting was initialized).
3. Call CoreTech::VSEnhanced::Shutdown() on engine shutdown.
4. To open the editor UI, call CoreTech::VSEnhanced::DrawEditor(&openFlag) during your ImGui debug pass.
5. The current implementation is intentionally small and demo-focused: it uses integer "values" for node outputs.
   To make it production-ready:
   - Replace the simple int storage with a typed Variant (float/int/string/object-pointer).
   - Implement port-level typing and coercion rules.
   - Expand node registration to accept metadata (port names, default values, editor widgets).
   - Implement persistent graph serialization (JSON or binary).
   - Add a proper canvas UI with node drag/drop and link creation.

Integration: CMake root file was added as example 'CMakeLists.txt'. The Visual Scripting files are included by default when ENABLE_VS_ENHANCED is ON.
