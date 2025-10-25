#pragma once
#include <string>
namespace CoreTech {
class IBLGenerator {
public:
    // Initialize GL resources
    static void init(int size = 512);
    // Generate irradiance cubemap from an HDR equirectangular texture
    // inputPath: HDR equirectangular image path
    // outIrradiance: OpenGL texture handle for irradiance cube
    // outPrefilter: OpenGL texture handle for prefiltered specular cube
    // outBRDFLUT: OpenGL texture handle for 2D BRDF LUT
    static bool generateIBL(const std::string& inputPath, unsigned int& outIrradiance, unsigned int& outPrefilter, unsigned int& outBRDFLUT);
    // utility to save generated maps to disk (optional)
    static bool saveTextureToFile(unsigned int tex, const std::string& path);
};
}