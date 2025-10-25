#pragma once
#include <unordered_map>
#include <string>

namespace CoreTech {

class InputManager {
public:
    // Key state query (stub). Integration with actual windowing system required.
    static void setKeyState(const std::string& key, bool pressed){
        get().keys[key] = pressed;
    }
    static bool isKeyPressed(const std::string& key){
        auto &m = get().keys;
        auto it = m.find(key);
        if(it==m.end()) return false;
        return it->second;
    }
private:
    InputManager() = default;
    static InputManager& get(){
        static InputManager inst;
        return inst;
    }
    std::unordered_map<std::string,bool> keys;
};

} // namespace CoreTech
