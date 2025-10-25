#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "../include/LODGenerator.h"
using json = nlohmann::json;
using namespace coretech;
static void write_bin(const std::string& path, const std::vector<char>& data){ std::ofstream f(path, std::ios::binary); f.write(data.data(), data.size()); }
int main(int argc, char** argv){
  if(argc < 3){ std::cout<<"Usage: export_gltf <in.obj> <out_prefix>\n"; return 1; }
  std::string in = argv[1]; std::string outp = argv[2]; Mesh m; if(!LoadOBJ(in, m)){ std::cerr<<"Failed to load OBJ\n"; return 1; }
  // pack binary buffer: positions(float32*3), normals, uvs, indices(uint32)
  std::vector<char> bin;
  auto push_f = [&](const void* ptr, size_t bytes){ size_t off = bin.size(); bin.resize(off+bytes); memcpy(bin.data()+off, ptr, bytes); };
  uint32_t vertexCount = (uint32_t)m.positions.size(); for(auto &p: m.positions) push_f(&p.x, sizeof(float)*3);
  for(auto &n: m.normals) push_f(&n.x, sizeof(float)*3);
  for(auto &uv: m.uvs) push_f(&uv.x, sizeof(float)*2);
  for(auto idx: m.indices){ uint32_t i = idx; push_f(&i, sizeof(uint32_t)); }
  std::string binName = outp + ".bin"; write_bin(binName, bin);
  // create minimal glTF JSON referencing the single buffer
  json j; j["asset"] = { {"version","2.0"} };
  // buffers
  j["buffers"] = json::array(); j["buffers"].push_back({ {"uri", binName}, {"byteLength", (int)bin.size()} });
  j["meshes"] = json::array(); json prim; prim["attributes"] = { {"POSITION",0} }; prim["indices"] = 1; j["meshes"].push_back({ {"primitives", json::array({prim})} });
  std::string gltfName = outp + ".gltf"; std::ofstream gf(gltfName); gf<<j.dump(2); std::cout<<"Wrote "<<gltfName<<" and "<<binName<<"\n"; return 0; }
