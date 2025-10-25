
#include "AssimpLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

// stb_image functions (declared here; implementation provided by stb_image.cpp)
extern "C" unsigned char* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
extern "C" void stbi_image_free(void* retval_from_stbi_load);

AssimpLoader::~AssimpLoader(){ clear(); }

static unsigned int createGLTextureFromFile(const std::string& file){
    int w,h,channels;
    unsigned char* data = stbi_load(file.c_str(), &w, &h, &channels, 4);
    if(!data){
        std::cerr << "[AssimpLoader] Failed to load texture: " << file << std::endl;
        return 0;
    }
    unsigned int tex=0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return tex;
}


Model* AssimpLoader::loadModel(const std::string& path){
    if(m_cache.find(path) != m_cache.end()) return &m_cache[path];
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    if(!scene || !scene->HasMeshes()){
        std::cerr << "[AssimpLoader] Failed to load model: " << path << std::endl;
        return nullptr;
    }

    Model model;
    // store directory for relative texture resolution
    size_t p = path.find_last_of("/\\");
    if(p != std::string::npos) model.directory = path.substr(0, p+1);
    else model.directory = "";

    // load materials first
    if(scene->HasMaterials()){
        model.materials.resize(scene->mNumMaterials);
        for(unsigned int mi=0; mi<scene->mNumMaterials; ++mi){
            aiMaterial* mat = scene->mMaterials[mi];
            Material matgl;
            // Try PBR base color (base color or diffuse)
            aiString texpath;
            if(mat->GetTexture(aiTextureType_BASE_COLOR, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.albedo = createGLTextureFromFile(full);
            } else if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.albedo = createGLTextureFromFile(full);
            }
            // Normal map
            if(mat->GetTexture(aiTextureType_NORMALS, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.normal = createGLTextureFromFile(full);
            } else if(mat->GetTexture(aiTextureType_HEIGHT, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.normal = createGLTextureFromFile(full);
            }
            // ORM/packed maps: try METALNESS & ROUGHNESS or UNKNOWN slots
            bool gotORM = false;
            if(mat->GetTexture(aiTextureType_METALNESS, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.orm = createGLTextureFromFile(full);
                gotORM = true;
            }
            if(!gotORM && mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.orm = createGLTextureFromFile(full);
                gotORM = true;
            }
            // fallback: try specular or unknown
            if(!gotORM && mat->GetTexture(aiTextureType_SPECULAR, 0, &texpath) == AI_SUCCESS){
                std::string full = model.directory + texpath.C_Str();
                matgl.orm = createGLTextureFromFile(full);
                gotORM = true;
            }
            model.materials[mi] = matgl;
        }
    }

    // Iterate meshes
    for(unsigned int mi=0; mi<scene->mNumMeshes; ++mi){
        aiMesh* mesh = scene->mMeshes[mi];
        std::vector<float> verts;
        std::vector<unsigned int> idx;
        for(unsigned int i=0;i<mesh->mNumVertices;++i){
            aiVector3D p = mesh->mVertices[i];
            aiVector3D n = mesh->mNormals ? mesh->mNormals[i] : aiVector3D(0,1,0);
            aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0,0,0);
            verts.push_back(p.x); verts.push_back(p.y); verts.push_back(p.z);
            verts.push_back(n.x); verts.push_back(n.y); verts.push_back(n.z);
            verts.push_back(t.x); verts.push_back(t.y);
        }
        for(unsigned int f=0; f<mesh->mNumFaces; ++f){
            aiFace face = mesh->mFaces[f];
            for(unsigned int j=0;j<face.mNumIndices;++j) idx.push_back(face.mIndices[j]);
        }
        unsigned int vao=0,vbo=0,ebo=0;
        glGenVertexArrays(1,&vao);
        glGenBuffers(1,&vbo);
        glGenBuffers(1,&ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
        // pos (3), normal (3), tex (2) => stride 8 floats
        glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(0));
        glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
        glBindVertexArray(0);

        GLMesh gm;
        gm.vao = vao; gm.vbo = vbo; gm.ebo = ebo; gm.indexCount = (unsigned int)idx.size();
        gm.materialIndex = mesh->mMaterialIndex;
        model.meshes.push_back(gm);
    }

    m_cache[path] = model;
    return &m_cache[path];
}


    Model model;
    // store directory for relative texture resolution
    size_t p = path.find_last_of("/\\");
    if(p != std::string::npos) model.directory = path.substr(0, p+1);
    else model.directory = "";

    // Iterate meshes
    for(unsigned int mi=0; mi<scene->mNumMeshes; ++mi){
        aiMesh* mesh = scene->mMeshes[mi];
        std::vector<float> verts;
        std::vector<unsigned int> idx;
        for(unsigned int i=0;i<mesh->mNumVertices;++i){
            aiVector3D p = mesh->mVertices[i];
            aiVector3D n = mesh->mNormals ? mesh->mNormals[i] : aiVector3D(0,1,0);
            aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0,0,0);
            verts.push_back(p.x); verts.push_back(p.y); verts.push_back(p.z);
            verts.push_back(n.x); verts.push_back(n.y); verts.push_back(n.z);
            verts.push_back(t.x); verts.push_back(t.y);
        }
        for(unsigned int f=0; f<mesh->mNumFaces; ++f){
            aiFace face = mesh->mFaces[f];
            for(unsigned int j=0;j<face.mNumIndices;++j) idx.push_back(face.mIndices[j]);
        }
        unsigned int vao=0,vbo=0,ebo=0;
        glGenVertexArrays(1,&vao);
        glGenBuffers(1,&vbo);
        glGenBuffers(1,&ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
        // pos (3), normal (3), tex (2) => stride 8 floats
        glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(0));
        glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
        glBindVertexArray(0);

        GLMesh gm;
        gm.vao = vao; gm.vbo = vbo; gm.ebo = ebo; gm.indexCount = (unsigned int)idx.size();

        // material / texture
        if(scene->HasMaterials()){
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
            aiString texpath;
            if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &texpath) == AI_SUCCESS){
                std::string texfile = texpath.C_Str();
                std::string full = model.directory + texfile;
                gm.texture = createGLTextureFromFile(full);
            }
        }

        model.meshes.push_back(gm);
    }

    m_cache[path] = model;
    return &m_cache[path];
}

void AssimpLoader::clear(){
    for(auto &kv : m_cache){
        for(auto &gm : kv.second.meshes){
            if(gm.vao) glDeleteVertexArrays(1, &gm.vao);
            if(gm.vbo) glDeleteBuffers(1, &gm.vbo);
            if(gm.ebo) glDeleteBuffers(1, &gm.ebo);
            if(gm.texture) glDeleteTextures(1, &gm.texture);
        }
    }
    m_cache.clear();
}
