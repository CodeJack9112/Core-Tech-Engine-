#include <iostream>
#include "../include/LODRuntime.h"
int main(){
  using namespace coretech;
  std::string id = "demo_model";
  std::vector<LODLevel> levels = { {"../assets/cube.obj", 5.0f}, {"../assets/cube_lod.obj", 20.0f}, {"../assets/cube_lod2.obj", 100.0f} };
  LOD_Register(id, levels);
  // simulate camera movement
  for(float z=0; z<30; z+=2.5f){ LOD_Update(glm::vec3(0.0f,0.0f,z)); }
  return 0;
}
