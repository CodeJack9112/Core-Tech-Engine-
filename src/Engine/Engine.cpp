#include "Engine/Engine.h"
#include <algorithm>
#include <thread>
#include <iostream>

namespace coretech {

Engine& Engine::instance() {
    static Engine e;
    return e;
}

void Engine::registerModule(EngineModule* mod) {
    if(mod) modules_.push_back(mod);
}

bool Engine::startup() {
    for(auto m : modules_) {
        std::cout << "[Engine] init " << m->name() << "\n";
        if(!m->init()) {
            // shutdown initialized modules in reverse
            for(auto it = std::make_reverse_iterator(modules_.end()); it != std::make_reverse_iterator(modules_.begin()); ++it) {
                (*it)->shutdown();
            }
            return false;
        }
    }
    running_.store(true);
    return true;
}

void Engine::shutdown() {
    running_.store(false);
    for(auto it = modules_.rbegin(); it != modules_.rend(); ++it) {
        std::cout << "[Engine] shutdown " << (*it)->name() << "\n";
        (*it)->shutdown();
    }
    modules_.clear();
}

void Engine::run(int maxFrames, float targetHz) {
    if(!running_) return;
    using clock = std::chrono::high_resolution_clock;
    const float targetDt = 1.0f / targetHz;
    int frames = 0;
    while(running_ && (maxFrames < 0 || frames < maxFrames)) {
        auto t0 = clock::now();
        // Update phase: call onUpdate on all modules
        for(auto m : modules_) {
            try{ m->onUpdate(targetDt); } catch(...) {}
        }
        // Render phase: call onRender on all modules
        for(auto m : modules_) {
            try{ m->onRender(); } catch(...) {}
        }
        frames++;
        auto t1 = clock::now();
        std::chrono::duration<float> dur = t1 - t0;
        float dt = dur.count();
        // Sleep to maintain targetHz if needed
        if(dt < targetDt) {
            std::this_thread::sleep_for(std::chrono::duration<float>(targetDt - dt));
        }
    }
}

void Engine::stop() {
    running_.store(false);
}

void Engine::broadcastEvent(const std::string& evt, const std::string& payload) {
    for(auto m : modules_) {
        try{ m->onEvent(evt,payload); } catch(...) {}
    }
}

} // namespace coretech
