CoreTech Supported File Formats (via backends)
=============================================
This engine aims to support as many common file formats as possible through multiple backends.

Model formats (via Assimp, tinygltf, tinyobj):
  - FBX, OBJ, glTF (glb), COLLADA (.dae), 3DS, BLEND, PLY, STL, LWO, ABC, GLTF, and many more (see Assimp supported list).
  - OBJ supported via tinyobjloader as a fallback.
  - glTF/glb supported via tinygltf as fallback/alternate loader.

Image formats (via stb_image):
  - PNG, JPEG, BMP, TGA, GIF (limited), HDR, PSD, and more supported by stb_image.h.

Audio formats (via miniaudio/OpenAL):
  - WAV, OGG, FLAC, MP3 (if miniaudio is compiled with relevant decoders), and more depending on backend.

Notes:
  - This repository includes loader wrappers and CMake FetchContent entries to acquire header-only or small libraries.
  - For many formats (FBX), Assimp must be available (via FetchContent or prebuilt) since FBX SDK is proprietary.
  - Some formats may require optional decoders; ensure miniaudio or OpenAL is configured appropriately.
