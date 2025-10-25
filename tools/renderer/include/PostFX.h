#pragma once
#include <vector>
#include <string>
namespace CoreTech {
class PostFX {
public:
    static void init();
    static void apply(); // apply chain (stub)
    static std::vector<std::string> effects;
};
}