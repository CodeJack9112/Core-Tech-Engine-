Production upgrade notes:

- Animation: Skeleton and AnimatorSystem now support per-clip bone animation sampling and apply sampled transforms to entity TransformComponents. This is a CPU-side high-level implementation; full GPU skinning requires per-vertex bone weights and shader integration.
- Terrain: Mesh generation includes indices, normals, UVs, and supports 'skip' LOD parameter to reduce mesh density for performance.
- Shadows: ShadowMapper now creates a depth framebuffer and depth texture; beginShadowPass/endShadowPass bound the FBO. Use shadow shader 'shadow_depth.vs' to render depth from light's view.
- Build: Updated CMake to include new sources. Use -DFETCH_DEPS=ON and ensure GLAD/GLFW are available for runtime.

Note: These are still prototype-to-production transitions. Please compile and test on your target machine; file an issues list and I will iterate further.
