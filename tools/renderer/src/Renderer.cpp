#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "MaterialComponent.h"
#include <map>
#include <stb_image.h>

#include <iostream>
#include <vector>
#include <string>

namespace CoreTech {

struct Renderer::Impl {
    std::unique_ptr<CoreTech::Shader> shader;
    std::map<std::string, unsigned int> textures;
    unsigned int cubeVAO=0, cubeVBO=0;
    unsigned int shaderProgram=0;
    int width=800, height=600;
};

static const char* vertexSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
void main(){
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
)";

static const char* fragSrc = R"(
#version 330 core
out vec4 FragColor;
void main(){
    vec3 lightDir = normalize(vec3(0.5,1.0,0.3));
    float diff = max(dot(normalize(vec3(0,0,1)), lightDir), 0.2);
    vec3 col = vec3(0.4,0.7,0.9) * diff;
    FragColor = vec4(col, 1.0);
}
)";

Renderer::Renderer(): p(new Impl()){}

Renderer& Renderer::instance(){
    static Renderer inst;
    return inst;
}

bool Renderer::init(int width, int height){
    p->width = width; p->height = height;
    // compile shader
    auto compile = [](GLenum type, const char* src)->unsigned int{
        unsigned int s = glCreateShader(type);
        glShaderSource(s, 1, &src, NULL);
        glCompileShader(s);
        int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if(!ok){ char buf[512]; glGetShaderInfoLog(s,512,NULL,buf); std::cerr<<"Shader compile error: "<<buf<<"\n"; return 0; }
        return s;
    };
    unsigned int vs = compile(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fragSrc);
    if(!vs || !fs) return false;
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    int ok; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok){ char buf[512]; glGetProgramInfoLog(prog,512,NULL,buf); std::cerr<<"Shader link error: "<<buf<<"\n"; return false; }
    glDeleteShader(vs); glDeleteShader(fs);
    p->shaderProgram = prog;

    // cube vertices (position only)
    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
    };
    glGenVertexArrays(1, &p->cubeVAO);
    glGenBuffers(1, &p->cubeVBO);
    glBindVertexArray(p->cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, p->cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Renderer::resize(int w, int h){
    p->width = w; p->height = h;
    glViewport(0,0,w,h);
}

void Renderer::beginFrame(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame(){
    // flush
}

void Renderer::drawCube(const glm::mat4& view, const glm::mat4& proj, const glm::mat4& model){
    glUseProgram(p->shaderProgram);
    int mid = glGetUniformLocation(p->shaderProgram, "uModel");
    int vid = glGetUniformLocation(p->shaderProgram, "uView");
    int pid = glGetUniformLocation(p->shaderProgram, "uProj");
    glUniformMatrix4fv(mid,1,GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(vid,1,GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(pid,1,GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(p->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glUseProgram(0);
}
} // namespace CoreTech


// Draw all entities in a scene as cubes at their transform
#include "../gameplay/include/SceneLoader.h"
#include "../gameplay/include/Entity.h"
#include "../gameplay/include/SceneLoader.h" // include for access - note: in-project include; acceptable for prototype
#include <glm/gtc/matrix_transform.hpp>

namespace CoreTech {

void drawEntityAsCube(std::shared_ptr<Scene> scene){
    if(!scene) return;
    auto ents = scene->getAllEntities();
    for(auto &e: ents){
        auto t = e->getComponent<TransformComponent>();
        if(!t) continue;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(t->x, t->y, t->z));
        model = glm::scale(model, glm::vec3(t->sx, t->sy, t->sz));
        // identity rotation applied; rotation support can be added
        // compute view/proj from defaults here (caller should pass real view/proj)
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)p->width/(float)p->height, 0.1f, 100.0f);
        Renderer::instance().drawCube(view, proj, model);
    }
}
} // namespace CoreTech


void Renderer::loadDefaultResources(){
    if(!p->shader){
        const char* vs = R"(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uModel; uniform mat4 uView; uniform mat4 uProj;
void main(){ gl_Position = uProj * uView * uModel * vec4(aPos,1.0); }
)";
        const char* fs = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main(){ FragColor = vec4(uColor,1.0); }
)";
        p->shader = std::make_unique<CoreTech::Shader>();
        p->shader->loadFromStrings(vs, fs);
    }
}

