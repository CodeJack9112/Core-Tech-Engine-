#version 330 core
out vec4 FragColor; in vec3 WorldPos;
uniform samplerCube environmentMap; uniform float roughness;
void main(){
    vec3 R = normalize(WorldPos);
    // placeholder: sample environment map with LOD based on roughness
    vec3 prefiltered = textureLod(environmentMap, R, roughness * 4.0).rgb;
    FragColor = vec4(prefiltered,1.0);
}