
#pragma once
#include <cstdint>
#include <typeindex>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <functional>
#include "Entity.h"

using ComponentTypeID = std::size_t;

inline ComponentTypeID getUniqueComponentTypeID() {
    static ComponentTypeID lastID = 0u;
    return lastID++;
}

template <typename T>
inline ComponentTypeID getComponentTypeID() noexcept {
    static ComponentTypeID typeID = getUniqueComponentTypeID();
    return typeID;
}

struct IComponent {
    virtual ~IComponent() = default;
};
using ComponentPtr = std::shared_ptr<IComponent>;

// Forward declare ComponentManager for scripting hooks
class ComponentManager;
