#pragma once
#include <memory>

namespace CoreTech {
class GBuffer;
class Scene;
class Camera;

class RendererDeferred {
public:
    RendererDeferred();
    bool Initialize(void* window);
    void RenderScene(Scene* scene, Camera* camera);
    void Shutdown();

private:
    std::unique_ptr<GBuffer> gbuffer;
    bool initialized;
};
}