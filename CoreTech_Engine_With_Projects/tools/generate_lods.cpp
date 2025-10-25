#include <iostream>
#include "../include/QEMSimplifier.h"
#include "../include/LODGenerator.h"
using namespace coretech;
int main(int argc, char** argv){
  if(argc < 5){ std::cout<<"Usage: generate_lods <in.obj> <out_prefix> <levels> <target_ratio>\n"; return 1; }
  std::string in = argv[1]; std::string outp = argv[2]; int levels = atoi(argv[3]); float ratio = atof(argv[4]);
  Mesh m; if(!LoadOBJ(in, m)){ std::cerr<<"Failed to load OBJ\n"; return 1; }
  unsigned baseV = (unsigned)m.positions.size();
  for(int i=1;i<=levels;i++){
    unsigned target = std::max(4u, (unsigned)(baseV * pow(ratio, (double)i)));
    std::cout<<"Generating LOD "<<i<<" target verts="<<target<<"\n";
    Mesh s = SimplifyQEM(m, target);
    std::string out = outp + "_lod" + std::to_string(i) + ".obj";
    if(!SaveOBJ(out, s)) std::cerr<<"Failed to save "<<out<<"\n";
  }
  return 0;
}
