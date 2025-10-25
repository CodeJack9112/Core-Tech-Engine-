#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;

out vec4 FragColor;

uniform vec3 uViewPos;
uniform vec3 uLightDir; // directional light (normalized)
uniform vec3 uLightColor;
uniform vec3 uBaseColor;
uniform float uLevels; // number of quantization levels, e.g., 3.0

void main() {
    // Diffuse
    float NdotL = max(dot(normalize(vNormal), normalize(-uLightDir)), 0.0);
    // Quantize lighting into discrete steps
    float quant = floor(NdotL * uLevels) / max(uLevels - 1.0, 1.0);
    vec3 diffuse = uLightColor * uBaseColor * quant;

    // Simple rim lighting for anime-style highlight
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    float rim = pow(1.0 - max(dot(viewDir, normalize(vNormal)), 0.0), 2.0);
    rim = smoothstep(0.0, 1.0, rim);
    vec3 rimColor = vec3(1.0) * rim * 0.25;

    FragColor = vec4(diffuse + rimColor, 1.0);
}
