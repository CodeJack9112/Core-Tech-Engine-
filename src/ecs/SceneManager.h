
#pragma once
#include <string>
#include "EntityManager.h"
#include "ComponentManager.h"

class SceneManager {
public:
    SceneManager(EntityManager& em, ComponentManager& cm);
    bool saveScene(const std::string& path);
    bool loadScene(const std::string& path);
private:
    EntityManager& m_entityMgr;
    ComponentManager& m_compMgr;
};
