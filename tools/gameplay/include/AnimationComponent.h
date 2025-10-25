#pragma once
#include "Entity.h"
#include <vector>
#include <string>
namespace CoreTech {
struct AnimationKey { float time; float x,y,z; };
struct AnimationComponent : public Component {
    std::string name;
    float length = 1.0f;
    std::vector<AnimationKey> keys;
    std::string getType() const override { return "Animation"; }
};
class Animator {
public:
    void update(std::shared_ptr<Scene> scene, float dt){
        // Simple animation: move entities with AnimationComponent along first keyframe progression (stub)
        for(auto &e : scene->getAllEntities()){
            auto anim = e->getComponent<AnimationComponent>();
            auto t = e->getComponent<TransformComponent>();
            if(anim && t && anim->keys.size()>0){
                // loop time progress
                animTime += dt;
                float tt = fmod(animTime, anim->length);
                // simple sample: use first key as target
                t->x = anim->keys[0].x; t->y = anim->keys[0].y; t->z = anim->keys[0].z;
            }
        }
    }
private:
    float animTime = 0.0f;
};
}