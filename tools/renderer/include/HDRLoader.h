#pragma once
#include <string>
#include <vector>
namespace CoreTech {
class HDRLoader {
public:
    // Load HDR or LDR into float RGBA buffer. Returns width,height and fills data as float[width*height*4]
    static bool loadImageRGBAf(const std::string& path, int& width, int& height, std::vector<float>& data);
};
}