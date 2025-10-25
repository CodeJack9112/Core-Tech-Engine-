#include "AssimpLoader.h"
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
#include <iostream>
namespace CoreTech {
bool loadMeshFromFile(const std::string& path, SimpleMesh& out){
#if HAVE_ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);
    if(!scene || !scene->HasMeshes()) { std::cerr << "Assimp failed to load: " << path << std::endl; return false; }
    aiMesh* mesh = scene->mMeshes[0];
    out.vertices.clear(); out.indices.clear();
    for(unsigned int i=0;i<mesh->mNumVertices;i++){
        out.vertices.push_back(mesh->mVertices[i].x);
        out.vertices.push_back(mesh->mVertices[i].y);
        out.vertices.push_back(mesh->mVertices[i].z);
    }
    for(unsigned int i=0;i<mesh->mNumFaces;i++){
        aiFace &f = mesh->mFaces[i];
        for(unsigned int j=0;j<f.mNumIndices;j++) out.indices.push_back(f.mIndices[j]);
    }
    return true;
#else
    std::cerr << "Assimp not available at compile time\n";
    return false;
#endif
}
}