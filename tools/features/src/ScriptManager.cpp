#include "ScriptManager.h"
#include <sstream>

namespace CoreTech {

ScriptManager::ScriptManager(){}
ScriptManager::~ScriptManager(){}

void ScriptManager::registerFunction(const std::string& name, Func f){
    funcs[name] = f;
}

std::string ScriptManager::execute(const std::string& line){
    std::istringstream iss(line);
    std::string name;
    if(!(iss >> name)) return "";
    std::string rest;
    std::getline(iss, rest);
    // trim leading spaces
    if(!rest.empty() && rest.front()==' ') rest.erase(0,1);
    auto it = funcs.find(name);
    if(it == funcs.end()) return std::string("ERROR: function not found: ") + name;
    try{
        return it->second(rest);
    }catch(...){
        return std::string("ERROR: exception in function ") + name;
    }
}

} // namespace CoreTech
