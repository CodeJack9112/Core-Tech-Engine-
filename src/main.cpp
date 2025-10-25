#include "Engine/Engine.h"
#include "EngineModules/RendererGL.h"
#include "EngineModules/ECSModule.h"
#include "EngineModules/PhysicsModule.h"
#include "EngineModules/AudioModule.h"
#include "EngineModules/InputModule.h"
#include "Scene/Scene.h"
#include <iostream>

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    using namespace coretech;
    Engine& eng = Engine::instance();

    static RendererGL renderer;
    static ECSModule ecs;
    static PhysicsModule physics;
    static AudioModule audio;
    static InputModule input;

    eng.registerModule(&renderer);
    eng.registerModule(&physics);
    eng.registerModule(&audio);
    eng.registerModule(&input);
    eng.registerModule(&ecs); // ECS can be last so other modules are ready

    if(!eng.startup()) {
        std::cerr << "Engine failed to start\n";
        return -1;
    }

    // create sample entity
    auto id = ecs.entities().create();
    coretech::RenderableComponent rc; rc.meshPath = "assets/default.mesh"; rc.material = "default";
    ecs.components().add<coretech::RenderableComponent>(id, rc);
    coretech::TransformComponent tc; tc.position[0]=0.0f; tc.position[1]=0.0f; tc.position[2]=0.0f;
    ecs.components().add<coretech::TransformComponent>(id, tc);

    // Load scene
    Scene scene;
    scene.loadFromFile("assets/scene.txt");

    // Run main loop under Engine control (600 frames ~ 10 seconds)
    eng.run(600, 60.0f);

    eng.shutdown();
    return 0;
}
