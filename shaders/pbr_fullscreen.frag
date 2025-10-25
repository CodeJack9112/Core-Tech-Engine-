// Minimal PBR lighting fullscreen fragment shader (GLSL pseudocode)
// Inputs: gbuffer textures (albedo, normalRoughness, metallicAO, depth)
// Outputs: final color
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D uAlbedo;
uniform sampler2D uNormalRoughness;
uniform sampler2D uMetallicAO;
uniform sampler2D uDepth;

void main() {
    vec3 albedo = texture(uAlbedo, TexCoord).rgb;
    vec3 packed = texture(uNormalRoughness, TexCoord).rgb;
    vec3 normal = packed.rgb * 2.0 - 1.0;
    float roughness = packed.a;
    float metallic = texture(uMetallicAO, TexCoord).r;
    // Very simplified PBR: direct light + ambient
    vec3 ambient = vec3(0.03) * albedo;
    vec3 lightDir = normalize(vec3(0.3,0.8,0.6));
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = albedo * NdotL;
    FragColor = vec4(diffuse + ambient, 1.0);
}