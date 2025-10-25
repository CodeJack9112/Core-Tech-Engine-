RendererGL g_renderer_global;


#include "../include/RendererGL.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

// Note: this implementation assumes you have GLFW and GLAD available and linked in your build.
#include <GLFW/glfw3.h>
#include <glad/glad.h>

static const char* vertexShaderSrc = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
uniform mat4 uMVP;
out vec3 vNormal;
out vec2 vTex;
void main(){
    vNormal = aNormal;
    vTex = aTex;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

"#version 460 core
in vec3 vNormal;
in vec2 vTex;
out vec4 FragColor;

uniform sampler2D uAlbedo;
uniform sampler2D uNormalMap;
uniform sampler2D uORM; // R=AO, G=Roughness, B=Metallic

vec3 getNormalFromMap(vec2 uv) {
    vec3 tangentNormal = texture(uNormalMap, uv).xyz * 2.0 - 1.0;
    return normalize(tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;
    return num / max(denom, 0.000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main(){
    vec3 albedo = pow(texture(uAlbedo, vTex).rgb, vec3(2.2)); // assume sRGB texture, approximate linearization
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vec3(0.0,0.0,1.0));
    vec3 L = normalize(vec3(0.5,0.8,0.3));
    vec3 H = normalize(V + L);
    vec3 orm = texture(uORM, vTex).rgb;
    float ao = orm.r;
    float roughness = clamp(orm.g, 0.05, 1.0);
    float metallic = clamp(orm.b, 0.0, 1.0);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Nrm = N;
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(Nrm, H, roughness);
    float G   = GeometrySmith(Nrm, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(Nrm, V), 0.001) * max(dot(Nrm, L), 0.001);
    vec3 specular = nominator / max(vec3(denominator), vec3(0.001));

    float NdotL = max(dot(Nrm, L), 0.0);
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 Lo = (kD * albedo / 3.14159265 + specular) * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); // gamma
    FragColor = vec4(color, 1.0);
}
)"

RendererGL::~RendererGL(){ shutdown(); }

bool RendererGL::initialize(int width, int height, const char* title){
    m_width = width; m_height = height;
    if(!glfwInit()){
        std::cerr << "[RendererGL] Failed to init GLFW\\n"; return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* w = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!w){ std::cerr << "[RendererGL] Failed to create window\\n"; glfwTerminate(); return false; }
    m_window = w;
    glfwMakeContextCurrent(w);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "[RendererGL] Failed to init GLAD\\n"; return false;
    }

    // compile shader
    unsigned int vs=0, fs=0;
    if(!compileShader(vertexShaderSrc, GL_VERTEX_SHADER, vs)){ std::cerr << "[RendererGL] VS compile failed\\n"; }
    if(!compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, fs)){ std::cerr << "[RendererGL] FS compile failed\\n"; }
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);
    int linked=0; glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linked);
    if(!linked){ char buf[1024]; glGetProgramInfoLog(m_shaderProgram,1024,nullptr,buf); std::cerr << "[RendererGL] Link error: " << buf << "\\n"; }

    glDeleteShader(vs); glDeleteShader(fs);

    glEnable(GL_DEPTH_TEST);
    createDemoMesh();
    std::cout << "[RendererGL] Initialized OpenGL " << glGetString(GL_VERSION) << "\\n";
    return true;
}

