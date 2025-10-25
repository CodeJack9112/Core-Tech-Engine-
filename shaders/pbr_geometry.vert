// Vertex shader for geometry pass (pseudocode)
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec2 TexCoord;
out vec3 WorldNormal;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    TexCoord = aUV;
    WorldNormal = mat3(uModel) * aNormal;
    gl_Position = uViewProj * worldPos;
}