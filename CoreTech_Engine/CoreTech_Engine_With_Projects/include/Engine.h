
#pragma once
#include "InputManager.h"
#include "ResourceManager.h"
#include "ecs/EntityManager.h"
#include "ecs/ComponentManager.h"
#include "ecs/System.h"

class Engine {
public:
    Engine();
    ~Engine();
    bool initialize(int width, int height, const char* title);
    void shutdown();
    void update(float dt);
    void run(); // main loop, blocks until exit
    // accessors
    EntityManager& entities() { return m_entityMgr; }
    ComponentManager& components() { return m_compMgr; }
    ResourceManager& resources() { return m_resMgr; }
private:
    InputManager m_input;
    ResourceManager m_resMgr;
    EntityManager m_entityMgr;
    ComponentManager m_compMgr;
    bool m_running = false;
    int m_width=800, m_height=600;
    const char* m_title = "CoreTech Engine";
};
