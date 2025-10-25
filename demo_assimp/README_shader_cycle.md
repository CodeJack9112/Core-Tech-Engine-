Shader Cycle Sequencer Demo
===========================
This demo cycles through cinematic shaders (MW1/MW2/MW3) on a model and attempts to save screenshots.

Steps:
  1. Ensure dependencies and thirdparty prebuilt SDKs are available (Assimp, GLM, Renderer target).
  2. Build the project using CMake presets (e.g., cmake --preset ninja-release).
  3. Build the demo target: demo_assimp/demo_shader_cycle
  4. Place a model at assets/CesiumMan.glb or pass a path to the demo binary.
  5. Run demo_assimp/run_shader_cycle.sh to run the sequencer and collect screenshots in demo_assimp/screenshots/.

Notes:
  - The demo uses best-effort renderer API calls (CreateEntityFromModel, SetMaterialForEntity, SaveScreenshot).
    If your renderer uses different function names, either adapt demo_shader_cycle.cpp or provide wrapper shims.
  - This environment could not execute the demo or capture frames; the code is provided for local use.
