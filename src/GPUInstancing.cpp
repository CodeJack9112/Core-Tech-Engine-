#include "GPUInstancing.h"
#include <iostream>
#ifdef __has_include
#  if __has_include(<GL/glew.h>)
#    include <GL/glew.h>
#    define HAVE_GL 1
#  endif
#endif
static unsigned instanceVBO = 0;
void GPUInstancer::Clear(){ instances.clear(); }
void GPUInstancer::Add(const InstanceData& d){ instances.push_back(d); }
void GPUInstancer::Upload(){
#ifdef HAVE_GL
  if(!instanceVBO) glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, instances.size()*sizeof(InstanceData), instances.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
  std::cout<<"GPUInstancer: GL not available; Upload is a stub."<<std::endl;
#endif
}
void GPUInstancer::BindAndDraw(unsigned meshVao, unsigned indexCount){
#ifdef HAVE_GL
  glBindVertexArray(meshVao);
  // assuming instance attributes are at locations 4..7 for mat4 and 8 for color (engine must match)
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  for(int i=0;i<4;i++){
    glEnableVertexAttribArray(4+i);
    glVertexAttribPointer(4+i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(float)*i*4));
    glVertexAttribDivisor(4+i, 1);
  }
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(8,4,GL_FLOAT,GL_FALSE,sizeof(InstanceData),(void*)(sizeof(float)*16));
  glVertexAttribDivisor(8,1);
  glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instances.size());
  glBindVertexArray(0);
#else
  std::cout<<"GPUInstancer: BindAndDraw stub (no GL). Instances: "<<instances.size()<<std::endl;
#endif
}
