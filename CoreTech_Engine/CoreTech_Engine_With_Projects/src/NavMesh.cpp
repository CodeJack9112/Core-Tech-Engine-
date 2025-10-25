#include "../include/NavMesh.h"
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>

void NavMesh::build(const std::vector<btVector3>& nodes, float connectRadius){
    m_nodes.clear();
    int n = (int)nodes.size();
    m_nodes.reserve(n);
    for(int i=0;i<n;i++){
        NavNode node; node.id = i; node.pos = nodes[i]; m_nodes.push_back(node);
    }
    float r2 = connectRadius*connectRadius;
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            float dx = m_nodes[i].pos.getX() - m_nodes[j].pos.getX();
            float dz = m_nodes[i].pos.getZ() - m_nodes[j].pos.getZ();
            float dy = m_nodes[i].pos.getY() - m_nodes[j].pos.getY();
            float dist2 = dx*dx + dy*dy + dz*dz;
            if(dist2 <= r2){
                m_nodes[i].neighbors.push_back(j);
                m_nodes[j].neighbors.push_back(i);
            }
        }
    }
}

int NavMesh::findClosestNode(const btVector3& p) const {
    int best = -1;
    double bestd = 1e30;
    for(const auto &n : m_nodes){
        double dx = n.pos.getX()-p.getX();
        double dy = n.pos.getY()-p.getY();
        double dz = n.pos.getZ()-p.getZ();
        double d = dx*dx+dy*dy+dz*dz;
        if(d < bestd){ bestd = d; best = n.id; }
    }
    return best;
}

double NavMesh::heuristic(int a, int b) const {
    const btVector3 &pa = m_nodes[a].pos;
    const btVector3 &pb = m_nodes[b].pos;
    double dx = pa.getX()-pb.getX();
    double dy = pa.getY()-pb.getY();
    double dz = pa.getZ()-pb.getZ();
    return std::sqrt(dx*dx+dy*dy+dz*dz);
}

std::vector<btVector3> NavMesh::findPath(const btVector3& start, const btVector3& goal) const {
    std::vector<btVector3> empty;
    if(m_nodes.empty()) return empty;
    int s = findClosestNode(start);
    int g = findClosestNode(goal);
    if(s<0 || g<0) return empty;
    // A* search
    int n = (int)m_nodes.size();
    std::vector<double> gscore(n, std::numeric_limits<double>::infinity());
    std::vector<double> fscore(n, std::numeric_limits<double>::infinity());
    std::vector<int> cameFrom(n, -1);
    struct PQItem { int id; double f; };
    struct Cmp { bool operator()(const PQItem&a,const PQItem&b) const { return a.f > b.f; } };
    std::priority_queue<PQItem, std::vector<PQItem>, Cmp> open;
    gscore[s] = 0.0;
    fscore[s] = heuristic(s,g);
    open.push({s,fscore[s]});

    while(!open.empty()){
        int current = open.top().id; open.pop();
        if(current == g) break;
        for(int nei : m_nodes[current].neighbors){
            double tentative = gscore[current] + heuristic(current, nei);
            if(tentative < gscore[nei]){
                cameFrom[nei] = current;
                gscore[nei] = tentative;
                fscore[nei] = tentative + heuristic(nei, g);
                open.push({nei, fscore[nei]});
            }
        }
    }
    // reconstruct path
    std::vector<int> rev;
    int cur = g;
    if(cameFrom[cur] == -1 && cur != s) return empty;
    while(cur != -1){
        rev.push_back(cur);
        cur = cameFrom[cur];
    }
    std::vector<btVector3> path;
    for(auto it = rev.rbegin(); it!=rev.rend(); ++it){
        path.push_back(m_nodes[*it].pos);
    }
    return path;
}
