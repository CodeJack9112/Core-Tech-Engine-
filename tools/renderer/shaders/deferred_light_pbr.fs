#version 330 core
out vec4 FragColor;
// G-buffer samplers
uniform sampler2D gPosition; uniform sampler2D gNormal; uniform sampler2D gAlbedoRoughMetal;
uniform samplerCube uIrradianceMap; uniform samplerCube uPrefilterMap; uniform sampler2D uBRDFLUT;
in vec2 TexCoords;
void main(){
    vec3 position = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 albedo = texture(gAlbedoRoughMetal, TexCoords).rgb;
    float roughness = texture(gAlbedoRoughMetal, TexCoords).a; // assume packed
    float metallic = 0.0; // TODO: unpack from another channel
    // Sample IBL (stub)
    vec3 irradiance = texture(uIrradianceMap, normal).rgb;
    vec3 diffuse = irradiance * albedo;
    vec3 prefiltered = textureLod(uPrefilterMap, reflect(-normalize(vec3(0.0,0.0,1.0)), normal), roughness * 4.0).rgb;
    vec2 brdf = texture(uBRDFLUT, vec2(max(dot(normal,vec3(0.0,0.0,1.0)),0.0), roughness)).rg;
    vec3 specular = prefiltered * brdf.x + brdf.y;
    vec3 color = diffuse + specular;
    FragColor = vec4(color,1.0);
}