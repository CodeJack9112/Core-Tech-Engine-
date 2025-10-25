
#include "ScriptEngine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

ScriptEngine::ScriptEngine(EntityManager& em, ComponentManager& cm): m_entityMgr(em), m_compMgr(cm) {
    // register built-in commands returning string results
    registerCommand("createEntity", [this](const std::vector<std::string>& tokens)->std::string{
        std::string name = tokens.size() > 1 ? tokens[1] : "";
        EntityID id = m_entityMgr.createEntity(name);
        if(!name.empty()){
            m_namedEntities[name] = id;
        }
        std::string res = std::to_string((uint32_t)id);
        std::cout << "[Script] Created entity " << id << \" name=\" << name << std::endl;
        return res;
    });
    registerCommand("addTransform", [this](const std::vector<std::string>& tokens)->std::string{
        if(tokens.size() < 5){ std::cerr << "[Script] addTransform requires: entity x y z\\n"; return \"\"; }
        std::string entTok = tokens[1];
        // allow named or numeric entity id
        uint32_t ent = 0;
        if(entTok.size() > 0 && entTok[0] == '$') {
            auto it = m_vars.find(entTok.substr(1));
            if(it != m_vars.end()) ent = std::stoul(it->second);
        } else {
            // if token is a known name
            auto itn = m_namedEntities.find(entTok);
            if(itn != m_namedEntities.end()) ent = itn->second;
            else ent = std::stoul(entTok);
        }
        float x = std::stof(tokens[2]);
        float y = std::stof(tokens[3]);
        float z = std::stof(tokens[4]);
        if(!m_compMgr.hasComponent<TransformComponent>(ent)) {
            m_compMgr.addComponent<TransformComponent>(ent);
        }
        auto t = m_compMgr.getComponent<TransformComponent>(ent);
        if(t){ t->position.x = x; t->position.y = y; t->position.z = z; std::cout << \"[Script] added transform to \" << ent << \"\\n\"; }
        return std::to_string(ent);
    });
    registerCommand("getEntityByName", [this](const std::vector<std::string>& tokens)->std::string{
        if(tokens.size() < 2) return \"\";
        auto it = m_namedEntities.find(tokens[1]);
        if(it == m_namedEntities.end()) return \"0\";
        return std::to_string((uint32_t)it->second);
    });
    registerCommand("print", [this](const std::vector<std::string>& tokens)->std::string{
        for(size_t i=1;i<tokens.size();++i){
            std::cout << tokens[i] << (i+1<tokens.size()?\" \":\"\\n\");
        }
        return \"\";
    });
}

void ScriptEngine::registerCommand(const std::string& name, std::function<std::string(const std::vector<std::string>&)> fn){
    m_cmds[name] = fn;
}

std::vector<std::string> ScriptEngine::tokenize(const std::string& line){
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string t;
    while(iss >> t) tokens.push_back(t);
    return tokens;
}

std::string ScriptEngine::substituteVariables(const std::string& token){
    if(token.size()>0 && token[0] == '$'){
        std::string name = token.substr(1);
        auto it = m_vars.find(name);
        if(it != m_vars.end()) return it->second;
        // also support named entities directly: $Enemy1 -> id if exists
        auto itn = m_namedEntities.find(name);
        if(itn != m_namedEntities.end()) return std::to_string((uint32_t)itn->second);
        return std::string(); // empty if not found
    }
    return token;
}

bool ScriptEngine::executeScriptFile(const std::string& path){
    std::ifstream f(path);
    if(!f){ std::cerr << "[Script] Failed to open script: " << path << "\\n"; return false; }
    std::string line;
    while(std::getline(f, line)){
        // trim
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch){ return !std::isspace(ch); }));
        if(line.empty() || line[0]=='#') continue;
        // support assignment: var = command args...
        size_t eq = line.find('=');
        std::string varName;
        if(eq != std::string::npos){
            varName = line.substr(0, eq);
            // trim varName spaces
            varName.erase(varName.find_last_not_of(" \t\r\n")+1);
            varName.erase(0, varName.find_first_not_of(" \t\r\n"));
            line = line.substr(eq+1);
        }
        auto tokens = tokenize(line);
        if(tokens.empty()) continue;
        // substitute variables in tokens
        for(auto &tok : tokens){
            if(tok.size()>0 && tok[0]=='$'){
                std::string sub = substituteVariables(tok);
                if(!sub.empty()) tok = sub;
            }
        }
        auto it = m_cmds.find(tokens[0]);
        if(it != m_cmds.end()){
            std::string result = it->second(tokens);
            if(!varName.empty()){
                m_vars[varName] = result;
                std::cout << "[Script] Set $" << varName << \" = \" << result << std::endl;
            }
        } else {
            std::cerr << "[Script] Unknown command: " << tokens[0] << \"\\n\";
        }
    }
    return true;
}

std::optional<std::string> ScriptEngine::getVariable(const std::string& name) const {
    auto it = m_vars.find(name);
    if(it==m_vars.end()) return std::nullopt;
    return it->second;
}
