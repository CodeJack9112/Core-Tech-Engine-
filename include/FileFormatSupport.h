#pragma once
// FileFormatSupport: unified loaders for models, images, and audio.
// This header exposes simple API surfaces that use Assimp, stb_image, tinyobjloader, tinygltf, and miniaudio
// when available. Each loader will return a simple struct or nullptr on failure.
//
// Note: This file only defines interfaces and small helpers. Actual implementations are in FileFormatSupport.cpp
// and require third-party libraries present at build time.

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

struct ImageData {
    int width=0, height=0, channels=0;
    std::vector<uint8_t> pixels; // row-major, channels per pixel
};

struct SoundData {
    int sampleRate=0;
    int channels=0;
    std::vector<int16_t> samples; // interleaved
};

struct ModelSummary {
    std::string filePath;
    int meshCount=0;
    int boneCount=0;
    int animationCount=0;
    std::vector<std::string> meshNames;
};

// Attempts to load image via stb_image (or stub). Returns nullptr on failure.
std::unique_ptr<ImageData> LoadImageFile(const std::string& path);

// Attempts to load audio via miniaudio/openal wrapper. Returns nullptr on failure.
std::unique_ptr<SoundData> LoadAudioFile(const std::string& path);

// Loads model using Assimp or fallback loaders (tinyobj, tinygltf). Returns summary on success, nullptr on failure.
std::unique_ptr<ModelSummary> LoadModelFileSummary(const std::string& path);
