
// ImGuizmo integration for 3D gizmos
#ifdef __has_include
  #if __has_include("ImGuizmo.h")
    #define HAVE_IMGUZMO 1
  #endif
#endif
#if HAVE_IMGUZMO
#include "ImGuizmo.h"
#endif
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "GameApp.h"
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "../gameplay/include/Entity.h"

// Simple editor state
static int selectedEntityId = -1;

void showEditorUI(std::shared_ptr<CoreTech::Scene> scene){
    ImGui::Begin("Scene Editor");

    if(ImGui::Button("Save Scene")){
        extern std::shared_ptr<CoreTech::Scene> globalScene;
        if(globalScene){
            bool ok = CoreTech::saveSceneToFile(globalScene, "tools/gameplay/sample_saved.scene.json");
            if(ok) ImGui::OpenPopup("Saved");
        }
    }
    if(ImGui::BeginPopupModal("Saved", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("Scene saved to tools/gameplay/sample_saved.scene.json");
        if(ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if(!scene){ ImGui::Text("No scene loaded"); ImGui::End(); return; }
    auto ents = scene->getAllEntities();
    ImGui::Text("Entities:");
    for(auto &e: ents){
        char buf[128]; snprintf(buf, sizeof(buf), "%u: %s", e->getId(), e->getName().c_str());
        if(ImGui::Selectable(buf, selectedEntityId == (int)e->getId())){
            selectedEntityId = (int)e->getId();
        }
    }
    if(selectedEntityId!=-1){
        auto ent = scene->getEntity((unsigned int)selectedEntityId);
        if(ent){
            auto t = ent->getComponent<CoreTech::TransformComponent>();
            if(t){
                float pos[3] = {t->x, t->y, t->z};
                if(ImGui::InputFloat3("Position", pos)){
                    t->x = pos[0]; t->y = pos[1]; t->z = pos[2];
                }
            }
        }
    }
    
    ImGui::Separator();
    ImGui::Text("Asset Browser:");
    static char assetPath[256] = "tools/assets/";
    ImGui::InputText("Assets Dir", assetPath, sizeof(assetPath));
    if(ImGui::Button("Refresh Assets")){
        // simple scan can be implemented by contributor
    }
    if(selectedEntityId!=-1){
        auto ent = scene->getEntity((unsigned int)selectedEntityId);
        if(ent){
            auto mat = ent->getComponent<CoreTech::MaterialComponent>();
            if(!mat) mat = ent->addComponent<CoreTech::MaterialComponent>();
            ImGui::Text("Material:");
            char namebuf[128]; strncpy(namebuf, mat->name.c_str(), sizeof(namebuf));
            if(ImGui::InputText("Name", namebuf, sizeof(namebuf))){ mat->name = std::string(namebuf); }
            float color[3] = { mat->albedo.x, mat->albedo.y, mat->albedo.z };
            if(ImGui::ColorEdit3("Albedo", color)){ mat->albedo = glm::vec3(color[0],color[1],color[2]); }
            char texbuf[256]; strncpy(texbuf, mat->albedoTexture.c_str(), sizeof(texbuf));
            if(ImGui::InputText("Albedo Tex", texbuf, sizeof(texbuf))){ mat->albedoTexture = std::string(texbuf); }
            if(ImGui::Button("Save Scene As JSON")){
                CoreTech::saveSceneToFile(scene, "tools/gameplay/sample_saved_full.scene.json");
                ImGui::OpenPopup("Saved");
            }
        }
    }

    
    ImGui::Separator();
    
    ImGui::Separator();
    ImGui::Text("Gizmos:");
    if(selectedEntityId!=-1){
        auto ent = scene->getEntity((unsigned int)selectedEntityId);
        if(ent){
            auto t = ent->getComponent<CoreTech::TransformComponent>();
            if(t){
#if HAVE_IMGUZMO
                // Prepare view and projection matrices from the engine camera if available, otherwise fallback
                glm::mat4 view;
                glm::mat4 proj;
                bool haveCamera = false;
                if(globalScene){
                    for(auto &ce : globalScene->getAllEntities()){
                        // try to find a CameraComponent on entities (RenderComponents.h defines CameraComponent)
                        #ifdef __has_include
                          #if __has_include("../../include/ecs/RenderComponents.h")
                            #include "../../include/ecs/RenderComponents.h"
                          #endif
                        #endif
                        auto cam = ce->getComponent<CameraComponent>();
                        auto ct = ce->getComponent<TransformComponent>();
                        if(cam && ct){
                            glm::vec3 pos = cam->position;
                            glm::vec3 forward = cam->forward;
                            glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
                            view = glm::lookAt(pos, pos + forward, up);
                            float aspect = ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y;
                            proj = glm::perspective(glm::radians(cam->fov), aspect, cam->nearPlane, cam->farPlane);
                            haveCamera = true;
                            break;
                        }
                    }
                }
                if(!haveCamera){
                    view = glm::lookAt(glm::vec3(0.0f,2.0f,5.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
                    proj = glm::perspective(glm::radians(60.0f), ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y, 0.1f, 100.0f);
                }\n
                // Model matrix from transform\n                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(t->x, t->y, t->z));\n                model = model * glm::mat4_cast(glm::quat(glm::vec3(glm::radians(t->rx), glm::radians(t->ry), glm::radians(t->rz))));\n                model = glm::scale(model, glm::vec3(t->sx, t->sy, t->sz));\n\n                ImGuizmo::SetOrthographic(false);\n                ImGuizmo::BeginFrame();\n                ImGuizmo::SetDrawlist();\n\n                // Setup rectangle for gizmo (use full window)\n                ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);\n
                static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                static ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;
                if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_T))) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R))) mCurrentGizmoOperation = ImGuizmo::ROTATE;
                if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S))) mCurrentGizmoOperation = ImGuizmo::SCALE;

                // Manipulate expects column-major matrices
                glm::mat4 modelMatrix = model;
                glm::mat4 viewMatrix = view;
                glm::mat4 projMatrix = proj;

                // Call ImGuizmo
                bool used = ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
                                  mCurrentGizmoOperation, mCurrentGizmoMode,
                                  glm::value_ptr(modelMatrix));

                if(used){
                    // Decompose modelMatrix back into translation, rotation, scale
                    glm::vec3 translation, scale, skew;
                    glm::vec4 perspective;
                    glm::quat orientation;
                    glm::decompose(modelMatrix, scale, orientation, translation, skew, perspective);
                    // orientation is a quaternion; convert to Euler degrees
                    glm::vec3 euler = glm::degrees(glm::eulerAngles(orientation));
                    t->x = translation.x; t->y = translation.y; t->z = translation.z;
                    t->rx = euler.x; t->ry = euler.y; t->rz = euler.z;
                    t->sx = scale.x; t->sy = scale.y; t->sz = scale.z;
                }
