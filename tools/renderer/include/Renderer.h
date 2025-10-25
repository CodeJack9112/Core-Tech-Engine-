#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace CoreTech {
class Renderer {
public:
    void setBoneMatrices(const std::vector<float>& mats);
public:
    bool loadDefaultResources();
    void drawMesh(const glm::mat4& view, const glm::mat4& proj, const glm::mat4& model);
    void reloadShadersIfNeeded();
    bool compileShader(const std::string& vsSrc, const std::string& fsSrc, unsigned int &outProgram);

public:
    static Renderer& instance();
    bool init(int width, int height);
    void resize(int w, int h);
    void beginFrame();
    void endFrame();
    // simple draw functions
    void drawCube(const glm::mat4& view, const glm::mat4& proj, const glm::mat4& model);
private:
    Renderer();
    struct Impl;
    Impl* p;
};
} // namespace CoreTech
