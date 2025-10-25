#include "RenderGraph.h"
#include <queue>
#include <iostream>
int RenderGraph::AddResource(const RGResource& r){ resources[r.name]=r; return (int)resources.size()-1; }
int RenderGraph::AddNode(const RGNode& n){ nodes.push_back(n); return (int)nodes.size()-1; }
bool RenderGraph::Compile(std::vector<int>& outOrder){ int n = nodes.size(); std::vector<int> indeg(n,0); for(int i=0;i<n;i++) for(int d: nodes[i].deps) if(d>=0 && d<n) indeg[i]++; std::queue<int> q; for(int i=0;i<n;i++) if(indeg[i]==0) q.push(i); while(!q.empty()){ int v=q.front(); q.pop(); outOrder.push_back(v); for(int i=0;i<n;i++){ for(int d: nodes[i].deps) if(d==v){ indeg[i]--; if(indeg[i]==0) q.push(i); } } } if((int)outOrder.size()!=n){ std::cerr<<"RenderGraph: cycle detected or missing deps"<<std::endl; return false; } return true; }
void RenderGraph::Execute(const std::vector<int>& order){ for(int idx: order){ auto &n = nodes[idx]; if(n.execute) n.execute(); } }
