Assimp Skinned Mesh Importer Demo
=================================
This demo compiles a small console tool that loads a model (glTF/FBX) using Assimp,
prints skeleton hierarchy and animation channel info, and performs a tiny CPU skinning
pass for the first vertex as an example.

Steps to run:
  1. Ensure Assimp (and glm) are available via FetchContent or installed on your system.
  2. From the project root: mkdir build && cd build
  3. cmake .. -DBUILD_ASSIMP_DEMO=ON -DCMAKE_BUILD_TYPE=Release
  4. cmake --build . --target AssimpSkinnedImporter --config Release
  5. Run: ./demo_assimp/AssimpSkinnedImporter ../assets/CesiumMan.glb
  6. To download the sample model automatically: ./demo_assimp/fetch_sample_model.sh

The sample model used is CesiumMan.glb from the KhronosGroup glTF-Sample-Models repository
(permissive samples for testing). If you prefer FBX, replace the assets/CesiumMan.glb with an FBX
file and run the same demo; Assimp supports FBX too.
