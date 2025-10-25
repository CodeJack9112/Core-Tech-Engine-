\
    // Simple Assimp-based skinned mesh importer demo.
    // This demo loads a model (glTF/FBX) via Assimp, prints skeleton hierarchy and animation info,
    // and performs a simple CPU skinning pass for the first animation pose (for demonstration).
    //
    // Requires: Assimp (find_package(assimp REQUIRED))
    //
    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>
    #include <iostream>
    #include <vector>
    #include <string>
    #include <unordered_map>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>

    static void printNode(const aiNode* node, int depth=0) {
        for(int i=0;i<depth;i++) std::cout << \"  \";
        std::cout << node->mName.C_Str() << \" (children=\"<< node->mNumChildren <<\")\\n\";
        for(unsigned int i=0;i<node->mNumChildren;i++) printNode(node->mChildren[i], depth+1);
    }

    int main(int argc, char** argv) {
        std::string path = \"../assets/CesiumMan.glb\";
        if(argc > 1) path = argv[1];
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices);
        if(!scene) {
            std::cerr << \"Failed to load: \" << importer.GetErrorString() << std::endl;
            return 1;
        }
        std::cout << \"Loaded scene with \" << scene->mNumMeshes << \" meshes, \" << scene->mNumAnimations << \" animations\\n\";
        if(scene->mRootNode) {
            std::cout << \"Node hierarchy:\\n\";
            printNode(scene->mRootNode);
        }
        // List bones per mesh
        for(unsigned int m=0;m<scene->mNumMeshes;m++) {
            const aiMesh* mesh = scene->mMeshes[m];
            std::cout << \"Mesh[\"<<m<<\"]: \"<< mesh->mName.C_Str() << \" vertices=\"<<mesh->mNumVertices<<\" bones=\"<<mesh->mNumBones<<\"\\n\";
            for(unsigned int b=0;b<mesh->mNumBones;b++) {
                const aiBone* bone = mesh->mBones[b];
                std::cout << \"  Bone[\"<<b<<\"]: \"<< bone->mName.C_Str() <<\" weights=\"<<bone->mNumWeights<<\"\\n\";
            }
        }
        // Print first animation channels and duration (if present)
        if(scene->mNumAnimations > 0) {
            const aiAnimation* anim = scene->mAnimations[0];
            std::cout << \"Animation[0]: duration=\"<<anim->mDuration<<\" ticksPerSecond=\"<<anim->mTicksPerSecond<<\" channels=\"<<anim->mNumChannels<<\"\\n\";
            for(unsigned int c=0;c<anim->mNumChannels;c++) {
                const aiNodeAnim* ch = anim->mChannels[c];
                std::cout << \"  Channel: \"<< ch->mNodeName.C_Str() << \" posKeys=\"<< ch->mNumPositionKeys <<\" rotKeys=\"<< ch->mNumRotationKeys <<\"\\n\";
            }
        } else {
            std::cout << \"No animations in scene.\\n\";
        }
        // Simple CPU skinning demo for mesh 0, if bones exist: compute skinned position for first vertex
        if(scene->mNumMeshes > 0 && scene->mMeshes[0]->mNumBones > 0) {
            const aiMesh* mesh = scene->mMeshes[0];
            // Build bone name -> offset matrix map
            std::unordered_map<std::string, glm::mat4> boneOffset;
            for(unsigned int b=0;b<mesh->mNumBones;b++) {
                const aiBone* bone = mesh->mBones[b];
                aiMatrix4x4 m = bone->mOffsetMatrix;
                glm::mat4 off = glm::transpose(glm::make_mat4(&m.a1)); // Assimp matrix to glm (note: may need transpose)
                boneOffset[bone->mName.C_Str()] = off;
            }
            // For demo, compute skinning for vertex 0
            glm::vec4 skinnedPos(0.0f);
            aiVector3D v = mesh->mVertices[0];
            glm::vec4 pos(v.x, v.y, v.z, 1.0f);
            // Accumulate weights from bones affecting vertex 0
            for(unsigned int b=0;b<mesh->mNumBones;b++) {
                const aiBone* bone = mesh->mBones[b];
                for(unsigned int w=0; w<bone->mNumWeights; w++) {
                    const aiVertexWeight& vw = bone->mWeights[w];
                    if(vw.mVertexId == 0) {
                        float weight = vw.mWeight;
                        glm::mat4 offset = boneOffset[bone->mName.C_Str()];
                        skinnedPos += (offset * pos) * weight;
                    }
                }
            }
            std::cout << \"Skinned position (vertex 0): \"<< skinnedPos.x <<\",\"<< skinnedPos.y <<\",\"<< skinnedPos.z <<\"\\n\";
        }
        std::cout << \"Demo finished.\\n\";
        return 0;
    }
