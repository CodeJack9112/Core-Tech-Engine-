
#pragma once
#include <cstdint>
#include <string>
#include <vector>

using EntityID = uint32_t;
static const EntityID INVALID_ENTITY = 0;

struct Entity {
    EntityID id = INVALID_ENTITY;
    std::string name;
};
