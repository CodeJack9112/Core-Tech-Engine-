
#pragma once
#include "Entity.h"
#include "Component.h"
#include "ComponentManager.h"

class System {
public:
    virtual ~System() = default;
    virtual void update(float dt, ComponentManager& comps) = 0;
};
