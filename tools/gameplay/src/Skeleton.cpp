#include "Skeleton.h"
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
namespace CoreTech {

glm::quat AnimatorSystem::slerp(const glm::quat& a, const glm::quat& b, float t){
    return glm::slerp(a,b,t);
}
glm::vec3 AnimatorSystem::lerp(const glm::vec3& a, const glm::vec3& b, float t){
    return a + (b-a)*t;
}
static AnimKey sampleKey(const std::vector<AnimKey>& keys, float t){
    if(keys.empty()) return AnimKey();
    if(keys.size()==1) return keys[0];
    // find interval
    if(t <= keys.front().time) return keys.front();
    if(t >= keys.back().time) return keys.back();
    for(size_t i=0;i+1<keys.size();++i){
        if(t >= keys[i].time && t <= keys[i+1].time){
            float dt = keys[i+1].time - keys[i].time;
            float local = (t - keys[i].time) / (dt>0?dt:1.0f);
            AnimKey out;
            out.time = t;
            out.translation = keys[i].translation + (keys[i+1].translation - keys[i].translation) * local;
            out.rotation = glm::slerp(keys[i].rotation, keys[i+1].rotation, local);
            out.scale = keys[i].scale + (keys[i+1].scale - keys[i].scale) * local;
            return out;
        }
    }
    return keys.back();
}

void AnimatorSystem::playClip(const std::shared_ptr<Entity>& e, const AnimClip& clip, bool loop){
    PlayInstance pi; pi.entity = e; pi.clip = clip; pi.time = 0.0f; pi.loop = loop;
    instances.push_back(pi);
}

void AnimatorSystem::update(float dt){
    for(auto &inst : instances){
        inst.time += dt;
        if(inst.time > inst.clip.length){
            if(inst.loop) inst.time = fmod(inst.time, inst.clip.length);
            else inst.time = inst.clip.length;
        }
        float t = inst.time;
        // apply per-bone animations by updating TransformComponents of entities that represent bones (simplified)
        for(const auto& ban : inst.clip.boneAnims){
            AnimKey k = sampleKey(ban.keys, t);
            // find bone entity or apply to owner entity transform if single-bone model
            if(auto ent = inst.entity.lock()){
                auto boneEnt = ent; // simplistic: apply to entity transform as example
                if(boneEnt){
                    auto tcomp = boneEnt->getComponent<TransformComponent>();
                    if(tcomp){
                        tcomp->x = k.translation.x; tcomp->y = k.translation.y; tcomp->z = k.translation.z;
                        glm::vec3 euler = glm::degrees(glm::eulerAngles(k.rotation));
                        tcomp->rx = euler.x; tcomp->ry = euler.y; tcomp->rz = euler.z;
                        tcomp->sx = k.scale.x; tcomp->sy = k.scale.y; tcomp->sz = k.scale.z;
                    }
                }
            }
        }
    }
}

void AnimatorSystem::applyBonePose(const PlayInstance& inst, const BoneAnim& ban, float tnorm){
    (void)inst;(void)ban;(void)tnorm;
    // advanced skinning not implemented in this prototype
}

}