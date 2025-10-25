#include "LODEditor.h"
#include "QEMSimplifier.h"
#include <iostream>
#ifdef __has_include
#  if __has_include("imgui.h")
#    include "imgui.h"
#    define HAVE_IMGUI 1
#  endif
#endif
#include <string>
#include <vector>
#include <filesystem>

namespace coretech {
static float s_targetRatio = 0.5f; // 50% vertices
static unsigned s_levels = 3;
void ShowLODEditorPanel(){
#ifdef HAVE_IMGUI
  if(ImGui::CollapsingHeader("LOD Generator")){
    ImGui::SliderFloat("Target Ratio", &s_targetRatio, 0.01f, 1.0f);
    ImGui::InputScalar("Levels", ImGuiDataType_U32, &s_levels);
    static char pathbuf[512] = "assets/CesiumMan.glb";
    ImGui::InputText("Model Path", pathbuf, sizeof(pathbuf));
    if(ImGui::Button("Generate LODs")){
      std::string path(pathbuf);
      // Best-effort: if file is OBJ, load and simplify; if not, notify user
      if(path.size() > 4 && path.substr(path.size()-4) == ".obj"){
        coretech::Mesh m; if(coretech::LoadOBJ(path, m)){
          unsigned int baseV = (unsigned int)m.positions.size();
          for(unsigned lvl=1; lvl<=s_levels; ++lvl){ unsigned int target = std::max(4u, (unsigned int)(baseV * pow(s_targetRatio, (double)lvl)));
            auto simp = coretech::SimplifyQEM(m, target);
            std::string out = path + "_lod" + std::to_string(lvl) + ".obj";
            coretech::SaveOBJ(out, simp);
          }
        }
      } else {
        std::cout<<"LODEditor: only .obj files are supported by the in-editor generator in this build.\n";
      }
    }
  }
#else
  std::cout<<"LODEditor: ImGui not available.\n";
#endif
}
}
