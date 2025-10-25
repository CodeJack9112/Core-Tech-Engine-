#include "MeshOptimizer.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>

// Tom Forsyth's Linear-Speed Vertex Cache Optimisation
// Reference: https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html

struct VertexData {
  int activeTriCount;
  std::vector<int> triIndices; // indices to triangle list
  int cachePos;
  VertexData(): activeTriCount(0), cachePos(-1) {}
};

static inline float FindVertexScore(const VertexData& v, unsigned cacheSize) {
  if(v.activeTriCount == 0) return -1.0f;
  float score = 0.0f;
  // Cache position score
  if(v.cachePos < 0) {
    // not in cache
  } else {
    if(v.cachePos < 3) score += 0.75f; // a bit arbitrary
    else {
      float scaler = 1.0f / (cacheSize - 3);
      score += 0.75f * (1.0f - (v.cachePos - 3) * scaler);
    }
  }
  // valence boost
  float valenceBoost = powf((float)v.activeTriCount, -0.5f);
  score += 2.0f * valenceBoost;
  return score;
}

void MeshOptimizer::OptimizeIndices(std::vector<unsigned int>& indices, unsigned int vertexCount, unsigned int cacheSize) {
  unsigned triCount = indices.size() / 3;
  if(triCount == 0) return;
  std::vector<VertexData> verts(vertexCount);
  std::vector<int> triAdded(triCount, 0);
  std::vector<int> triScore(triCount, 0);
  std::vector<float> triScoreF(triCount, 0.0f);
  // Build adjacency
  for(unsigned i=0;i<triCount;i++){
    for(int j=0;j<3;j++){
      unsigned vi = indices[3*i + j];
      if(vi >= vertexCount) continue;
      verts[vi].activeTriCount++;
      verts[vi].triIndices.push_back(i);
    }
  }
  // initial tri scores
  for(unsigned i=0;i<triCount;i++){
    float s = 0.0f;
    for(int j=0;j<3;j++){
      unsigned vi = indices[3*i + j];
      s += FindVertexScore(verts[vi], cacheSize);
    }
    triScoreF[i] = s;
  }
  std::vector<int> output; output.reserve(indices.size());
  // simulated cache as list of vertex indices, front is most recently used
  std::deque<int> cache;
  auto useVertex = [&](int v){
    // update cache pos
    // find position
    int pos = -1; for(size_t i=0;i<cache.size();++i) if(cache[i] == v){ pos = (int)i; break; }
    if(pos != -1) cache.erase(cache.begin() + pos);
    cache.push_front(v);
    while((int)cache.size() > (int)cacheSize) cache.pop_back();
    // update cachePos for all verts (inefficient but ok for moderate sizes)
    for(unsigned vi=0; vi<verts.size(); ++vi) {
      int p = -1; for(size_t k=0;k<cache.size();++k) if(cache[k]==(int)vi){ p=(int)k; break; }
      verts[vi].cachePos = p;
    }
  };
  // main loop: pick best-scoring triangle
  for(unsigned remaining=triCount; remaining>0; --remaining){
    // find best tri
    int best = -1; float bestScore = -1e9f;
    for(unsigned i=0;i<triCount;i++) if(!triAdded[i]){
      if(triScoreF[i] > bestScore){ bestScore = triScoreF[i]; best = (int)i; }
    }
    if(best == -1) break;
    // emit triangle
    triAdded[best] = 1;
    for(int j=0;j<3;j++){ unsigned vi = indices[3*best + j]; output.push_back(vi); }
    // update verts
    for(int j=0;j<3;j++){
      unsigned vi = indices[3*best + j]; verts[vi].activeTriCount--;
    }
    // simulate using vertices
    for(int j=0;j<3;j++) useVertex(indices[3*best + j]);
    // update scores for affected triangles
    for(int v=0; v<(int)vertexCount; ++v) {
      if(verts[v].triIndices.empty()) continue;
      for(int triIdx : verts[v].triIndices){ if(triAdded[triIdx]) continue; float s=0.0f; for(int k=0;k<3;k++){ unsigned vi = indices[3*triIdx + k]; s += FindVertexScore(verts[vi], cacheSize); } triScoreF[triIdx] = s; }
    }
  }
  if(output.size() == indices.size()){
    indices.assign(output.begin(), output.end());
  }
}
