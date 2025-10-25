
#include "MeshLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <tuple>

static std::string trim(const std::string& s){
    size_t a = s.find_first_not_of(" \t\r\n"); if(a==std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n"); return s.substr(a, b-a+1);
}

bool MeshLoader::loadOBJ(const std::string& path, MeshData& out){
    std::ifstream f(path);
    if(!f){ std::cerr << "[MeshLoader] Failed to open OBJ: " << path << "\\n"; return false; }
    std::vector<std::array<float,3>> positions;
    std::vector<std::array<float,3>> normals;
    std::vector<std::array<float,2>> uvs;
    std::string line;
    // map of unique vertex (p/t/n) to index
    struct Key { int p,t,n; bool operator==(Key const& o) const { return p==o.p && t==o.t && n==o.n; } };
    struct KeyHash { size_t operator()(Key const& k) const noexcept { return (k.p*73856093) ^ (k.t*19349663) ^ (k.n*83492791); } };
    std::unordered_map<Key, uint32_t, KeyHash> vertMap;
    std::vector<float> verts;
    uint32_t vertCount = 0;
    while(std::getline(f,line)){
        line = trim(line);
        if(line.empty() || line[0]=='#') continue;
        std::istringstream iss(line);
        std::string tag; iss >> tag;
        if(tag=="v"){
            float x,y,z; iss>>x>>y>>z; positions.push_back({x,y,z});
        } else if(tag=="vn"){
            float x,y,z; iss>>x>>y>>z; normals.push_back({x,y,z});
        } else if(tag=="vt"){
            float u,v; iss>>u>>v; uvs.push_back({u,v});
        } else if(tag=="f"){
            // face entries like p/t/n or p//n or p/t
            std::string a,b,c; iss>>a>>b>>c;
            std::string arr[3] = {a,b,c};
            for(int i=0;i<3;i++){
                std::string s = arr[i];
                int p=-1,t=-1,n=-1;
                size_t p1 = s.find('/');
                if(p1==std::string::npos){ p = std::stoi(s)-1; }
                else {
                    p = std::stoi(s.substr(0,p1))-1;
                    size_t p2 = s.find('/', p1+1);
                    if(p2==std::string::npos){
                        t = std::stoi(s.substr(p1+1))-1;
                    } else {
                        if(p1+1 < p2) t = std::stoi(s.substr(p1+1, p2-(p1+1)))-1;
                        n = std::stoi(s.substr(p2+1))-1;
                    }
                }
                Key key{p,t,n};
                auto it = vertMap.find(key);
                uint32_t idx;
                if(it==vertMap.end()){
                    // create vertex
                    float px=0,py=0,pz=0, nx=0,ny=0,nz=0, u=0,vv=0;
                    if(p>=0 && p < (int)positions.size()){ px=positions[p][0]; py=positions[p][1]; pz=positions[p][2]; }
                    if(n>=0 && n < (int)normals.size()){ nx=normals[n][0]; ny=normals[n][1]; nz=normals[n][2]; }
                    if(t>=0 && t < (int)uvs.size()){ u=uvs[t][0]; vv=uvs[t][1]; }
                    verts.push_back(px); verts.push_back(py); verts.push_back(pz);
                    verts.push_back(nx); verts.push_back(ny); verts.push_back(nz);
                    verts.push_back(u); verts.push_back(vv);
                    idx = vertCount++;
                    vertMap[key] = idx;
                } else {
                    idx = it->second;
                }
                // We won't use an index buffer for simplicity; duplicate vertex as needed by pushing its data again
                // But above we already stored unique vertices; now append same data to output array to keep simple draw call
                // For simplicity here we will push the vertex data again:
                size_t base = idx * 8;
                if(base + 7 < verts.size()){
                    out.vertices.push_back(verts[base+0]);
                    out.vertices.push_back(verts[base+1]);
                    out.vertices.push_back(verts[base+2]);
                    out.vertices.push_back(verts[base+3]);
                    out.vertices.push_back(verts[base+4]);
                    out.vertices.push_back(verts[base+5]);
                    out.vertices.push_back(verts[base+6]);
                    out.vertices.push_back(verts[base+7]);
                }
            }
        }
    }
    out.vertexCount = (uint32_t)(out.vertices.size() / 8);
    std::cout << "[MeshLoader] Loaded OBJ " << path << " verts=" << out.vertexCount << std::endl;
    return true;
}
