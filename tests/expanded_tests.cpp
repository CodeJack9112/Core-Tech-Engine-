#include <catch2/catch.hpp>
#include "Engine/Engine.h"
#include "EngineModules/ECSModule.h"
#include "EngineModules/RendererGL.h"
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"
#include "ecs/CameraComponent.h"

TEST_CASE("ECS create and component isolation", "[ecs]") {
    using namespace coretech;
    ECSModule ecs;
    REQUIRE(ecs.init() == true);
    auto e1 = ecs.entities().create();
    auto e2 = ecs.entities().create();
    // Add components only to e1
    TransformComponent t1; t1.position[0]=1.0f;
    ecs.components().add<TransformComponent>(e1, t1);
    RenderableComponent r1; r1.meshPath = "meshA"; r1.material = "matA";
    ecs.components().add<RenderableComponent>(e1, r1);

    // e2 should not have these components
    auto ge2_t = ecs.components().get<TransformComponent>(e2);
    auto ge2_r = ecs.components().get<RenderableComponent>(e2);
    REQUIRE(ge2_t == nullptr);
    REQUIRE(ge2_r == nullptr);

    // e1 should have them
    auto ge1_t = ecs.components().get<TransformComponent>(e1);
    auto ge1_r = ecs.components().get<RenderableComponent>(e1);
    REQUIRE(ge1_t != nullptr);
    REQUIRE(ge1_r != nullptr);
    REQUIRE(ge1_t->position[0] == Approx(1.0f));

    ecs.shutdown();
}

TEST_CASE("ECS movement update affects TransformComponent", "[ecs]") {
    using namespace coretech;
    ECSModule ecs;
    REQUIRE(ecs.init() == true);
    auto id = ecs.entities().create();
    TransformComponent tc; tc.position[0] = 0.0f;
    ecs.components().add<TransformComponent>(id, tc);

    // call update which (in current implementation) moves x by +0.01f per update
    ecs.onUpdate(1.0f/60.0f);
    auto got = ecs.components().get<TransformComponent>(id);
    REQUIRE(got != nullptr);
    REQUIRE(got->position[0] == Approx(0.01f).epsilon(1e-3));

    ecs.shutdown();
}

TEST_CASE("Renderer draw invocation via Engine modules", "[renderer]") {
    using namespace coretech;
    Engine& eng = Engine::instance();
    // ensure a fresh engine state (best effort)
    eng.shutdown();

    static RendererGL renderer;
    static ECSModule ecs;
    eng.registerModule(&renderer);
    eng.registerModule(&ecs);

    REQUIRE(eng.startup() == true);

    // create an entity and add components so renderer finds it
    auto id = ecs.entities().create();
    TransformComponent tc; tc.position[0]=0.5f; tc.position[1]=0.0f; tc.position[2]=0.0f;
    ecs.components().add<TransformComponent>(id, tc);
    RenderableComponent rc; rc.meshPath = "assets/test.mesh"; rc.material = "default";
    ecs.components().add<RenderableComponent>(id, rc);

    // Call renderer onRender directly; in headless mode this should be a no-op and not throw.
    REQUIRE_NOTHROW(renderer.onRender());

    // Also test camera component usage (even if unused by renderer)
    CameraComponent cam;
    cam.position[0]=0.0f; cam.position[2]=5.0f;
    ecs.components().add<CameraComponent>(id, cam);
    auto gotc = ecs.components().get<CameraComponent>(id);
    REQUIRE(gotc != nullptr);
    REQUIRE(gotc->position[2] == Approx(5.0f));

    eng.shutdown();
}

