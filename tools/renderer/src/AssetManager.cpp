#include "AssetManager.h"
#include <iostream>
#ifdef __has_include
  #if __has_include("tiny_obj_loader.h")
    #define HAVE_TINYOBJ 1
  #endif
  #if __has_include("stb_image.h")
    #define HAVE_STB 1
  #endif
#endif
#if HAVE_TINYOBJ
#include "tiny_obj_loader.h"
#endif
#if HAVE_STB
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#include <glad/glad.h>

#ifdef __has_include
  #if __has_include(<assimp/Importer.hpp>)
    #define HAVE_ASSIMP 1
  #endif
#endif
#if HAVE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif


namespace CoreTech {
bool AssetManager::loadOBJ(const std::string& path, SimpleMesh& out){
    // try Assimp skinned loader first for animated models
    if(loadSkinnedModel(path, out)) return true;

#if HAVE_TINYOBJ
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())){
        std::cerr << "tinyobj error: " << warn << " " << err << std::endl; return false;
    }
    out.vertices.clear(); out.indices.clear();
    for(const auto& shape: shapes){
        for(const auto& face : shape.mesh.indices){
            int idx = face.vertex_index;
            out.vertices.push_back(attrib.vertices[3*idx+0]);
            out.vertices.push_back(attrib.vertices[3*idx+1]);
            out.vertices.push_back(attrib.vertices[3*idx+2]);
            out.indices.push_back((unsigned int)out.indices.size());
        }
    }
    return true;
#else
    std::cerr << "tinyobj not available\n"; return false;
#endif
}

unsigned int AssetManager::loadTexture(const std::string& path){
#if HAVE_STB
    int w,h,n;
    unsigned char* data = stbi_load(path.c_str(), &w,&h,&n,4);
    if(!data){ std::cerr << "stb_image failed to load: " << path << std::endl; return 0; }
    unsigned int tex; glGenTextures(1,&tex); glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return tex;
#else
    std::cerr << "stb_image not available\n"; return 0;
#endif
}
}

bool loadSkinnedModel(const std::string& path, SimpleMesh& out){
#if HAVE_ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    if(!scene) return false;
    out.vertices.clear(); out.indices.clear(); out.normals.clear(); out.uvs.clear(); out.boneIds.clear(); out.boneWeights.clear();
    // For simplicity, only load first mesh
    if(scene->mNumMeshes==0) return false;
    aiMesh* mesh = scene->mMeshes[0];
    size_t vcount = mesh->mNumVertices;
    out.vertices.reserve(vcount*3); out.normals.reserve(vcount*3); out.uvs.reserve(vcount*2);
    // init bone data containers (4 bones per vertex)
    out.boneIds.assign(vcount*4, -1);
    out.boneWeights.assign(vcount*4, 0.0f);
    for(unsigned int i=0;i<mesh->mNumVertices;i++){
        out.vertices.push_back(mesh->mVertices[i].x);
        out.vertices.push_back(mesh->mVertices[i].y);
        out.vertices.push_back(mesh->mVertices[i].z);
        if(mesh->mNormals){
            out.normals.push_back(mesh->mNormals[i].x); out.normals.push_back(mesh->mNormals[i].y); out.normals.push_back(mesh->mNormals[i].z);
        } else { out.normals.push_back(0); out.normals.push_back(0); out.normals.push_back(0); }
        if(mesh->mTextureCoords[0]){ out.uvs.push_back(mesh->mTextureCoords[0][i].x); out.uvs.push_back(mesh->mTextureCoords[0][i].y); }
        else { out.uvs.push_back(0); out.uvs.push_back(0); }
    }
    // indices
    for(unsigned int f=0; f<mesh->mNumFaces; ++f){
        aiFace face = mesh->mFaces[f];
        for(unsigned int k=0;k<face.mNumIndices;k++) out.indices.push_back(face.mIndices[k]);
    }
    // bones
    for(unsigned int b=0;b<mesh->mNumBones;b++){
        aiBone* bone = mesh->mBones[b];
        for(unsigned int wi=0; wi<bone->mNumWeights; ++wi){
            aiVertexWeight w = bone->mWeights[wi];
            unsigned int vid = w.mVertexId;
            float weight = w.mWeight;
            // find first free slot (4 per vertex)
            for(int slot=0; slot<4; ++slot){
                int idx = vid*4 + slot;
                if(out.boneWeights[idx] == 0.0f){
                    out.boneIds[idx] = (int)b;
                    out.boneWeights[idx] = weight;
                    break;
                }
            }
        }
    }
    return true;
#else
    (void)path; (void)out; return false;
#endif
}
