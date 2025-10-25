#include "LODRuntime.h"
#include <unordered_map>
#include <iostream>
#ifdef __has_include
#  if __has_include("Renderer.h")
#    include "Renderer.h"
#    define HAVE_RENDERER 1
#  endif
#endif
using namespace std;
namespace coretech {
  static unordered_map<string, vector<LODLevel>> g_lods;
  // mapping id->current LOD index
  static unordered_map<string, int> g_current;
  void LOD_Register(const string& id, const vector<LODLevel>& levels){ g_lods[id]=levels; g_current[id] = -1; }
  void LOD_Unregister(const string& id){ g_lods.erase(id); g_current.erase(id); }
  void LOD_Update(const glm::vec3& cameraPos){
    for(auto &kv : g_lods){ const string &id = kv.first; const auto &levels = kv.second; float bestDist = 1e30f; int bestIdx = -1;
      for(size_t i=0;i<levels.size();++i){ // simplistic: compute distance to origin of mesh by loading bounding box? We assume mesh at origin for demo
        float d = glm::length(cameraPos); // fallback
        if(d < levels[i].distanceThreshold){ bestIdx = (int)i; break; }
      }
      if(bestIdx == -1) bestIdx = (int)levels.size()-1;
      if(g_current[id] != bestIdx){ g_current[id] = bestIdx; // apply via renderer if available
#if defined(HAVE_RENDERER)
        try {
          Renderer* r = Renderer::Get(); if(r){ // call a hypothetical API SetMeshForModel
            r->SetMeshForModel(id, levels[bestIdx].meshPath);
            cout<<"LODRuntime: set "<<id<<" to LOD "<<bestIdx<<" ("<<levels[bestIdx].meshPath<<")\n";
          }
        } catch(...) { cout<<"LODRuntime: renderer call failed\n"; }
#else
        cout<<"LODRuntime: LOD for "<<id<<" changed to "<<bestIdx<<" ("<<levels[bestIdx].meshPath<<")\n";
#endif
      }
    }
  }
}
