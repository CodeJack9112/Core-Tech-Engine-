#include <catch2/catch.hpp>
#include "Engine/Engine.h"
#include "EngineModules/ECSModule.h"
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"

TEST_CASE("ECS add/get components", "[ecs]") {
    using namespace coretech;
    ECSModule ecs;
    REQUIRE(ecs.init() == true);
    auto id = ecs.entities().create();
    TransformComponent tc; tc.position[0]=1.0f; tc.position[1]=2.0f; tc.position[2]=3.0f;
    ecs.components().add<TransformComponent>(id, tc);
    auto got = ecs.components().get<TransformComponent>(id);
    REQUIRE(got != nullptr);
    REQUIRE(got->position[0] == Approx(1.0f));
    ecs.shutdown();
}

TEST_CASE("Renderer draw invocation (no GL)", "[renderer]") {
    using namespace coretech;
    // Use RendererGL but in headless mode it should be a harmless stub
    RendererGL r;
    REQUIRE(r.init() == true);
    // call onRender which should not throw even if GL not available
    r.onRender();
    r.shutdown();
}
