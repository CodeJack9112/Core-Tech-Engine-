#include <iostream>
#include <string>
#include "../include/LODGenerator.h"
using namespace coretech;
int main(int argc, char** argv){
  if(argc < 4){ std::cout<<"Usage: lod_generator <in.obj> <out.obj> <gridSize>\n"; return 1; }
  std::string in = argv[1]; std::string out = argv[2]; float grid = (float)atof(argv[3]);
  Mesh m;
  if(!LoadOBJ(in, m)){ std::cerr<<"Failed to load input OBJ\n"; return 1; }
  std::cout<<"Loaded mesh: positions="<<m.positions.size()<<" tris="<<m.indices.size()/3<<"\n";
  Mesh simp = SimplifyByVertexClustering(m, grid);
  std::cout<<"Simplified mesh: positions="<<simp.positions.size()<<" tris="<<simp.indices.size()/3<<"\n";
  if(!SaveOBJ(out, simp)){ std::cerr<<"Failed to save output OBJ\n"; return 1; }
  std::cout<<"Wrote "<<out<<"\n";
  return 0;
}
