
#pragma once
#include "Component.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <type_traits>
#include <mutex>

// Contiguous storage for a single component type
template<typename T>
class ComponentStorage {
public:
    using value_type = T;
    void add(uint32_t entity, T comp) {
        if(entityToIndex.find(entity) != entityToIndex.end()) return; // already exists
        entities.push_back(entity);
        components.push_back(comp);
        entityToIndex[entity] = (uint32_t)(components.size() - 1);
    }

    bool has(uint32_t entity) const {
        return entityToIndex.find(entity) != entityToIndex.end();
    }

    T* get(uint32_t entity) {
        auto it = entityToIndex.find(entity);
        if(it == entityToIndex.end()) return nullptr;
        return &components[it->second];
    }

    void remove(uint32_t entity) {
        auto it = entityToIndex.find(entity);
        if(it == entityToIndex.end()) return;
        uint32_t idx = it->second;
        uint32_t last = (uint32_t)components.size() - 1;
        if(idx != last) {
            components[idx] = components[last];
            entities[idx] = entities[last];
            entityToIndex[entities[idx]] = idx;
        }
        components.pop_back();
        entities.pop_back();
        entityToIndex.erase(it);
    }

    template<typename Func>
    void forEach(Func f) {
        for(size_t i=0;i<components.size();++i){
            f(entities[i], components[i]);
        }
    }

    size_t size() const { return components.size(); }

private:
    std::vector<T> components;
    std::vector<uint32_t> entities;
    std::unordered_map<uint32_t, uint32_t> entityToIndex;
};

class ComponentManager {
public:
    template<typename T, typename... Args>
    T& addComponent(uint32_t entity, Args&&... args) {
        static_assert(std::is_class<T>::value, "T must be a class type");
        auto &storage = getStorage<T>();
        storage.add(entity, T(std::forward<Args>(args)...));
        return *storage.get(entity);
    }

    template<typename T>
    T* getComponent(uint32_t entity) {
        auto &storage = getStorage<T>();
        return storage.get(entity);
    }

    template<typename T>
    void removeComponent(uint32_t entity) {
        auto &storage = getStorage<T>();
        storage.remove(entity);
    }

    template<typename T, typename Func>
    void forEach(Func f) {
        auto &storage = getStorage<T>();
        storage.forEach(f);
    }

    template<typename T>
    bool hasComponent(uint32_t entity) {
        auto &storage = getStorage<T>();
        return storage.has(entity);
    }

private:
    template<typename T>
    ComponentStorage<T>& getStorage() {
        auto id = getComponentTypeID<T>();
        std::lock_guard<std::mutex> lk(m_mutex);
        if(storages.find(id) == storages.end()){
            storages[id] = std::make_shared<StorageHolder<T>>();
        }
        return static_cast<StorageHolder<T>*>(storages[id].get())->storage;
    }

    struct IStorageHolder { virtual ~IStorageHolder() = default; };
    template<typename T>
    struct StorageHolder : IStorageHolder { ComponentStorage<T> storage; };

    std::unordered_map<ComponentTypeID, std::shared_ptr<IStorageHolder>> storages;
    std::mutex m_mutex;
};
