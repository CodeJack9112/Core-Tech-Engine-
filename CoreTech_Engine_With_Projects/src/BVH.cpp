#include "BVH.h"
#include <algorithm>
void BVH::Build(const std::vector<glm::vec3>& primCenters){ nodes.clear(); if(primCenters.empty()) return; struct Item{glm::vec3 p; int idx;}; std::vector<Item> items; items.reserve(primCenters.size()); for(size_t i=0;i<primCenters.size();++i) items.push_back({primCenters[i], (int)i});
// simple recursive median split
std::function<int(int,int)> buildRec = [&](int l, int r)->int{
    int nodeIdx = nodes.size(); nodes.push_back({{0,0,0},{0,0,0},-1,-1,l,r-l});
    if(r-l <= 2) { nodes[nodeIdx].firstPrim = l; nodes[nodeIdx].primCount = r-l; return nodeIdx; }
    // find extent
    glm::vec3 min(1e9f), max(-1e9f);
    for(int i=l;i<r;i++){ min = glm::min(min, items[i].p); max = glm::max(max, items[i].p); }
    glm::vec3 ext = max - min; int axis = (ext.x>ext.y?(ext.x>ext.z?0:2):(ext.y>ext.z?1:2));
    std::sort(items.begin()+l, items.begin()+r, [&](const Item&a,const Item&b){ return a.p[axis] < b.p[axis]; });
    int m = (l+r)/2;
    int left = buildRec(l,m);
    int right = buildRec(m,r);
    nodes[nodeIdx].left = left; nodes[nodeIdx].right = right; nodes[nodeIdx].primCount = 0; nodes[nodeIdx].firstPrim = -1; return nodeIdx;
};
buildRec(0,(int)items.size()); }
