#include "ToonIntegration.h"
#include <iostream>
#ifdef __has_include
#  if __has_include("imgui.h")
#    include "imgui.h"
#    define HAVE_IMGUI 1
#  endif
#endif

namespace coretech {
static float s_levels = 3.0f;
static float s_outlineScale = 0.03f;
static float s_edgeBias = 3.0f;
static float s_baseColor[3] = {0.9f, 0.5f, 0.2f};

static auto GetRenderer() -> Renderer* { return nullptr; }

void InitializeToonMaterials() {

// --- Cinematic shader registration (MW1/MW2/MW3 style) ---
try {
    // register cinematic shaders (fragment-only variants use Cinematic.vert.glsl)
    r->LoadShader("shaders/Cinematic.vert.glsl","shaders/MW1_style.frag.glsl","cinematic_mw1");
    r->LoadShader("shaders/Cinematic.vert.glsl","shaders/MW2_style.frag.glsl","cinematic_mw2");
    r->LoadShader("shaders/Cinematic.vert.glsl","shaders/MW3_style.frag.glsl","cinematic_mw3");
    r->CreateMaterial("Mat_Cinematic_MW1","cinematic_mw1");
    r->CreateMaterial("Mat_Cinematic_MW2","cinematic_mw2");
    r->CreateMaterial("Mat_Cinematic_MW3","cinematic_mw3");
    std::cout<<"ToonIntegration: cinematic shaders registered."<<std::endl;
} catch(...) { std::cout<<"ToonIntegration: cinematic shader registration failed (best-effort)."<<std::endl; }
    std::cout<<"ToonIntegration: Renderer shader API not detected at compile time. Skipping auto registration."<<std::endl;
}

void ReloadToonShaders() {
    // No renderer API detected; no-op
}

void ToonImGuiControls() {
#if defined(HAVE_IMGUI)
    if(ImGui::CollapsingHeader("Toon Shaders")) {
        ImGui::SliderFloat("Levels", &s_levels, 1.0f, 8.0f);
        ImGui::SliderFloat("Outline Scale", &s_outlineScale, 0.0f, 0.2f);
        ImGui::SliderFloat("Edge Bias", &s_edgeBias, 0.0f, 8.0f);
        ImGui::ColorEdit3("Base Color", s_baseColor);
        if(ImGui::Button("Reload Toon Shaders")) { ReloadToonShaders(); }
        Renderer* r = GetRenderer();
        if(r) {
            // No uniform setter detected in Renderer.h; please adapt ToonIntegration.cpp to your renderer API.
        }
    }
#endif


// --- Cinematic shader controls ---
if(ImGui::CollapsingHeader("Cinematic Shaders")) {
    if(ImGui::TreeNode("MW1 (gritty)")) {
        static float mw1_contrast = 1.2f;
        static float mw1_sat = 0.85f;
        static float mw1_grain = 0.04f;
        static float mw1_tint[3] = {0.8f,0.9f,1.0f};
        ImGui::SliderFloat("MW1 Contrast", &mw1_contrast, 0.5f, 2.5f);
        ImGui::SliderFloat("MW1 Saturation", &mw1_sat, 0.0f, 2.0f);
        ImGui::SliderFloat("MW1 Grain", &mw1_grain, 0.0f, 0.2f);
        ImGui::ColorEdit3("MW1 Tint", mw1_tint);
        if(r) {
            // push to shader uniforms; best-effort uniform names
            r->SetGlobalUniformFloat("uMW1_Contrast", mw1_contrast);
            r->SetGlobalUniformFloat("uMW1_Saturation", mw1_sat);
            r->SetGlobalUniformFloat("uMW1_Grain", mw1_grain);
            r->SetGlobalUniformVec3("uMW1_Tint", mw1_tint[0], mw1_tint[1], mw1_tint[2]);
        }
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("MW2 (cold)")) {
        static float mw2_desat = 0.6f;
        static float mw2_contrast = 1.1f;
        static float mw2_chrom = 0.02f;
        static float mw2_grade[3] = {0.95f,1.0f,1.05f};
        ImGui::SliderFloat("MW2 Desat", &mw2_desat, 0.0f, 1.0f);
        ImGui::SliderFloat("MW2 Contrast", &mw2_contrast, 0.5f, 2.0f);
        ImGui::SliderFloat("MW2 ChromAb", &mw2_chrom, 0.0f, 0.1f);
        ImGui::ColorEdit3("MW2 Grade", mw2_grade);
        if(r) {
            r->SetGlobalUniformFloat("uMW2_Desat", mw2_desat);
            r->SetGlobalUniformFloat("uMW2_Contrast", mw2_contrast);
            r->SetGlobalUniformFloat("uMW2_Chrom", mw2_chrom);
            r->SetGlobalUniformVec3("uMW2_Grade", mw2_grade[0], mw2_grade[1], mw2_grade[2]);
        }
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("MW3 (warm)")) {
        static float mw3_contrast = 1.3f;
        static float mw3_vignette = 0.6f;
        static float mw3_warm[3] = {1.05f,1.02f,0.95f};
        ImGui::SliderFloat("MW3 Contrast", &mw3_contrast, 0.5f, 2.5f);
        ImGui::SliderFloat("MW3 Vignette", &mw3_vignette, 0.0f, 1.0f);
        ImGui::ColorEdit3("MW3 Warmth", mw3_warm);
        if(r) {
            r->SetGlobalUniformFloat("uMW3_Contrast", mw3_contrast);
            r->SetGlobalUniformFloat("uMW3_Vignette", mw3_vignette);
            r->SetGlobalUniformVec3("uMW3_Warm", mw3_warm[0], mw3_warm[1], mw3_warm[2]);
        }
        ImGui::TreePop();
    }
}

}

} // namespace coretech