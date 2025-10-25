#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "Engine/Engine.h"
#include "EngineModules/RendererGL.h"
#include "EngineModules/ECSModule.h"

TEST_CASE("Engine lifecycle", "[engine]") {
    using namespace coretech;
    Engine& eng = Engine::instance();
    static RendererGL r;
    static ECSModule e;
    eng.registerModule(&r);
    eng.registerModule(&e);
    REQUIRE(eng.startup() == true);
    eng.stop();
    eng.shutdown();
}