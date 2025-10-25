#pragma once
#include <string>
#include <functional>
#include <unordered_map>

namespace CoreTech {

class ScriptManager {
public:
    using Func = std::function<std::string(const std::string&)>;

    ScriptManager();
    ~ScriptManager();

    // Register a named function callable from scripts.
    void registerFunction(const std::string& name, Func f);

    // Execute a one-line "script" -- for now it supports calling registered functions
    // in the form: name arg_string
    std::string execute(const std::string& line);
private:
    std::unordered_map<std::string, Func> funcs;
};

} // namespace CoreTech
