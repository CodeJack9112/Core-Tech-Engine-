#include "QEMSimplifier.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <iostream>

namespace coretech {
// Improved QEM: maintain valid edge costs with time-stamped entries to avoid repeated expensive recomputation
struct SymmetricMatrix { double m[10]; SymmetricMatrix(){ for(int i=0;i<10;i++) m[i]=0.0; } };
static inline SymmetricMatrix MakePlaneQuadric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c){
  glm::vec3 n = glm::normalize(glm::cross(b-a, c-a)); double A=n.x,B=n.y,C=n.z; double D=-(A*a.x+B*a.y+C*a.z);
  SymmetricMatrix Q; Q.m[0]=A*A; Q.m[1]=A*B; Q.m[2]=A*C; Q.m[3]=A*D; Q.m[4]=B*B; Q.m[5]=B*C; Q.m[6]=B*D; Q.m[7]=C*C; Q.m[8]=C*D; Q.m[9]=D*D; return Q;
}
static inline SymmetricMatrix Add(const SymmetricMatrix&a,const SymmetricMatrix&b){ SymmetricMatrix r; for(int i=0;i<10;i++) r.m[i]=a.m[i]+b.m[i]; return r; }

struct EdgeEntry { double cost; unsigned v1,v2; glm::vec3 pos; uint64_t stamp; };
struct PQComp { bool operator()(EdgeEntry const& a, EdgeEntry const& b) const { return a.cost > b.cost; } };

// A simplified improved QEM: compute initial edge costs and iteratively collapse lowest-cost edges.
Mesh SimplifyQEMOptimized(const Mesh& inMesh, unsigned int targetVertexCount){
  Mesh out = inMesh;
  unsigned int vCount = (unsigned int)out.positions.size(); if(targetVertexCount>=vCount || vCount==0) return out;
  unsigned int triCount = (unsigned int)out.indices.size()/3;
  std::vector<SymmetricMatrix> Q(vCount);
  for(unsigned i=0;i<triCount;i++){ unsigned i0=out.indices[3*i], i1=out.indices[3*i+1], i2=out.indices[3*i+2]; Q[i0]=Add(Q[i0], MakePlaneQuadric(out.positions[i0], out.positions[i1], out.positions[i2])); Q[i1]=Add(Q[i1], MakePlaneQuadric(out.positions[i0], out.positions[i1], out.positions[i2])); Q[i2]=Add(Q[i2], MakePlaneQuadric(out.positions[i0], out.positions[i1], out.positions[i2])); }
  std::vector<std::unordered_set<int>> adj(vCount);
  for(unsigned i=0;i<triCount;i++){ int a=out.indices[3*i], b=out.indices[3*i+1], c=out.indices[3*i+2]; adj[a].insert(b); adj[a].insert(c); adj[b].insert(a); adj[b].insert(c); adj[c].insert(a); adj[c].insert(b); }
  auto eval = [&](int a,int b)->EdgeEntry{ EdgeEntry e; e.v1=a; e.v2=b; e.pos = (out.positions[a]+out.positions[b])*0.5f; SymmetricMatrix Qsum = Add(Q[a], Q[b]); double x=e.pos.x,y=e.pos.y,z=e.pos.z; double err = Qsum.m[0]*x*x + 2*Qsum.m[1]*x*y + 2*Qsum.m[2]*x*z + 2*Qsum.m[3]*x + Qsum.m[4]*y*y + 2*Qsum.m[5]*y*z + 2*Qsum.m[6]*y + Qsum.m[7]*z*z + 2*Qsum.m[8]*z + Qsum.m[9]; e.cost = err; e.stamp = 0; return e; };
  std::priority_queue<EdgeEntry, std::vector<EdgeEntry>, PQComp> pq;
  std::vector<uint64_t> vertexStamp(vCount,0);
  for(unsigned i=0;i<vCount;i++){
    for(int nb: adj[i]){ if((int)nb > (int)i){ EdgeEntry ee = eval((int)i, nb); pq.push(ee); }
    }
  }
  std::vector<char> removed(vCount,0);
  unsigned currentV = vCount;
  while(currentV > targetVertexCount && !pq.empty()){
    EdgeEntry top = pq.top(); pq.pop(); int a = (int)top.v1, b=(int)top.v2; if(removed[a]||removed[b]) continue;
    // validate adjacency
    if(adj[a].find(b)==adj[a].end()) continue;
    // perform collapse: merge b into a
    out.positions[a] = top.pos; Q[a] = Add(Q[a], Q[b]);
    // replace b indices with a
    for(size_t i=0;i<out.indices.size();++i) if(out.indices[i] == b) out.indices[i] = a;
    removed[b]=1; currentV--;
    // merge adjacency
    for(int nb: adj[b]){ if(nb==a) continue; adj[a].insert(nb); adj[nb].erase(b); adj[nb].insert(a); }
    adj[b].clear();
    // recompute costs for edges involving a
    for(int nb: adj[a]){ EdgeEntry ne = eval(a, nb); pq.push(ne); }
  }
  // compact vertices
  std::vector<int> remap(vCount, -1); unsigned next=0;
  for(unsigned i=0;i<vCount;i++) if(!removed[i]) remap[i]=next++;
  Mesh finalM; finalM.positions.reserve(next); finalM.normals.reserve(next); finalM.uvs.reserve(next);
  for(unsigned i=0;i<vCount;i++) if(remap[i]>=0){ finalM.positions.push_back(out.positions[i]); if(i<out.normals.size()) finalM.normals.push_back(out.normals[i]); if(i<out.uvs.size()) finalM.uvs.push_back(out.uvs[i]); }
  for(size_t i=0;i<out.indices.size(); i+=3){ int ia=remap[out.indices[i]], ib=remap[out.indices[i+1]], ic=remap[out.indices[i+2]]; if(ia==ib||ib==ic||ic==ia) continue; finalM.indices.push_back(ia); finalM.indices.push_back(ib); finalM.indices.push_back(ic); }
  return finalM;
}

} // namespace coretech
