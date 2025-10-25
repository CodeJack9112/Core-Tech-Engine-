#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uAlbedo;
uniform vec3 uLightDir;
uniform vec3 uViewPos;
uniform float uContrast; // e.g., 1.2
uniform float uSaturation; // e.g., 0.85
uniform float uGrainStrength; // e.g., 0.04
uniform vec3 uTint; // e.g., vec3(0.8,0.9,1.0) * subtle blue

// Simple filmic tonemapping (Uncharted-like curve approximation)
vec3 FilmicToneMap(vec3 c) {
    c = max(vec3(0.0), c - 0.004);
    vec3 num = c * (6.2 * c + 0.5);
    vec3 den = c * (6.2 * c + 1.7) + 0.06;
    return num / den;
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec3 albedo = texture(uAlbedo, vUV).rgb;
    // lighting
    float NdotL = max(dot(normalize(vNormal), normalize(-uLightDir)), 0.0);
    vec3 lit = albedo * (0.2 + 0.8 * NdotL);
    // contrast
    lit = ((lit - 0.5) * uContrast) + 0.5;
    // saturation
    float luma = dot(lit, vec3(0.2126, 0.7152, 0.0722));
    lit = mix(vec3(luma), lit, uSaturation);
    // tint + tone map
    lit *= uTint;
    lit = FilmicToneMap(lit);
    // film grain
    float g = rand(vUV * vec2(1920.0,1080.0)) * 2.0 - 1.0;
    lit += g * uGrainStrength;
    FragColor = vec4(clamp(lit, 0.0, 1.0), 1.0);
}
