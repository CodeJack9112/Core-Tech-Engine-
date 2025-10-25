
#pragma once
#include "Entity.h"
#include <queue>
#include <unordered_set>

class EntityManager {
public:
    EntityManager() = default;

    EntityID createEntity(const std::string& name="") {
        EntityID id = m_nextID++;
        Entity e; e.id = id; e.name = name;
        m_entities.insert(id);
        return id;
    }

    void destroyEntity(EntityID id) {
        m_entities.erase(id);
        m_freeIDs.push(id);
    }

    bool isAlive(EntityID id) const {
        return m_entities.find(id) != m_entities.end();
    }

private:
    EntityID m_nextID {1};
    std::unordered_set<EntityID> m_entities;
    std::queue<EntityID> m_freeIDs;
};
