#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// Minimal Renderer stub
#include "Material.h" with shader loading support.
// Adds Unreal Engine 3 style Blinn-Phong shader (GLSL) files to the project.
// This renderer assumes an OpenGL-like backend; it's a lightweight integration
// so you can replace the internals with your preferred graphics API.

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    // Load shader sources into memory (from files). Returns true if files found.
    bool loadShaders(const std::string& vsPath, const std::string& fsPath);

    // Get shader source strings (vertex/fragment)
    const std::string& vertexSource() const { return m_vertexSrc; }
    const std::string& fragmentSource() const { return m_fragmentSrc; }

    // Compile shaders (no-op if no GL). Provided as convenience; fails gracefully if no GL context.
    bool compileShaders();

    // HLSL support: load HLSL shader file contents (for DirectX backends). Stores source string for later use.
    bool loadHLSLShader(const std::string& path);
    const std::string& hlslSource() const { return m_hlslSrc; }


    // Simple set of uniform-like parameters for demoing Blinn-Phong lighting
    void setLightDirection(float x, float y, float z);
    void setSpecularPower(float power);
    void setSpecularIntensity(float intensity);

    // Material / texture setters (stubs; real implementation requires GL texture loading)
    bool loadAlbedoTexture(const std::string& path);
    bool loadNormalTexture(const std::string& path);
    void setMaterial(const Material& mat);
    Material currentMaterial() const { return m_material; }

private:
    std::string m_hlslSrc;
    std::string m_vertexSrc;
    std::string m_fragmentSrc;

    // Parameters
    float m_lightDir[3] = {0.0f, -1.0f, 0.0f};
    float m_specPower = 32.0f;
    float m_specIntensity = 1.0f;
    Material m_material;
};
