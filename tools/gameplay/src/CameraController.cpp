#include "Entity.h"
#include "InputManager.h"
#include <iostream>

namespace CoreTech {

// Simple camera controller that moves the first camera entity in the scene
static void updateCamera(std::shared_ptr<Scene> scene, float dt){
    auto ents = scene->getAllEntities();
    for(auto &e: ents){
        auto cam = e->getComponent<CameraComponent>();
        auto t = e->getComponent<TransformComponent>();
        if(cam && t){
            float speed = 3.0f * dt;
            if(InputManager::isKeyPressed("W")) t->z += speed;
            if(InputManager::isKeyPressed("S")) t->z -= speed;
            if(InputManager::isKeyPressed("A")) t->x -= speed;
            if(InputManager::isKeyPressed("D")) t->x += speed;
            // debug print
            // std::cout << "Camera at: " << t->x << "," << t->y << "," << t->z << std::endl;
        }
    }
}

} // namespace CoreTech
