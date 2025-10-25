#include "QEMSimplifier.h"
#include <queue>
#include <set>
#include <unordered_map>
#include <limits>
#include <iostream>

namespace coretech {

struct SymmetricMatrix { double m[10]; SymmetricMatrix(){ for(int i=0;i<10;i++) m[i]=0.0; } SymmetricMatrix(double a){ for(int i=0;i<10;i++) m[i]=a; } };

static inline SymmetricMatrix MakePlaneQuadric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c){
  glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
  double A = n.x, B = n.y, C = n.z; double D = - (A*a.x + B*a.y + C*a.z);
  SymmetricMatrix Q;
  Q.m[0] = A*A; Q.m[1] = A*B; Q.m[2] = A*C; Q.m[3] = A*D;
  Q.m[4] = B*B; Q.m[5] = B*C; Q.m[6] = B*D;
  Q.m[7] = C*C; Q.m[8] = C*D; Q.m[9] = D*D;
  return Q;
}
static inline SymmetricMatrix Add(const SymmetricMatrix&a,const SymmetricMatrix&b){ SymmetricMatrix r; for(int i=0;i<10;i++) r.m[i]=a.m[i]+b.m[i]; return r; }

struct EdgeCollapse { double cost; unsigned int v1,v2; glm::vec3 pos; };

Mesh SimplifyQEM(const Mesh& inMesh, unsigned int targetVertexCount){
  Mesh out = inMesh; unsigned int vCount = (unsigned int)out.positions.size();
  if(targetVertexCount >= vCount || vCount==0) return out;
  unsigned int triCount = (unsigned int)out.indices.size()/3;
  // build initial quadrics per vertex
  std::vector<SymmetricMatrix> Q(vCount);
  for(unsigned i=0;i<triCount;i++){
    unsigned int i0 = out.indices[3*i+0]; unsigned int i1 = out.indices[3*i+1]; unsigned int i2 = out.indices[3*i+2];
    glm::vec3 a = out.positions[i0]; glm::vec3 b = out.positions[i1]; glm::vec3 c = out.positions[i2];
    SymmetricMatrix q = MakePlaneQuadric(a,b,c);
    Q[i0] = Add(Q[i0], q); Q[i1] = Add(Q[i1], q); Q[i2] = Add(Q[i2], q);
  }
  // adjacency list
  std::vector<std::set<int>> adj(vCount);
  for(unsigned i=0;i<triCount;i++){ int a=out.indices[3*i+0], b=out.indices[3*i+1], c=out.indices[3*i+2]; adj[a].insert(b); adj[a].insert(c); adj[b].insert(a); adj[b].insert(c); adj[c].insert(a); adj[c].insert(b); }
  // compute initial costs (naive: edge pair cost evaluated at midpoint)
  auto eval_collapse = [&](int v1,int v2)->EdgeCollapse{
    EdgeCollapse ec; ec.v1=v1; ec.v2=v2; glm::vec3 p = (out.positions[v1] + out.positions[v2]) * 0.5f; ec.pos = p;
    // compute error = p^T * Q * p
    double x=p.x, y=p.y, z=p.z; double v[4]={x,y,z,1.0}; SymmetricMatrix Qsum = Add(Q[v1], Q[v2]);
    double err = Qsum.m[0]*x*x + 2*Qsum.m[1]*x*y + 2*Qsum.m[2]*x*z + 2*Qsum.m[3]*x*1 + Qsum.m[4]*y*y + 2*Qsum.m[5]*y*z + 2*Qsum.m[6]*y*1 + Qsum.m[7]*z*z + 2*Qsum.m[8]*z*1 + Qsum.m[9]*1*1;
    ec.cost = err; return ec; };
  // priority queue
  auto cmp = [](const EdgeCollapse&a,const EdgeCollapse&b){ return a.cost > b.cost; };
  std::priority_queue<EdgeCollapse, std::vector<EdgeCollapse>, decltype(cmp)> pq(cmp);
  std::vector<std::unordered_map<int,double>> bestCost(vCount);
  for(unsigned i=0;i<vCount;i++){
    for(int nb: adj[i]){ EdgeCollapse ec = eval_collapse(i, nb); pq.push(ec); bestCost[i][nb]=ec.cost; }
  }
  std::vector<char> removed(vCount,0);
  unsigned int currentV = vCount;
  while(currentV > targetVertexCount && !pq.empty()){
    EdgeCollapse ec = pq.top(); pq.pop(); if(removed[ec.v1] || removed[ec.v2]) continue; // invalid
    // collapse v2 into v1 (naive)
    unsigned int a = ec.v1, b = ec.v2;
    // move position to ec.pos
    out.positions[a] = ec.pos; // merge
    // merge quadrics
    Q[a] = Add(Q[a], Q[b]);
    // reassign faces: replace b with a
    for(size_t i=0;i<out.indices.size();++i) if(out.indices[i]==(int)b) out.indices[i]=a;
    // mark b removed
    removed[b]=1; currentV--;
    // clean adjacency and rebuild neighbors list (naive approach)
    adj[a].erase(b);
    for(int nb: adj[b]){ if(nb==a) continue; adj[a].insert(nb); adj[nb].erase(b); adj[nb].insert(a); }
    adj[b].clear();
    // push new collapse candidates for a
    for(int nb: adj[a]){ EdgeCollapse nec = eval_collapse((int)a, nb); pq.push(nec); }
  }
  // remove degenerate triangles and remap indices
  std::vector<int> remap(vCount, -1); unsigned int next=0;
  for(unsigned i=0;i<vCount;i++) if(!removed[i]) remap[i]=next++;
  Mesh finalM; finalM.positions.reserve(next); finalM.normals.reserve(next); finalM.uvs.reserve(next);
  for(unsigned i=0;i<vCount;i++) if(remap[i]>=0){ finalM.positions.push_back(out.positions[i]); if(i < out.normals.size()) finalM.normals.push_back(out.normals[i]); if(i < out.uvs.size()) finalM.uvs.push_back(out.uvs[i]); }
  for(size_t i=0;i<out.indices.size(); i+=3){ int ia=remap[out.indices[i+0]]; int ib=remap[out.indices[i+1]]; int ic=remap[out.indices[i+2]]; if(ia==ib || ib==ic || ic==ia) continue; finalM.indices.push_back((unsigned int)ia); finalM.indices.push_back((unsigned int)ib); finalM.indices.push_back((unsigned int)ic); }
  return finalM;
}

} // namespace coretech
