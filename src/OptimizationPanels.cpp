#include "OptimizationPanels.h"
#ifdef __has_include
#  if __has_include("imgui.h")
#    include "imgui.h"
#  endif
#endif
#include <iostream>
void ShowOptimizationPanels(){
#ifdef IMGUI_API
  if(ImGui::CollapsingHeader("Optimizations")){
    if(ImGui::TreeNode("Frustum Culling")){
      ImGui::Text("Frustum culling available: Frustum::Intersects(AABB)"); ImGui::TreePop(); }
    if(ImGui::TreeNode("Octree")) { ImGui::Text("Octree spatial partitioning present"); ImGui::TreePop(); }
    if(ImGui::TreeNode("BVH")) { ImGui::Text("BVH builder present"); ImGui::TreePop(); }
    if(ImGui::TreeNode("HZB")) { ImGui::Text("HZB occlusion stub present"); ImGui::TreePop(); }
    if(ImGui::TreeNode("LOD")) { ImGui::Text("LOD manager present"); ImGui::TreePop(); }
    if(ImGui::TreeNode("GPU Instancing")) { ImGui::Text("GPU instancer present"); ImGui::TreePop(); }
    if(ImGui::TreeNode("Render Graph")) { ImGui::Text("Render graph skeleton present"); ImGui::TreePop(); }
    if(ImGui::TreeNode("Job System")) { ImGui::Text("Job system thread pool present"); ImGui::TreePop(); }
  }
#else
  // No ImGui available; just print to stdout
  std::cout<<"Optimization panels stub (ImGui not available)."<<std::endl;
#endif
}
