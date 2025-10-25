#include "../include/Renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Renderer::loadShaders(const std::string& vsPath, const std::string& fsPath){
    std::ifstream vs(vsPath);
    std::ifstream fs(fsPath);
    if(!vs || !fs) return false;
    std::stringstream vss, fss;
    vss << vs.rdbuf();
    fss << fs.rdbuf();
    m_vertexSrc = vss.str();
    m_fragmentSrc = fss.str();
    return true;
}

bool Renderer::compileShaders(){
    // This is a stub: if you have an OpenGL context and GLEW/GLAD, you can compile shaders here.
    // For now we only check that sources are non-empty.
    if(m_vertexSrc.empty() || m_fragmentSrc.empty()){
        std::cerr << "Renderer: shader sources empty, compile failed" << std::endl;
        return false;
    }
    std::cout << "Renderer: shader sources loaded (compile step skipped in stub)" << std::endl;
    return true;
}

void Renderer::setLightDirection(float x, float y, float z){
    m_lightDir[0]=x; m_lightDir[1]=y; m_lightDir[2]=z;
}

void Renderer::setSpecularPower(float power){ m_specPower = power; }
void Renderer::setSpecularIntensity(float intensity){ m_specIntensity = intensity; }


bool Renderer::loadAlbedoTexture(const std::string& path){
    // Stub: in a real renderer you'd load image data and create GPU texture.
    (void)path;
    return true;
}

bool Renderer::loadNormalTexture(const std::string& path){
    (void)path;
    return true;
}

void Renderer::setMaterial(const Material& mat){
    m_material = mat;
    // apply material defaults to renderer parameters
    m_specPower = mat.specPower;
    m_specIntensity = mat.specIntensity;
}


bool Renderer::loadHLSLShader(const std::string& path){
    std::ifstream fs(path);
    if(!fs) return false;
    std::stringstream ss; ss << fs.rdbuf();
    m_hlslSrc = ss.str();
    std::cout << "Renderer: loaded HLSL shader from " << path << std::endl;
    return true;
}
