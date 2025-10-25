#pragma once
// Hierarchical Z-Buffer occlusion culling interface (stub).
// Implementations should generate HZB mip chain from depth buffer and test bounding boxes.
struct HZBOcclusion {
  void BuildFromDepthTexture(unsigned int depthTextureId); // GPU texture id (API-specific)
  bool IsOccluded(float minx,float miny,float minz,float maxx,float maxy,float maxz);
};
