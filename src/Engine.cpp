
#include "../include/Engine.h"
#include "../include/RendererGL.h"
#include <chrono>
#include <iostream>

Engine::Engine(){}
Engine::~Engine(){ shutdown(); }

bool Engine::initialize(int width, int height, const char* title){
    m_width = width; m_height = height; m_title = title;
    // initialize renderer (single global instance in RendererGL)
    extern RendererGL g_renderer_global;
    if(!g_renderer_global.initialize(width, height, title)){
        std::cerr << "[Engine] Failed to init renderer\\n"; return false;
    }
    m_running = true;
    return true;
}

void Engine::shutdown(){
    extern RendererGL g_renderer_global;
    g_renderer_global.shutdown();
    m_running = false;
}

void Engine::update(float dt){
    // Placeholder for per-frame updates across subsystems
    // In future: call systems update, scripting, physics, audio, etc.
    (void)dt;
}

void Engine::run(){
    using clk = std::chrono::high_resolution_clock;
    auto last = clk::now();
    extern RendererGL g_renderer_global;
    while(m_running){
        auto now = clk::now();
        std::chrono::duration<float> diff = now - last; last = now;
        float dt = diff.count();
        // poll events handled in renderer main loop; here call update subsystems
        update(dt);
        // simple sleep to avoid busy loop
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
