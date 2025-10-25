#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
layout(location=3) in ivec4 aBoneIDs;
layout(location=4) in vec4 aBoneWeights;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 u_BoneMatrices[100]; // supports up to 100 bones

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main(){
    mat4 skinMat = mat4(0.0);
    for(int i=0;i<4;i++){
        int id = aBoneIDs[i];
        float w = aBoneWeights[i];
        if(id >= 0 && id < 100 && w>0.0){
            skinMat += u_BoneMatrices[id] * w;
        }
    }
    vec4 skinnedPos = skinMat * vec4(aPos, 1.0);
    vec4 worldPos = uModel * skinnedPos;
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(uModel))) * mat3(skinMat) * aNormal;
    TexCoords = aUV;
    gl_Position = uProj * uView * worldPos;
}