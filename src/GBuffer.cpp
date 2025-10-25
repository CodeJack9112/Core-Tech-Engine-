#include "GBuffer.h"
#include <iostream>

namespace CoreTech {

GBuffer::GBuffer() : width(0), height(0), initialized(false) {}

bool GBuffer::Initialize(int w, int h) {
    // Create textures and framebuffers for albedo, normal+roughness, metallic+Ao, depth.
    // Implementation depends on chosen graphics API (OpenGL/DirectX). We'll leave API hooks here.
    width = w; height = h; initialized = true;
    std::cout << "GBuffer initialized " << w << "x" << h << std::endl;
    return true;
}

bool GBuffer::Initialize() {
    // fallback default size; in real integration query backbuffer size
    return Initialize(1280, 720);
}

void GBuffer::BindForWriting() {
    // Bind framebuffer for geometry pass
}

void GBuffer::BindForReading() {
    // Bind textures for lighting pass
}

void GBuffer::Shutdown() {
    // Release textures
    initialized = false;
}

}