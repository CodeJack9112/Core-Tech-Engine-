#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>

namespace CoreTech {

using EntityId = unsigned int;

class Component {
public:
    virtual ~Component() = default;
    virtual std::string getType() const = 0;
};

class TransformComponent : public Component {
public:
    float x=0, y=0, z=0;
    float rx=0, ry=0, rz=0;
    float sx=1, sy=1, sz=1;
    std::string getType() const override { return "Transform"; }
};

class CameraComponent : public Component {
public:
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    std::string getType() const override { return "Camera"; }
};

class Entity {
public:
    Entity(EntityId id, const std::string& name): id(id), name(name) {}
    EntityId getId() const { return id; }
    const std::string& getName() const { return name; }

    template<typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args){
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        components.push_back(comp);
        return comp;
    }

    template<typename T>
    std::shared_ptr<T> getComponent(){
        for(auto &c: components){
            auto ptr = std::dynamic_pointer_cast<T>(c);
            if(ptr) return ptr;
        }
        return nullptr;
    }

    void debugPrint() const {
        std::cout << "Entity " << id << " '" << name << "' components: ";
        for(auto &c: components) std::cout << c->getType() << " ";
        std::cout << std::endl;
    }

private:
    EntityId id;
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
};

class Scene {
public:
    Scene() : nextId(1) {}
    std::shared_ptr<Entity> createEntity(const std::string& name){
        auto e = std::make_shared<Entity>(nextId++, name);
        entities[e->getId()] = e;
        order.push_back(e->getId());
        return e;
    }
    std::shared_ptr<Entity> getEntity(EntityId id){
        auto it = entities.find(id);
        if(it==entities.end()) return nullptr;
        return it->second;
    }
    std::vector<std::shared_ptr<Entity>> getAllEntities(){
        std::vector<std::shared_ptr<Entity>> out;
        for(auto id : order) out.push_back(entities[id]);
        return out;
    }
    void debugPrint(){
        std::cout << "Scene Entities:\n";
        for(auto &p: entities) p.second->debugPrint();
    }
private:
    EntityId nextId;
    std::unordered_map<EntityId, std::shared_ptr<Entity>> entities;
    std::vector<EntityId> order;
};

} // namespace CoreTech
