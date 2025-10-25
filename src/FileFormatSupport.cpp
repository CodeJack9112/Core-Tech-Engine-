\
    #include "FileFormatSupport.h"
    #include <iostream>
    #include <fstream>
    #include <algorithm>

    // Try to include libraries if available
    #ifdef __has_include
    #  if __has_include(<assimp/Importer.hpp>)
    #    include <assimp/Importer.hpp>
    #    include <assimp/scene.h>
    #    include <assimp/postprocess.h>
    #    define HAVE_ASSIMP 1
    #  endif
    #  if __has_include(<stb_image.h>)
    #    include <stb_image.h>
    #    define HAVE_STB 1
    #  endif
    #  if __has_include(<miniaudio.h>)
    #    include <miniaudio.h>
    #    define HAVE_MINIAUDIO 1
    #  endif
    #  if __has_include(<tiny_obj_loader.h>)
    #    include <tiny_obj_loader.h>
    #    define HAVE_TINYOBJ 1
    #  endif
    #  if __has_include(<tiny_gltf.h>)
    #    include <tiny_gltf.h>
    #    define HAVE_TINYGLTF 1
    #  endif
    #endif

    #include <glm/glm.hpp>

    std::unique_ptr<ImageData> LoadImageFile(const std::string& path) {
    #ifdef HAVE_STB
        int w,h,c;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
        if(!data) {
            std::cerr << \"stb_image failed to load: \" << path << std::endl;
            return nullptr;
        }
        auto img = std::make_unique<ImageData>();
        img->width = w; img->height = h; img->channels = c;
        img->pixels.assign(data, data + (w*h*c));
        stbi_image_free(data);
        return img;
    #else
        std::cerr << \"stb_image not available. Put stb_image.h in thirdparty/include or enable FetchContent.\" << std::endl;
        return nullptr;
    #endif
    }

    std::unique_ptr<SoundData> LoadAudioFile(const std::string& path) {
    #ifdef HAVE_MINIAUDIO
        // Simple miniaudio decode example (requires miniaudio.h implementation in one translation unit)
        ma_result r;
        ma_decoder decoder;
        r = ma_decoder_init_file(path.c_str(), NULL, &decoder);
        if(r != MA_SUCCESS) {
            std::cerr << \"miniaudio failed to open: \" << path << std::endl;
            return nullptr;
        }
        ma_uint64 frameCount = ma_decoder_get_length_in_pcm_frames(&decoder);
        ma_uint32 channels = ma_decoder_get_channels(&decoder);
        ma_uint32 sampleRate = ma_decoder_get_sample_rate(&decoder);
        std::vector<int16_t> samples(frameCount * channels);
        ma_decoder_read_pcm_frames(&decoder, samples.data(), frameCount);
        ma_decoder_uninit(&decoder);
        auto sd = std::make_unique<SoundData>();
        sd->sampleRate = (int)sampleRate; sd->channels = (int)channels;
        sd->samples = std::move(samples);
        return sd;
    #else
        std::cerr << \"miniaudio not available. Place miniaudio.h in thirdparty/include or enable FetchContent.\" << std::endl;
        return nullptr;
    #endif
    }

    std::unique_ptr<ModelSummary> LoadModelFileSummary(const std::string& path) {
    #ifdef HAVE_ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
        if(!scene) {
            std::cerr << \"Assimp failed to load: \" << importer.GetErrorString() << std::endl;
            return nullptr;
        }
        auto ms = std::make_unique<ModelSummary>();
        ms->filePath = path;
        ms->meshCount = (int)scene->mNumMeshes;
        int bones = 0;
        for(unsigned int m=0;m<scene->mNumMeshes;m++) bones += scene->mMeshes[m]->mNumBones;
        ms->boneCount = bones;
        ms->animationCount = (int)scene->mNumAnimations;
        for(unsigned int m=0;m<scene->mNumMeshes;m++) {
            ms->meshNames.push_back(scene->mMeshes[m]->mName.C_Str());
        }
        return ms;
    #else
        // Fallback notes: tinyobj (OBJ) or tinygltf (glTF) could be used if present.
    #ifdef HAVE_TINYOBJ
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        if(!ok) { std::cerr << \"tinyobj failed: \"<< warn << err << std::endl; return nullptr; }
        auto ms = std::make_unique<ModelSummary>();
        ms->filePath = path;
        ms->meshCount = (int)shapes.size();
        ms->boneCount = 0;
        ms->animationCount = 0;
        for(auto &s: shapes) ms->meshNames.push_back(s.name);
        return ms;
    #elif defined(HAVE_TINYGLTF)
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err; std::string warn;
        bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
        if(!ret) {
            // try ASCII glTF
            ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        }
        if(!ret) { std::cerr<<\"tinygltf failed: \"<<err<<warn<<std::endl; return nullptr; }
        auto ms = std::make_unique<ModelSummary>();
        ms->filePath = path;
        ms->meshCount = (int)model.meshes.size();
        ms->boneCount = 0;
        ms->animationCount = (int)model.animations.size();
        for(auto &m: model.meshes) ms->meshNames.push_back(m.name);
        return ms;
    #else
        std::cerr << \"No model loader available (Assimp/tinyobj/tinygltf).\" << std::endl;
        return nullptr;
    #endif
    #endif // HAVE_ASSIMP
    }
