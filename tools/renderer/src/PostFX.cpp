#include "PostFX.h"
#include <iostream>
namespace CoreTech {
std::vector<std::string> PostFX::effects;
void PostFX::init(){ effects.clear(); effects.push_back("tonemap"); std::cout<<"PostFX init (stub)\n"; }
void PostFX::apply(){ /* bind chain and apply shaders - stub */ }
}