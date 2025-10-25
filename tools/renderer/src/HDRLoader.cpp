#include "HDRLoader.h"
#include <iostream>
#ifdef __has_include
  #if __has_include("stb_image.h")
    #define HAVE_STB 1
  #endif
#endif
#if HAVE_STB
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

namespace CoreTech {
bool HDRLoader::loadImageRGBAf(const std::string& path, int& width, int& height, std::vector<float>& data){
#if HAVE_STB
    int comp = 0;
    float* img = stbi_loadf(path.c_str(), &width, &height, &comp, 4);
    if(!img) { std::cerr << "HDRLoader: failed to load " << path << "\\n"; return false; }
    data.assign(img, img + (size_t)width*(size_t)height*4);
    stbi_image_free(img);
    return true;
#else
    (void)path;(void)width;(void)height;(void)data;
    return false;
#endif
}
}