#pragma once
#include <array>
struct CameraComponent {
    float position[3] = {0.0f, 0.0f, 5.0f};
    float target[3] = {0.0f, 0.0f, 0.0f};
    float up[3] = {0.0f, 1.0f, 0.0f};
    float fov = 45.0f;
    float aspect = 16.0f/9.0f;
    float nearZ = 0.1f;
    float farZ = 100.0f;
};