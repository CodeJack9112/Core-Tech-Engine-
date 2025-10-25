#pragma once
#include <vector>
#include <unordered_map>
#include <btBulletDynamicsCommon.h>

struct NavNode {
    int id;
    btVector3 pos;
    std::vector<int> neighbors;
};

class NavMesh {
public:
    NavMesh() {}
    ~NavMesh() {}

    // Build graph from node positions: connect nodes within radius
    void build(const std::vector<btVector3>& nodes, float connectRadius);

    // Find path between two points (closest node to start/goal) using A*
    // Returns list of world positions representing the path (empty if not found)
    std::vector<btVector3> findPath(const btVector3& start, const btVector3& goal) const;

    const std::vector<NavNode>& nodes() const { return m_nodes; }

private:
    std::vector<NavNode> m_nodes;

    int findClosestNode(const btVector3& p) const;
    double heuristic(int a, int b) const;
};
