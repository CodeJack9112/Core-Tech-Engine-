#include <functional>
#pragma once
#include <string>
#include <vector>

class RendererGL {
public:
    RendererGL() = default;
    ~RendererGL();

    // Initialize GLFW and GLAD, create window. Returns false on failure.
    bool initialize(int width = 1280, int height = 720, const char* title = "CoreTech GL Demo");

    // Main loop (blocks)
    void runMainLoop(class PhysicsManager* phys);

    // Shutdown
    void shutdown();

    // Load and compile GLSL shader program from vertex/fragment paths
    bool loadAndCompile(const std::string& vsPath, const std::string& fsPath);

    // Simple setters
    void setViewPos(float x, float y, float z);

private:
    void renderFrame();
    bool compileShader(const std::string& src, unsigned int shaderType, unsigned int &outShader);
    unsigned int m_shaderProgram = 0;
    void* m_window = nullptr; // GLFWwindow*
    float m_viewPos[3] = {0.0f,0.0f,5.0f};
    unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
    unsigned int m_gBuffer = 0;
    unsigned int m_gPosition = 0, m_gNormal = 0, m_gAlbedo = 0;
    unsigned int m_rboDepth = 0;
    unsigned int m_geomProgram = 0;
    unsigned int m_compProgram = 0;

    unsigned int m_quadVAO = 0, m_quadVBO = 0, m_quadTexture = 0;
    int m_width = 1280, m_height = 720;
    void createDemoMesh();
    void destroyDemoMesh();
};

extern RendererGL g_renderer_global;
