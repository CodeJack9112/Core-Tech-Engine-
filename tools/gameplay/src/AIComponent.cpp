#include "AIComponent.h"
#include "Entity.h"
#include <iostream>
namespace CoreTech {
void AISystem::update(std::shared_ptr<Scene> scene, float dt){
    for(auto &e : scene->getAllEntities()){
        auto ai = e->getComponent<AIComponent>();
        auto t = e->getComponent<TransformComponent>();
        if(!ai || !t) continue;
        if(ai->state == AIComponent::IDLE){
            // idle - small oscillation
            t->x += 0.0f;
        } else if(ai->state == AIComponent::SEEK){
            auto target = scene->getEntity(ai->targetId);
            if(target){
                auto tt = target->getComponent<TransformComponent>();
                if(tt){
                    float vx = tt->x - t->x;
                    float vz = tt->z - t->z;
                    float len = sqrt(vx*vx+vz*vz);
                    if(len > 0.001f){
                        t->x += (vx/len) * ai->speed * dt;
                        t->z += (vz/len) * ai->speed * dt;
                    }
                }
            }
        }
    }
} }