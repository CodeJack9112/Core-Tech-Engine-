CoreTech Optimization Implementations - Summary
==============================================

This package updates previously added optimization stubs with fuller implementations (where possible)
that do not require a specific renderer API. All code is additive and guarded where OpenGL is needed.

Implementations added:
- MeshOptimizer: Forsyth algorithm implementation for reordering triangle indices (improves post-transform cache).
- JobSystem: improved thread-pool with local queues and steal attempts between queues.
- HZB: CPU-side hierarchical Z-buffer builder from a depth buffer and an NDCRange occlusion query.
- TextureStreaming: background loader using stb_image if present; callback-based delivery for GPU upload.
- RenderGraph: resource-aware render graph with topological compile and execution.
- GPUInstancing: OpenGL-backed instancer (guarded by GL headers) with Upload/BindAndDraw methods.
- ShaderCache: tries to precompile fragment shaders via OpenGL if available, otherwise caches source.

Limitations & required wiring:
- GPU-specific parts require your renderer to expose GL context or to adapt calls to your API. These are
  guarded by checks for GL headers but will be no-ops if not present.
- HZB expects a CPU-accessible depth buffer (vector<float> depth in 0..1). For GPU-only pipelines you must
  read back the depth buffer to CPU or implement HZB on GPU.
- TextureStreaming depends on stb_image.h for decoding. Place stb_image.h in thirdparty include or enable FetchContent.

Next steps (optional):
- Replace JobSystem with a true work-stealing deque implementation for maximum throughput.
- Implement GPU HZB generation and occlusion query for highest performance.
- Hook GPUInstancing into your renderer's VAO/attribute binding conventions.


Added QEM optimized implementation, runtime LOD switching system (LODRuntime), and a glTF export tool (export_gltf). Note: export_gltf requires nlohmann/json.hpp in thirdparty/json.hpp.
