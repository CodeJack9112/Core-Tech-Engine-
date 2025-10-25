#include "LODGenerator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <tuple>
#include <cmath>

namespace coretech {

static inline long long PackCellKey(int x, int y, int z){
  // pack three 21-bit signed ints into 64-bit key
  const long long mask = (1LL<<21)-1;
  long long a = (long long)(x & mask);
  long long b = (long long)(y & mask);
  long long c = (long long)(z & mask);
  return (a) | (b<<21) | (c<<42);
}

Mesh SimplifyByVertexClustering(const Mesh& inMesh, float gridSize){
  Mesh out;
  if(gridSize <= 0.0f) return inMesh;
  struct Accum { glm::vec3 pos = glm::vec3(0.0f); glm::vec3 n = glm::vec3(0.0f); glm::vec2 uv = glm::vec2(0.0f); int count=0; };
  std::unordered_map<long long, Accum> cells;
  std::unordered_map<size_t, long long> vertexCell; // map input vertex index -> cell key

  size_t vertCount = inMesh.positions.size();
  for(size_t i=0;i<vertCount;i++){
    glm::vec3 p = inMesh.positions[i];
    int cx = (int)floor(p.x / gridSize);
    int cy = (int)floor(p.y / gridSize);
    int cz = (int)floor(p.z / gridSize);
    long long key = PackCellKey(cx,cy,cz);
    vertexCell[i] = key;
    auto &a = cells[key];
    a.pos += p;
    if(i < inMesh.normals.size()) a.n += inMesh.normals[i];
    if(i < inMesh.uvs.size()) a.uv += inMesh.uvs[i];
    a.count++;
  }
  // create mapping from cell key to new vertex index
  std::unordered_map<long long, unsigned int> cellToNewIdx;
  cellToNewIdx.reserve(cells.size());
  for(auto &kv : cells){ long long key = kv.first; Accum &a = kv.second; unsigned int idx = (unsigned int)out.positions.size(); out.positions.push_back(a.pos / (float)a.count);
    if(a.count>0){ out.normals.push_back(glm::normalize(a.n / (float)a.count)); out.uvs.push_back(a.uv / (float)a.count); } else { out.normals.push_back(glm::vec3(0.0f)); out.uvs.push_back(glm::vec2(0.0f)); }
    cellToNewIdx[key] = idx; }
  // reindex triangles
  for(size_t t=0; t < inMesh.indices.size(); t+=3){ unsigned int i0 = inMesh.indices[t+0]; unsigned int i1 = inMesh.indices[t+1]; unsigned int i2 = inMesh.indices[t+2]; long long k0 = vertexCell[i0]; long long k1 = vertexCell[i1]; long long k2 = vertexCell[i2]; unsigned int n0 = cellToNewIdx[k0]; unsigned int n1 = cellToNewIdx[k1]; unsigned int n2 = cellToNewIdx[k2];
    // skip degenerate triangles
    if(n0==n1 || n1==n2 || n2==n0) continue;
    out.indices.push_back(n0); out.indices.push_back(n1); out.indices.push_back(n2);
  }
  return out;
}

// Minimal OBJ loader (supports only triangles, v, vn, vt, f with v/vt/vn or v//vn or v/vt)
bool LoadOBJ(const std::string& path, Mesh& outMesh){
  std::ifstream f(path);
  if(!f){ std::cerr<<"LoadOBJ: failed to open "<<path<<"\n"; return false; }
  std::vector<glm::vec3> temp_v; std::vector<glm::vec3> temp_n; std::vector<glm::vec2> temp_uv;
  outMesh.positions.clear(); outMesh.normals.clear(); outMesh.uvs.clear(); outMesh.indices.clear();
  std::string line;
  // For index parsing, we'll store a map from triple to index
  using Key = std::tuple<int,int,int>;
  struct KeyHash{ size_t operator()(Key const& k) const noexcept { auto [a,b,c]=k; return ((size_t)a*73856093u) ^ ((size_t)b*19349663u) ^ ((size_t)c*83492791u); } };
  std::unordered_map<Key, unsigned int, KeyHash> indexMap;
  while(std::getline(f,line)){
    if(line.size()<2) continue;
    std::istringstream ss(line);
    std::string prefix; ss>>prefix;
    if(prefix=="v"){ float x,y,z; ss>>x>>y>>z; temp_v.emplace_back(x,y,z); }
    else if(prefix=="vn"){ float x,y,z; ss>>x>>y>>z; temp_n.emplace_back(x,y,z); }
    else if(prefix=="vt"){ float u,v; ss>>u>>v; temp_uv.emplace_back(u,v); }
    else if(prefix=="f"){ std::string a,b,c; ss>>a>>b>>c; auto parseVert = [&](const std::string &s)->Key{
        int vi=0,ti=0,ni=0; // 1-based indices
        size_t p1 = s.find('/'); if(p1==std::string::npos){ vi = std::stoi(s); }
        else { size_t p2 = s.find('/', p1+1); vi = std::stoi(s.substr(0,p1)); if(p2==std::string::npos){ ti = std::stoi(s.substr(p1+1)); } else { if(p2==p1+1) ni = std::stoi(s.substr(p2+1)); else { ti = std::stoi(s.substr(p1+1, p2-p1-1)); ni = std::stoi(s.substr(p2+1)); } } }
        return std::make_tuple(vi, ti, ni);
      };
      Key ka = parseVert(a); Key kb = parseVert(b); Key kc = parseVert(c);
      Key keys[3] = {ka,kb,kc};
      for(int k=0;k<3;k++){
        Key kk = keys[k]; auto it = indexMap.find(kk); unsigned int idx;
        if(it==indexMap.end()){
          int vi = std::get<0>(kk); int ti = std::get<1>(kk); int ni = std::get<2>(kk);
          glm::vec3 pos(0.0f); glm::vec3 nor(0.0f); glm::vec2 uv(0.0f);
          if(vi!=0){ int iv = vi>0? vi-1 : (int)temp_v.size()+vi; if(iv>=0 && iv < (int)temp_v.size()) pos = temp_v[iv]; }
          if(ni!=0){ int in_ = ni>0? ni-1 : (int)temp_n.size()+ni; if(in_>=0 && in_ < (int)temp_n.size()) nor = temp_n[in_]; }
          if(ti!=0){ int itv = ti>0? ti-1 : (int)temp_uv.size()+ti; if(itv>=0 && itv < (int)temp_uv.size()) uv = temp_uv[itv]; }
          idx = (unsigned int)outMesh.positions.size(); outMesh.positions.push_back(pos); outMesh.normals.push_back(nor); outMesh.uvs.push_back(uv);
          indexMap[kk] = idx;
        } else idx = it->second;
        outMesh.indices.push_back(idx);
      }
    }
  }
  return true;
}

bool SaveOBJ(const std::string& path, const Mesh& mesh){
  std::ofstream f(path);
  if(!f) return false;
  for(auto &p : mesh.positions) f<<"v "<<p.x<<" "<<p.y<<" "<<p.z<<"\n";
  for(auto &uv : mesh.uvs) f<<"vt "<<uv.x<<" "<<uv.y<<"\n";
  for(auto &n : mesh.normals) f<<"vn "<<n.x<<" "<<n.y<<" "<<n.z<<"\n";
  // write faces using 1-based indices; assume positions/normals/uvs are same length
  for(size_t i=0;i<mesh.indices.size(); i+=3){ unsigned int a = mesh.indices[i]+1; unsigned int b = mesh.indices[i+1]+1; unsigned int c = mesh.indices[i+2]+1; f<<"f "<<a<<"/"<<a<<"/"<<a<<" "<<b<<"/"<<b<<"/"<<b<<" "<<c<<"/"<<c<<"/"<<c<<"\n"; }
  return true;
}

} // namespace coretech
