
#pragma once
class InputManager {
public:
    InputManager() = default;
    bool isKeyPressed(int vkCode); // virtual-key code
    bool isKeyDown(int vkCode);
};
