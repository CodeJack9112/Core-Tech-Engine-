#pragma once
#include <memory>
#include <vector>
#include <string>
#include "PhysicsComponent.h"

class PickupManager {
public:
    PickupManager() : collectedCount(0) {}
    ~PickupManager(){}
    void onPickupCollected(std::shared_ptr<PhysicsComponent> pickup);
    int collected() const { return collectedCount; }
    std::vector<std::string> log() const { return events; }
private:
    int collectedCount;
    std::vector<std::string> events;
};
