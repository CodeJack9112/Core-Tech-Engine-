
#include "InputManager.h"
#ifdef _WIN32
#include <Windows.h>
#endif

bool InputManager::isKeyPressed(int vkCode){
#ifdef _WIN32
    SHORT s = GetAsyncKeyState(vkCode);
    return (s & 0x0001) != 0; // was key pressed since last call
#else
    (void)vkCode; return false;
#endif
}

bool InputManager::isKeyDown(int vkCode){
#ifdef _WIN32
    SHORT s = GetAsyncKeyState(vkCode);
    return (s & 0x8000) != 0;
#else
    (void)vkCode; return false;
#endif
}
