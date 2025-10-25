#pragma once
#include <array>
struct TransformComponent {
    float position[3] = {0.0f,0.0f,0.0f};
    float rotation[3] = {0.0f,0.0f,0.0f};
    float scale[3] = {1.0f,1.0f,1.0f};
};