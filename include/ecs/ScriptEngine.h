
#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <optional>
#include "Entity.h"
#include "ComponentManager.h"
#include "EntityManager.h"

// Simple ScriptEngine that runs text-based commands.
// Features added:
// - variable assignment: var = command args...
// - variable substitution: use $var in arguments to refer to stored values
// - named entities: createEntity name will store mapping from name -> id
// - functions return string results which can be assigned to variables
// - host functions can be registered to allow future language bindings

class ScriptEngine {
public:
    ScriptEngine(EntityManager& em, ComponentManager& cm);
    bool executeScriptFile(const std::string& path);

    // Register custom command: name -> function(tokens) returns string result
    void registerCommand(const std::string& name, std::function<std::string(const std::vector<std::string>&)> fn);

    // Fetch a variable by name (if set by script)
    std::optional<std::string> getVariable(const std::string& name) const;

private:
    EntityManager& m_entityMgr;
    ComponentManager& m_compMgr;
    std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> m_cmds;
    std::unordered_map<std::string, std::string> m_vars; // variables set by scripts
    std::unordered_map<std::string, EntityID> m_namedEntities; // map names to IDs

    std::vector<std::string> tokenize(const std::string& line);
    std::string substituteVariables(const std::string& token);
};
