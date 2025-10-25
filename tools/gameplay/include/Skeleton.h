#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace CoreTech {
struct Bone {
    std::string name;
    int parent = -1;
    glm::mat4 inverseBind; // inverse bind matrix
};
struct Skeleton {
    std::vector<Bone> bones;
};
struct SkinComponent : public Component {
    // per-vertex skinning not implemented; instead we update per-bone transforms and leave mesh skinning to renderer integration
    std::shared_ptr<Skeleton> skeleton;
    std::string getType() const override { return "Skin"; }
};
struct AnimKey {
    float time = 0.0f;
    glm::vec3 translation = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1,0,0,0);
    glm::vec3 scale = glm::vec3(1.0f);
};
struct BoneAnim {
    int boneIndex = -1;
    std::vector<AnimKey> keys;
};
struct AnimClip {
    std::string name;
    float length = 0.0f;
    std::vector<BoneAnim> boneAnims;
};
class AnimatorSystem {
public:
    void playClip(const std::shared_ptr<Entity>& e, const AnimClip& clip, bool loop=true);
    void update(float dt);
private:
    struct PlayInstance { std::weak_ptr<Entity> entity; AnimClip clip; float time=0.0f; bool loop=true; };
    std::vector<PlayInstance> instances;
    glm::quat slerp(const glm::quat& a, const glm::quat& b, float t);
    glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float t);
    void applyBonePose(const PlayInstance& inst, const BoneAnim& ban, float tnorm);
};
}