#else
                // ImGuizmo not available: fallback to numeric gizmos
                float pos[3] = {t->x, t->y, t->z};
                if(ImGui::DragFloat3("Position##gizmo", pos, 0.05f)){ t->x=pos[0]; t->y=pos[1]; t->z=pos[2]; }
                float rot[3] = {t->rx, t->ry, t->rz};
                if(ImGui::DragFloat3("Rotation##gizmo", rot, 0.5f)){ t->rx=rot[0]; t->ry=rot[1]; t->rz=rot[2]; }
                float scl[3] = {t->sx, t->sy, t->sz};
                if(ImGui::DragFloat3("Scale##gizmo", scl, 0.01f)){ t->sx=scl[0]; t->sy=scl[1]; t->sz=scl[2]; }
                if(ImGui::Button("Snap to Origin")){ t->x=t->y=t->z=0.0f; }
#endif
            }
        }
    }
ImGui::End();
}


    ImGui::Begin("Hierarchy");
    if(globalScene){
        int idx=0;
        for(auto &e : globalScene->getAllEntities()){
            char buf[128]; snprintf(buf, sizeof(buf), "%s##%u", e->getName().c_str(), e->getId());
            if(ImGui::Selectable(buf, selectedEntityId==(int)e->getId())){
                selectedEntityId = (int)e->getId();
            }
        }
    }
    ImGui::End();
    ImGui::Begin("Inspector");
    if(selectedEntityId!=-1 && globalScene){
        auto ent = globalScene->getEntity((unsigned int)selectedEntityId);
        if(ent){
            ImGui::Text("Entity: %s", ent->getName().c_str());
            // list components
            if(ent->getComponent<CoreTech::TransformComponent>()){
                auto t = ent->getComponent<CoreTech::TransformComponent>();
                float pos[3]={t->x,t->y,t->z}; if(ImGui::DragFloat3("Position", pos, 0.05f)){ t->x=pos[0]; t->y=pos[1]; t->z=pos[2]; }
            }
            // show other components in similar fashion
        }
    }
    
ImGui::Separator();
ImGui::Text("Environment / IBL:");
static char hdrPath[512] = "tools/assets/hdr/default.hdr";
ImGui::InputText("HDR Path", hdrPath, sizeof(hdrPath));
if(ImGui::Button("Generate IBL")){
    unsigned int irr=0,pref=0,brdf=0;
    CoreTech::IBLGenerator::generateIBL(std::string(hdrPath), irr, pref, brdf);
    // store into renderer (stub): Renderer::instance().setIBLTextures(irr,pref,brdf);
}
ImGui::End();
