#version 330 core
out vec4 FragColor; in vec3 WorldPos;
uniform samplerCube environmentMap;
void main(){
    // cosine-weighted convolution approximation (stub)
    vec3 N = normalize(WorldPos);
    vec3 irradiance = vec3(0.0);
    // simple sample - not real convolution; placeholder for contributor to implement importance sampling
    irradiance = texture(environmentMap, N).rgb;
    FragColor = vec4(irradiance,1.0);
}