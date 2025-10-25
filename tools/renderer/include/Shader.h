#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace CoreTech {
class Shader {
public:
    Shader() = default;
    bool loadFromStrings(const std::string& vsSrc, const std::string& fsSrc);
    bool loadFromFiles(const std::string& vsPath, const std::string& fsPath);
    void use();
    void setMat4(const std::string& name, const glm::mat4& m);
    void setVec3(const std::string& name, const glm::vec3& v);
private:
    unsigned int id = 0;
    std::unordered_map<std::string,int> cache;
public:
    void setMat4Array(const std::string& name, const float* data, int count);

    int getUniformLocation(const std::string& name);
};
}