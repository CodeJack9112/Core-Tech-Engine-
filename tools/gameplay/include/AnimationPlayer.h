#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
namespace CoreTech {
struct AnimKey { float time; glm::vec3 pos; glm::quat rot; glm::vec3 scale; };
struct AnimClip { std::string name; float length=1.0f; std::vector<AnimKey> keys; };
class AnimationPlayer {
public:
    void play(const AnimClip& clip, bool loop=true);
    void update(float dt);
    float time=0.0f;
    bool playing=false;
    const AnimClip* current=nullptr;
};
}