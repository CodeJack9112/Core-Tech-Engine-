#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace CoreTech {
static unsigned int compileShader(unsigned int type, const char* src){
    unsigned int s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){ char buf[1024]; glGetShaderInfoLog(s,1024,NULL,buf); std::cerr<<"Shader compile error: "<<buf<<"\n"; return 0; }
    return s;
}

bool Shader::loadFromStrings(const std::string& vsSrc, const std::string& fsSrc){
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vsSrc.c_str());
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fsSrc.c_str());
    if(!vs || !fs) return false;
    id = glCreateProgram();
    glAttachShader(id, vs); glAttachShader(id, fs);
    glLinkProgram(id);
    int ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if(!ok){ char buf[1024]; glGetProgramInfoLog(id,1024,NULL,buf); std::cerr<<"Shader link error: "<<buf<<"\n"; return false; }
    glDeleteShader(vs); glDeleteShader(fs);
    return true;
}

bool Shader::loadFromFiles(const std::string& vsPath, const std::string& fsPath){
    std::ifstream a(vsPath), b(fsPath);
    if(!a || !b) return false;
    std::stringstream sa, sb; sa<<a.rdbuf(); sb<<b.rdbuf();
    return loadFromStrings(sa.str(), sb.str());
}

void Shader::use(){ if(id) glUseProgram(id); }
int Shader::getUniformLocation(const std::string& name){
    if(cache.count(name)) return cache[name];
    int loc = glGetUniformLocation(id, name.c_str());
    cache[name]=loc; return loc;
}
void Shader::setMat4(const std::string& name, const glm::mat4& m){ int loc = getUniformLocation(name); if(loc>=0) glUniformMatrix4fv(loc,1,GL_FALSE, glm::value_ptr(m)); }
void Shader::setVec3(const std::string& name, const glm::vec3& v){ int loc = getUniformLocation(name); if(loc>=0) glUniform3f(loc, v.x, v.y, v.z); }
}

void Shader::setMat4Array(const std::string& name, const float* data, int count){
    int loc = getUniformLocation(name);
    if(loc>=0){
        glUniformMatrix4fv(loc, count, GL_FALSE, data);
    }
}
