CoreTech Optimization Systems - Added
====================================

The following optimization modules were added as part of the 'Optimized All' package.

- FrustumCuller: robust frustum extraction and AABB intersection test.
- Octree: spatial partitioning for dynamic object queries.
- BVH: simple bounding-volume hierarchy builder for prim centers (useful for ray queries/broadphase).
- HZBOcclusion: Hierarchical Z-Buffer occlusion culling stub (GPU-side implementation required).
- LODManager: simple distance-based LOD selector.
- GPUInstancing: instance data collection and GPU upload stub (create instance buffers and draw instanced).
- RenderGraph: node-based render graph with topological compile and execution order.
- JobSystem: cross-platform thread-pool based job system (simple implementation provided).
- ShaderCache: stub for shader precompilation and caching.
- TextureStreaming: request/process streaming stub for async texture loading and GPU upload.
- MeshOptimizer: placeholder for vertex cache optimization (Forsyth/Tipsify recommended).
- OptimizationPanels: ImGui integration to visualize and toggle systems at runtime.

Notes:
- Many of these modules are provided as working prototypes and stubs that require renderer-specific
  GPU APIs to complete (e.g., HZB needs GPU depth mip generation; GPUInstancing needs a concrete
  buffer upload method).
- The package includes CMake integration. Build the CoreTechOptimizations static library to include these
  systems in your engine.

Recommended next steps to make them production-ready:
- Implement GPU-side HZB generation and occlusion queries using your graphics API.
- Implement Forsyth/Tipsify in MeshOptimizer for real performance gains.
- Add GPU indirect draw and command buffer generation in RenderGraph.
- Implement async GPU uploads in TextureStreaming and use staging buffers.
- Replace the JobSystem with a work-stealing scheduler for low-latency tasks.
