#pragma once
#include <string>
#include <array>

// Simple UE3-like material description (data-driven)
// This is a lightweight representation for demo purposes.
struct Material {
    std::string name;
    // Base color (albedo)
    std::array<float,3> albedo = {1.0f,1.0f,1.0f};
    // Specular properties
    float specPower = 32.0f;
    float specIntensity = 1.0f;
    // Roughness approx (in UE3 this was less formal)
    float roughness = 0.5f;
    // Texture paths (optional)
    std::string albedoTexture;
    std::string normalTexture;
    std::string specularTexture;
};
