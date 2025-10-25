#include "../include/PickupManager.h"
#include <iostream>

void PickupManager::onPickupCollected(std::shared_ptr<PhysicsComponent> pickup){
    collectedCount++;
    events.push_back("Pickup #" + std::to_string(collectedCount));
    std::cout << "PickupManager: collected pickup #" << collectedCount << std::endl;
    // Additional game logic (add to inventory, spawn effects) could be placed here.
}