void RendererGL::shutdown(){
    destroyDemoMesh();
    if(m_shaderProgram){ glDeleteProgram(m_shaderProgram); m_shaderProgram=0; }
    if(m_window){
        glfwDestroyWindow((GLFWwindow*)m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void RendererGL::runMainLoop(class PhysicsManager* phys){
    GLFWwindow* w = (GLFWwindow*)m_window;
    if(!w) return;
    using clk = std::chrono::high_resolution_clock;
    auto last = clk::now();
    while(!glfwWindowShouldClose(w)){
        auto now = clk::now();
        std::chrono::duration<float> diff = now - last; last = now;
        float dt = diff.count();
        // simple fixed-step physics call if provided
        if(phys) phys->update(dt);

        glfwPollEvents();
        renderFrame();

        glfwSwapBuffers(w);
    }
}

void RendererGL::renderFrame(){
    glViewport(0,0,m_width,m_height);
    glClearColor(0.1f,0.12f,0.15f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shaderProgram);
    // simple MVP matrix: identity with small projection
    float aspect = (float)m_width / (float)m_height;
    float proj[16] = {1.0f/aspect,0,0,0, 0,1.0f,0,0, 0,0,-1.0f,-1.0f, 0,0,-0.2f,0};
    int loc = glGetUniformLocation(m_shaderProgram, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glUseProgram(0);
}

bool RendererGL::compileShader(const std::string& src, unsigned int shaderType, unsigned int &outShader){
    const char* s = src.c_str();
    unsigned int sh = glCreateShader(shaderType);
    glShaderSource(sh, 1, &s, nullptr);
    glCompileShader(sh);
    int compiled=0; glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
    if(!compiled){ char buf[1024]; glGetShaderInfoLog(sh,1024,nullptr,buf); std::cerr << "[RendererGL] Shader compile error: " << buf << "\\n"; return false; }
    outShader = sh; return true;
}


#include "MeshLoader.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLMesh { unsigned int vao=0, vbo=0; uint32_t vertexCount=0; };

static std::unordered_map<std::string, GLMesh> g_meshes;

// Register mesh data and create VAO/VBO. Returns true on success.
bool registerMesh(const std::string& path, const MeshData& md){
    if(g_meshes.find(path) != g_meshes.end()) return true;
    GLMesh gm;
    glGenVertexArrays(1, &gm.vao);
    glGenBuffers(1, &gm.vbo);
    glBindVertexArray(gm.vao);
    glBindBuffer(GL_ARRAY_BUFFER, gm.vbo);
    glBufferData(GL_ARRAY_BUFFER, md.vertices.size()*sizeof(float), md.vertices.data(), GL_STATIC_DRAW);
    // pos
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    // normal
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    // texcoord
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glBindVertexArray(0);
    gm.vertexCount = md.vertexCount;
    g_meshes[path] = gm;
    return true;
}

void renderMeshInstance(const std::string& path, const glm::vec3& pos){
    auto it = g_meshes.find(path);
    if(it == g_meshes.end()) return;
    GLMesh &gm = it->second;
    // build MVP (simple translate)
    float mvp[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    // apply translate in mvp[12..14]
    mvp[12] = pos.x; mvp[13] = pos.y; mvp[14] = pos.z;
    int loc = glGetUniformLocation(m_shaderProgram, "uMVP");
    glUseProgram(m_shaderProgram);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
    glBindVertexArray(gm.vao);
    glDrawArrays(GL_TRIANGLES, 0, gm.vertexCount);
    glBindVertexArray(0);
    glUseProgram(0);
}
void RendererGL::createDemoMesh(){
    // create a simple cube VBO/VAO
    float verts[] = {
        // positions        // normals       // texcoords
        -0.5f,-0.5f,-0.5f,  0,0,-1,   0,0,
         0.5f,-0.5f,-0.5f,  0,0,-1,   1,0,
         0.5f, 0.5f,-0.5f,  0,0,-1,   1,1,
         0.5f, 0.5f,-0.5f,  0,0,-1,   1,1,
        -0.5f, 0.5f,-0.5f,  0,0,-1,   0,1,
        -0.5f,-0.5f,-0.5f,  0,0,-1,   0,0,

        -0.5f,-0.5f, 0.5f,  0,0,1,   0,0,
         0.5f,-0.5f, 0.5f,  0,0,1,   1,0,
         0.5f, 0.5f, 0.5f,  0,0,1,   1,1,
         0.5f, 0.5f, 0.5f,  0,0,1,   1,1,
        -0.5f, 0.5f, 0.5f,  0,0,1,   0,1,
        -0.5f,-0.5f, 0.5f,  0,0,1,   0,0,

        -0.5f, 0.5f, 0.5f, -1,0,0,   1,0,
        -0.5f, 0.5f,-0.5f, -1,0,0,   1,1,
        -0.5f,-0.5f,-0.5f, -1,0,0,   0,1,
        -0.5f,-0.5f,-0.5f, -1,0,0,   0,1,
        -0.5f,-0.5f, 0.5f, -1,0,0,   0,0,
        -0.5f, 0.5f, 0.5f, -1,0,0,   1,0,

         0.5f, 0.5f, 0.5f, 1,0,0,   1,0,
         0.5f, 0.5f,-0.5f, 1,0,0,   1,1,
         0.5f,-0.5f,-0.5f, 1,0,0,   0,1,
         0.5f,-0.5f,-0.5f, 1,0,0,   0,1,
         0.5f,-0.5f, 0.5f, 1,0,0,   0,0,
         0.5f, 0.5f, 0.5f, 1,0,0,   1,0,

        -0.5f,-0.5f,-0.5f, 0,-1,0,   0,1,
         0.5f,-0.5f,-0.5f, 0,-1,0,   1,1,
         0.5f,-0.5f, 0.5f, 0,-1,0,   1,0,
         0.5f,-0.5f, 0.5f, 0,-1,0,   1,0,
        -0.5f,-0.5f, 0.5f, 0,-1,0,   0,0,
        -0.5f,-0.5f,-0.5f, 0,-1,0,   0,1,

        -0.5f, 0.5f,-0.5f, 0,1,0,   0,1,
         0.5f, 0.5f,-0.5f, 0,1,0,   1,1,
         0.5f, 0.5f, 0.5f, 0,1,0,   1,0,
         0.5f, 0.5f, 0.5f, 0,1,0,   1,0,
        -0.5f, 0.5f, 0.5f, 0,1,0,   0,0,
        -0.5f, 0.5f,-0.5f, 0,1,0,   0,1
    };
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    // pos
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    // normal
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    // texcoord
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glBindVertexArray(0);
}

void RendererGL::destroyDemoMesh(){
    if(m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if(m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
}

void RendererGL::setViewPos(float x, float y, float z){ m_viewPos[0]=x; m_viewPos[1]=y; m_viewPos[2]=z; }



// ---------- Textured quad helpers ----------
static unsigned int createCheckerTexture() {
    const int W = 128, H = 128;
    std::vector<unsigned char> data(W*H*3);
    for(int y=0;y<H;++y){
        for(int x=0;x<W;++x){
            int checker = ((x/8) % 2) ^ ((y/8) % 2);
            unsigned char v = checker ? 255 : 50;
            int idx = (y*W + x)*3;
            data[idx+0] = v;
            data[idx+1] = v;
            data[idx+2] = v;
        }
    }
    unsigned int tex=0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

void RendererGL::initTexturedQuad(){
    if(m_quadVAO) return;
    float quadVerts[] = {
        // positions      // normals   // tex
        -1.0f, -1.0f, 0.0f,  0,0,1,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  0,0,1,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  0,0,1,  1.0f, 1.0f,
         1.0f,  1.0f, 0.0f,  0,0,1,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0,0,1,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0,0,1,  0.0f, 0.0f
    };
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glBindVertexArray(0);
    m_quadTexture = createCheckerTexture();
}

void RendererGL::renderTexturedQuad(){
    if(!m_quadVAO) return;
    glUseProgram(m_shaderProgram);
    int loc = glGetUniformLocation(m_shaderProgram, "uMVP");
    float mvp[16] = {1,0,0,0, 0,1,0,0, 0,0,-1, -0.2f, 0,0,0,1};
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_quadTexture);
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
// --------------------------------------------



#include <functional>

bool RendererGL::initializeDeferred(int width, int height){
    // create g-buffer FBO
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    // position (RGB16F)
    glGenTextures(1, &m_gPosition);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);
    // normal (RGB16F)
    glGenTextures(1, &m_gNormal);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);
    // albedo (RGBA8)
    glGenTextures(1, &m_gAlbedo);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedo, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // depth renderbuffer
    glGenRenderbuffers(1, &m_rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "[RendererGL] G-Buffer not complete\\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // compile geometry pass shader (vertex same as basic; fragment writes to multiple targets)
    const char* geomVS = vertexShaderSrc;
    const char* geomFS = R"(
#version 460 core
layout(location=0) out vec3 FragPos;
layout(location=1) out vec3 FragNormal;
layout(location=2) out vec4 FragAlbedo;
in vec3 vNormal;
in vec2 vTex;
in vec3 aPos;
uniform sampler2D uAlbedo;
void main(){
    FragPos = aPos;
    FragNormal = normalize(vNormal);
    FragAlbedo = texture(uAlbedo, vTex);
    // gl_Position must be set by caller via uniform uMVP; copy previous logic
    // write full-screen position is not needed here
}
)";
    // Note: Above fragment shader is a placeholder; actual implementation will be replaced in final build.
    unsigned int vs=0, fs=0;
    compileShader(geomVS, GL_VERTEX_SHADER, vs);
    compileShader(geomFS, GL_FRAGMENT_SHADER, fs);
    m_geomProgram = glCreateProgram();
    glAttachShader(m_geomProgram, vs);
    glAttachShader(m_geomProgram, fs);
    glLinkProgram(m_geomProgram);
    glDeleteShader(vs); glDeleteShader(fs);

    // compile composite shader (reads gbuffer and does lighting)
    const char* compVS = R"(
#version 460 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aTex;
out vec2 vUV;
void main(){ vUV = aTex; gl_Position = vec4(aPos,1.0); }
)";
    const char* compFS = R"(
#version 460 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
void main(){
    vec3 pos = texture(gPosition, vUV).rgb;
    vec3 normal = normalize(texture(gNormal, vUV).rgb);
    vec3 albedo = texture(gAlbedo, vUV).rgb;
    vec3 lightDir = normalize(vec3(0.5,0.8,0.3));
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 color = albedo * NdotL + 0.03 * albedo;
    FragColor = vec4(color,1.0);
}
)";
    compileShader(compVS, GL_VERTEX_SHADER, vs);
    compileShader(compFS, GL_FRAGMENT_SHADER, fs);
    m_compProgram = glCreateProgram();
    glAttachShader(m_compProgram, vs);
    glAttachShader(m_compProgram, fs);
    glLinkProgram(m_compProgram);
    glDeleteShader(vs); glDeleteShader(fs);

    return true;
}

void RendererGL::shutdownDeferred(){
    if(m_gPosition){ glDeleteTextures(1, &m_gPosition); m_gPosition = 0; }
    if(m_gNormal){ glDeleteTextures(1, &m_gNormal); m_gNormal = 0; }
    if(m_gAlbedo){ glDeleteTextures(1, &m_gAlbedo); m_gAlbedo = 0; }
    if(m_rboDepth){ glDeleteRenderbuffers(1, &m_rboDepth); m_rboDepth = 0; }
    if(m_gBuffer){ glDeleteFramebuffers(1, &m_gBuffer); m_gBuffer = 0; }
    if(m_geomProgram){ glDeleteProgram(m_geomProgram); m_geomProgram = 0; }
    if(m_compProgram){ glDeleteProgram(m_compProgram); m_compProgram = 0; }
}

void RendererGL::renderDeferred(const std::function<void()>& drawGeometry){
    // geometry pass
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_geomProgram);
    // geometry drawing callback should bind per-mesh textures and VAOs and issue draw calls
    drawGeometry();
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // composite pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_compProgram);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glUniform1i(glGetUniformLocation(m_compProgram, "gPosition"), 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glUniform1i(glGetUniformLocation(m_compProgram, "gNormal"), 1);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
    glUniform1i(glGetUniformLocation(m_compProgram, "gAlbedo"), 2);
    // render full-screen quad
    renderTexturedQuad();
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
