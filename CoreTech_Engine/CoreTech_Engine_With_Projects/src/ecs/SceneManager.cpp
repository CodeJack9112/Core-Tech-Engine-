
#include "SceneManager.h"
#include <fstream>
#include <iostream>

SceneManager::SceneManager(EntityManager& em, ComponentManager& cm): m_entityMgr(em), m_compMgr(cm){}

bool SceneManager::saveScene(const std::string& path){
    std::ofstream f(path);
    if(!f){ std::cerr << "[Scene] Failed to open " << path << " for writing\\n"; return false; }
    // Very simple JSON-like format: list of entities with transform components
    f << "{\\n  \"entities\":[\\n";
    bool firstE = true;
    // We assume TransformComponent exists - iterate through storage via forEach
    m_compMgr.forEach<TransformComponent>([&](uint32_t ent, TransformComponent& t){
        if(!firstE) f << ",\\n";
        firstE = false;
        f << "    { \"id\": " << ent << ", \"name\": \"\", \"transform\": { \"x\": " << t.position.x << ", \"y\": " << t.position.y << ", \"z\": " << t.position.z << " } }";
    });
    f << "\\n  ]\\n}\\n";
    std::cout << "[Scene] Saved scene to " << path << "\\n";
    return true;
}

bool SceneManager::loadScene(const std::string& path){
    std::ifstream f(path);
    if(!f){ std::cerr << "[Scene] Failed to open " << path << " for reading\\n"; return false; }
    // primitive parser: look for lines with "id" and "x","y","z"
    std::string line;
    while(std::getline(f, line)){
        auto idpos = line.find("\"id\":");
        if(idpos != std::string::npos){
            // parse id
            size_t p = idpos + 5;
            while(p < line.size() && (line[p]==' '||line[p]==':')) p++;
            uint32_t id = (uint32_t)std::stoul(line.substr(p));
            // find next lines for x,y,z
            float x=0,y=0,z=0;
            std::getline(f, line); // transform line maybe
            auto xpos = line.find("\"x\":");
            if(xpos!=std::string::npos){
                size_t q = xpos+4; while(q<line.size()&&(line[q]==' '||line[q]==':')) q++;
                x = std::stof(line.substr(q));
                // try to find y and z on same line
                auto ypos = line.find("\"y\":");
                if(ypos!=std::string::npos){
                    size_t r = ypos+4; while(r<line.size()&&(line[r]==' '||line[r]==':')) r++;
                    y = std::stof(line.substr(r));
                }
                auto zpos = line.find("\"z\":");
                if(zpos!=std::string::npos){
                    size_t s=zpos+4; while(s<line.size()&&(line[s]==' '||line[s]==':')) s++;
                    z = std::stof(line.substr(s));
                }
            }
            // create entity and add transform
            EntityID eid = m_entityMgr.createEntity();
            m_compMgr.addComponent<TransformComponent>(eid);
            auto t = m_compMgr.getComponent<TransformComponent>(eid);
            if(t){ t->position.x = x; t->position.y = y; t->position.z = z; }
            std::cout << "[Scene] Loaded entity " << eid << " pos=(" << x << \",\" << y << \",\" << z << \")\\n";
        }
    }
    return true;
}
