#include "Octree.h"
#include <cmath>
#include <algorithm>
#include <queue>
Octree::Node::Node(): center(0.0f), halfSize(0.0f) { for(int i=0;i<8;i++) children[i]=nullptr; }
Octree::Node::~Node(){ for(int i=0;i<8;i++) if(children[i]) delete children[i]; }
Octree::Octree(const glm::vec3& c, float h){ root = new Node(); root->center = c; root->halfSize = h; }
Octree::~Octree(){ delete root; }
static int ChildIndex(const glm::vec3& c, const glm::vec3& p){ int idx=0; if(p.x>c.x) idx|=1; if(p.y>c.y) idx|=2; if(p.z>c.z) idx|=4; return idx; }
void Octree::Insert(int objectId, const glm::vec3& pos){ Node* n = root; int depth=0; while(depth<maxDepth){ if(n->objects.size()< (size_t)maxObjects) { n->objects.push_back(objectId); return; } int idx = ChildIndex(n->center,pos); if(!n->children[idx]){ n->children[idx]=new Node(); n->children[idx]->halfSize = n->halfSize*0.5f; n->children[idx]->center = n->center + glm::vec3(((idx&1)?0.5f:-0.5f)*n->halfSize, ((idx&2)?0.5f:-0.5f)*n->halfSize, ((idx&4)?0.5f:-0.5f)*n->halfSize); } n = n->children[idx]; depth++; }
n->objects.push_back(objectId);
}
void Octree::QueryRange(const glm::vec3& center, float radius, std::vector<int>& out){ std::queue<Node*> q; q.push(root); while(!q.empty()){ Node* n=q.front(); q.pop(); float dist = glm::distance(n->center, center); if(dist - n->halfSize > radius) continue; for(int id: n->objects) out.push_back(id); for(int i=0;i<8;i++) if(n->children[i]) q.push(n->children[i]); } }
