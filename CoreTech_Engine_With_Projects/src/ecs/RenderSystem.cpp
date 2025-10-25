
#include "../../include/RendererGL.h"

#include "../../include/AssimpLoader.h"
static AssimpLoader g_assimpLoader;
static std::unordered_map<uint32_t, Model*> g_entityModels; // cache entity->model

static RendererGL g_renderer;
static bool g_rendererInitialized = false;

#include "RenderSystem.h"
#include "../MeshLoader.h"
#include <glm/glm.hpp>
#include <iostream>

void RenderSystem::update(float dt, ComponentManager& comps) {
    if(!g_rendererInitialized){ if(g_renderer.initialize(800,600,"CoreTech RenderSystem")){ g_renderer.initTexturedQuad(); g_renderer.initializeDeferred(800,600); g_rendererInitialized=true; } }

    // ensure meshes registered
    comps.forEach<MeshComponent>([&](uint32_t ent, MeshComponent& m){
        if(!m.meshPath.empty()){
            MeshData md;
            if(registerMesh(m.meshPath, md) == false){
                // try load
                if(MeshLoader::loadOBJ(m.meshPath, md)){
                    registerMesh(m.meshPath, md);
                } else {
                    std::cerr << "[RenderSystem] Failed to load mesh: " << m.meshPath << std::endl;
                }
            }
        }
    });
    // render instances (use transform if exists)
    comps.forEach<MeshComponent>([&](uint32_t ent, MeshComponent& m){
        TransformComponent* t = comps.getComponent<TransformComponent>(ent);
        glm::vec3 pos(0.0f);
        if(t) pos = t->position;
        renderMeshInstance(m.meshPath, pos);
    });
}



// Run deferred pass by providing geometry drawing callback
g_renderer.renderDeferred([&](){
    // draw all mesh entities into G-buffer
    for(auto &kv : g_entityModels){
        Model* model = kv.second;
        if(!model) continue;
        for(auto &gm : model->meshes){
            // bind material textures to unit 0 (albedo)
            Material &mat = model->materials[gm.materialIndex < 0 ? 0 : gm.materialIndex];
            if(mat.albedo){ glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, mat.albedo); }
            // draw mesh
            glBindVertexArray(gm.vao);
            glDrawElements(GL_TRIANGLES, gm.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            if(mat.albedo){ glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0); }
        }
    }
});
