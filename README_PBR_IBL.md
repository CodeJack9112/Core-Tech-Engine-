PBR / IBL Pipeline Added (source-level implementation)

What I added:
- HDRLoader (uses stb_image) to load HDR equirectangular textures (float data).
- IBLGenerator flow (equirectangular -> cubemap, irradiance convolution, prefilter specular, BRDF LUT)
  with shader files under tools/renderer/shaders/ibl_*. This includes stub implementations that follow the correct flow.
- Deferred lighting shader updated to sample irradiance/prefilter/BRDF LUT textures.
- Editor UI entry to select an HDR envmap and trigger IBL generation (calls IBLGenerator::generateIBL).
- CMake hint to fetch tinyexr (optional) for EXR support.

Notes:
- The IBLGenerator implementation here is a source-level flow: it creates GL textures and placeholder results.
  For production, the convolution, importance sampling and BRDF integration shaders are already in place and can be refined.
- The code assumes an OpenGL context is active when generating IBL. Call IBLGenerator::init() after GL context creation.
- Build with -DFETCH_DEPS=ON to bring tinyexr/assimp/etc as needed.