void Renderer::drawMesh(const glm::mat4& view, const glm::mat4& proj, const glm::mat4& model){
    loadDefaultResources();
    p->shader->use();
    p->shader->setMat4("uView", view);
    p->shader->setMat4("uProj", proj);
    p->shader->setMat4("uModel", model);
    // set a default color
    p->shader->setVec3("uColor", glm::vec3(0.6,0.6,0.85));
    glBindVertexArray(p->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Renderer::setBoneMatrices(const std::vector<float>& mats){
    // store current bone matrices to be uploaded before drawing skinned meshes
    if(!p->shader) return;
    // upload as uniform array (shader must declare u_BoneMatrices)
    if(mats.size()>0){
        p->shader->use();
        p->shader->setMat4Array("u_BoneMatrices", mats.data(), (int)(mats.size()/16));
    }
}


#include <filesystem>
#include <chrono>
#include <unordered_map>
#include <sys/stat.h>

static std::unordered_map<std::string, std::time_t> shaderTimestamps;

#include <unordered_map>

static std::unordered_map<std::string, unsigned int> shaderPrograms;

static void replaceProgram(const std::string &key, unsigned int newProg) {
    auto it = shaderPrograms.find(key);
    if(it != shaderPrograms.end()) {
        // delete old program if GL available
        #ifdef HAVE_GLAD
        if(it->second) glDeleteProgram(it->second);
        #endif
        it->second = newProg;
    } else {
        shaderPrograms[key] = newProg;
    }
}


static std::time_t get_mtime(const std::string& path) {
    try {
        auto ftime = std::filesystem::last_write_time(path);
        return decltype(ftime)::clock::to_time_t(ftime);
    } catch(...) {
        return 0;
    }
}

void Renderer::reloadShadersIfNeeded() {
    // simple check for shader files under shaders/; recompile if changed
    std::vector<std::pair<std::string,std::string>> shaderPairs = {
        {\"shaders/simple.vs\",\"shaders/simple.fs\"},
    };
    for(auto &p : shaderPairs) {
        auto vs = p.first, fs = p.second;
        auto vsmt = get_mtime(vs);
        auto fsmt = get_mtime(fs);
        std::time_t last = 0;
        auto it = shaderTimestamps.find(vs+\"|\"+fs);
        if(it!=shaderTimestamps.end()) last = it->second;
        if(vsmt>last || fsmt>last) {
            unsigned int prog = 0;
            if(compileShader(vs, fs, prog)) {
                shaderTimestamps[vs+\"|\"+fs] = std::max(vsmt, fsmt);
                // replace current shader (not shown: manage program lifetimes)
                std::cout << \"[Renderer] shader recompiled: \" << vs << \",\" << fs << \" prog=\" << prog << \"\\n\";
            } else {
                std::cerr << \"[Renderer] shader compile failed: \" << vs << \",\" << fs << \"\\n\";
            }
        }
    }
}

bool Renderer::compileShader(const std::string& vsSrc, const std::string& fsSrc, unsigned int &outProgram) {
    // naive: load from file and compile using GL functions if available; else return false
#ifdef HAVE_GLAD
    std::string vsCode, fsCode;
    try {
        std::ifstream ifs(vsSrc); vsCode.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        std::ifstream ifs2(fsSrc); fsCode.assign((std::istreambuf_iterator<char>(ifs2)), std::istreambuf_iterator<char>());
    } catch(...) { return false; }
    auto compile = [&](unsigned int type, const std::string &src)->unsigned int {
        unsigned int s = glCreateShader(type);
        const char* cstr = src.c_str();
        glShaderSource(s,1,&cstr,nullptr);
        glCompileShader(s);
        int ok=0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if(!ok) {
            int len=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
            std::string log(len, '\\0'); glGetShaderInfoLog(s, len, nullptr, log.data());
            std::cerr << \"Shader compile log: \" << log << \"\\n\";
            glDeleteShader(s);
            return 0;
        }
        return s;
    };
    unsigned int vs = compile(GL_VERTEX_SHADER, vsCode);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fsCode);
    if(!vs || !fs) return false;
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    int ok=0; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok) {
        int len=0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\\0'); glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::cerr << \"Program link log: \" << log << \"\\n\";
        glDeleteProgram(prog);
        glDeleteShader(vs); glDeleteShader(fs);
        return false;
    }
    glDeleteShader(vs); glDeleteShader(fs);
    outProgram = prog;
    // manage program lifetime
    replaceProgram(vsSrc+"|"+fsSrc, prog);
    return true;
#else
    (void)vsSrc; (void)fsSrc; (void)outProgram;
    return false;
#endif
}
