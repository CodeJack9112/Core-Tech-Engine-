#include "HZBOcclusion.h"
#include <iostream>
void HZBOcclusion::BuildFromDepthTexture(unsigned int depthTextureId){ std::cout<<"HZB: BuildFromDepthTexture stub called (depthTextureId="<<depthTextureId<<")\n"; }
bool HZBOcclusion::IsOccluded(float minx,float miny,float minz,float maxx,float maxy,float maxz){ return false; }
