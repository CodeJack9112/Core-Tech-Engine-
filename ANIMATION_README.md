Animations & Assimp integration
===============================

- Assimp supports FBX, Collada, glTF, and many formats. Use assimp::Importer to load scenes.
- The engine contains skeleton and animation components (check include/ and src/ for Animation names).
- Typical flow:
  1. Load model via Assimp.
  2. Extract mesh, bone weights, and bone hierarchy.
  3. Upload skeletal matrices to shader for GPU skinning (vertex shader).
  4. Play animations by sampling keyframes and computing bone transforms per-frame.

- If you want, I can create an example importer that uses Assimp to import a skinned mesh and sample animations,
  and add a demo scene + sample FBX. To do that, upload a sample FBX or let me fetch a permissively licensed sample.
