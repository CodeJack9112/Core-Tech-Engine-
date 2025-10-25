#include "EngineModules/ECSModule.h"
#include <iostream>

namespace coretech {

void ECSModule::onUpdate(float dt) {
    // For now, just print entity count and update movement system if any exists
    (void)dt;
    std::cout << "[ECSModule] entities=" << entities().list().size() << "\n";
    // Example: move any entity with TransformComponent
    for(auto id : entities().list()) {
        auto t = components().get<TransformComponent>(id);
        if(t) {
            t->position[0] += 0.01f; // simple move
        }
    }
}

} // namespace coretech
