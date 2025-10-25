#pragma once
#include <string>
#include <functional>
#include <vector>
struct StreamingTexture { std::string path; int width,height,channels; std::vector<unsigned char> pixels; };
struct TextureStreaming {
  void Start(); void Stop(); void Request(const std::string& path, std::function<void(const StreamingTexture&)> cb);
};
