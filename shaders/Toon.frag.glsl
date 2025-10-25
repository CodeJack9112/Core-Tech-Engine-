#version 330 core
// Toon / Cel shading fragment shader (quantized lighting + simple rim)
in vec3 vNormal;
in vec3 vViewPos;
in vec2 vTexCoord;

uniform vec3 uLightDir; // should be normalized, in view space
uniform sampler2D uAlbedo;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform float uLevels; // e.g. 3.0 for 3-tone shading
uniform float uRimPower; // rim exponent
uniform vec3 uRimColor;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightDir);
    float ndotl = max(dot(N, L), 0.0);
    // Quantize lighting into discrete bands
    float quant = floor(ndotl * uLevels) / (uLevels - 1.0);
    // Basic texture
    vec4 albedo = texture(uAlbedo, vTexCoord);
    vec3 color = albedo.rgb * (uAmbientColor + uLightColor * quant);
    // Rim lighting (view-space)
    vec3 V = normalize(-vViewPos);
    float rim = pow(max(0.0, 1.0 - dot(N, V)), uRimPower);
    color += uRimColor * rim;
    FragColor = vec4(color, albedo.a);
}
