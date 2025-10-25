#pragma once
#include "Engine/Engine.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"

namespace coretech {

using EntityId = uint32_t;

class ComponentManager {
public:
    template<typename T>
    void add(EntityId id, T comp) {
        auto& map = getMap<T>();
        map[id] = std::make_shared<T>(comp);
    }
    template<typename T>
    std::shared_ptr<T> get(EntityId id) {
        auto& map = getMap<T>();
        auto it = map.find(id);
        if(it==map.end()) return nullptr;
        return it->second;
    }
    template<typename T>
    std::unordered_map<EntityId,std::shared_ptr<T>>& getMapRef() {
        return getMap<T>();
    }
private:
    template<typename T>
    static std::unordered_map<EntityId,std::shared_ptr<T>>& getMap() {
        static std::unordered_map<EntityId,std::shared_ptr<T>> map;
        return map;
    }
};

class EntityManager {
public:
    EntityId create() { EntityId id = ++last_; entities_.push_back(id); return id; }
    const std::vector<EntityId>& list() const { return entities_; }
private:
    EntityId last_ = 0;
    std::vector<EntityId> entities_;
};

class ECSModule : public EngineModule {
public:
    ECSModule() = default;
    bool init() override {
        std::cout << "[ECSModule] init\n";
        return true;
    }
    void shutdown() override {
        std::cout << "[ECSModule] shutdown\n";
    }
    const char* name() const override { return "ECS"; }
    void onUpdate(float dt) override;
    EntityManager& entities() { return em_; }
    ComponentManager& components() { return cm_; }
private:
    EntityManager em_;
    ComponentManager cm_;
};

} // namespace coretech
