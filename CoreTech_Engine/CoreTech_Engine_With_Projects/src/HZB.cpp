#include "HZB.h"
#include <algorithm>
#include <cmath>
void HZB::BuildFromDepthBuffer(const std::vector<float>& depth, int w, int h){ width=w; height=h; data=depth; // build mip chain
  mips.clear(); mips.push_back(data);
  int pw=w, ph=h; const float* prev = data.data(); int level=1;
  while(pw>1 && ph>1){ int nw = std::max(1, pw/2); int nh = std::max(1, ph/2); std::vector<float> mip(nw*nh);
    for(int y=0;y<nh;y++) for(int x=0;x<nw;x++){ float d0 = prev[(2*y)*pw + (2*x)]; float d1 = prev[(2*y)*pw + (2*x+1)]; float d2 = prev[(2*y+1)*pw + (2*x)]; float d3 = prev[(2*y+1)*pw + (2*x+1)]; mip[y*nw + x] = std::min(std::min(d0,d1), std::min(d2,d3)); }
    mips.push_back(std::move(mip)); prev = mips.back().data(); pw = nw; ph = nh; level++; }
}
bool HZB::IsOccludedNDCRange(float xmin,float ymin,float xmax,float ymax, float zmin, float zmax) const {
  if(mips.empty()) return false;
  // choose appropriate mip based on rect size
  float sx = std::max(1e-6f, xmax - xmin); float sy = std::max(1e-6f, ymax - ymin);
  int mip = 0; float maxDim = std::max(sx, sy);
  // heuristic: larger rect => use higher mip
  if(maxDim > 0.5f) mip = std::min((int)mips.size()-1, 0+2);
  // sample mip densely: map ndc to mip coords
  const auto &M = mips[mip]; int mw = std::max(1, width >> mip); int mh = std::max(1, height >> mip);
  int x0 = std::clamp(int((xmin*0.5f+0.5f) * mw), 0, mw-1); int x1 = std::clamp(int((xmax*0.5f+0.5f) * mw), 0, mw-1);
  int y0 = std::clamp(int((ymin*0.5f+0.5f) * mh), 0, mh-1); int y1 = std::clamp(int((ymax*0.5f+0.5f) * mh), 0, mh-1);
  for(int y=y0;y<=y1;y++) for(int x=x0;x<=x1;x++){ float d = M[y*mw + x]; if(d < zmax) return true; }
  return false; }
