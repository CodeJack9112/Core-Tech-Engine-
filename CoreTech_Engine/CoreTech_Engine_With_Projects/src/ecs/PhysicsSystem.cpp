
#include "PhysicsSystem.h"
#include <iostream>

void PhysicsSystem::update(float dt, ComponentManager& comps){
    // For entities with TransformComponent, apply simple gravity on Y and do simple ground collision at y=0
    comps.forEach<TransformComponent>([dt](uint32_t ent, TransformComponent& t){
        t.position.y += -9.8f * dt;
        if(t.position.y < 0.0f) t.position.y = 0.0f;
    });
    // naive collision: if two entities are at same x,z and close in y, print collision (demo)
    std::vector<std::pair<uint32_t, TransformComponent>> list;
    comps.forEach<TransformComponent>([&list](uint32_t ent, TransformComponent& t){ list.push_back({ent, t}); });
    for(size_t i=0;i<list.size();++i){
        for(size_t j=i+1;j<list.size();++j){
            auto &a = list[i].second; auto &b = list[j].second;
            float dx = a.position.x - b.position.x;
            float dz = a.position.z - b.position.z;
            float dist2 = dx*dx + dz*dz;
            if(dist2 < 0.01f && fabs(a.position.y - b.position.y) < 1.0f){
                std::cout << "[Physics] Entities " << list[i].first << " and " << list[j].first << " collided (demo)\\n";
            }
        }
    }
}
