#include "AnimationPlayer.h"
namespace CoreTech {
void AnimationPlayer::play(const AnimClip& clip, bool loop){
    current = &clip;
    time = 0.0f;
    playing = true;
}
void AnimationPlayer::update(float dt){
    if(!playing || !current) return;
    time += dt;
    if(time > current->length) time = fmod(time, current->length);
    // apply keys to entity transforms (not implemented - contributor to hook per-entity)
}
}