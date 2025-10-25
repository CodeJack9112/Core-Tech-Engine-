LOD Generator (Vertex Clustering)
=================================

This tool implements a simple vertex-clustering mesh simplifier and a command-line utility
`lod_generator` that reads a Wavefront OBJ and writes a simplified OBJ.

Usage:
  1. Build the tools: from project root: mkdir build && cd build; cmake ..; cmake --build . --target lod_generator
  2. Run: ./tools/lod_generator <in.obj> <out.obj> <gridSize>
     e.g. ./tools/lod_generator ../assets/cube.obj ../assets/cube_lod.obj 0.1

Notes:
  - The simplifier merges vertices into cubic grid cells of size `gridSize` and averages attributes.
  - This is a fast, simple approach suitable for coarse LOD generation and previews. For higher quality
    LODs, consider edge-collapse or quadric error metric (QEM) based simplifiers.


Added QEM-based Simplifier and LOD Editor GUI integration. Use tools/generate_lods or the in-engine ImGui panel to create LODs.
