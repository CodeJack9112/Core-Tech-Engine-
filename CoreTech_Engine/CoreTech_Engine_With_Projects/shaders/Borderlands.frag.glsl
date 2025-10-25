#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;
out vec4 FragColor;

uniform vec3 uViewPos;
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uBaseColor;
uniform float uLevels;
uniform float uEdgeBias; // bias to darken toward edges

void main() {
    float NdotL = max(dot(normalize(vNormal), normalize(-uLightDir)), 0.0);
    float quant = floor(NdotL * uLevels) / max(uLevels - 1.0, 1.0);
    // edge detection approximation using view-normal angle
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    float edge = pow(1.0 - abs(dot(viewDir, normalize(vNormal))), 1.5);
    // Darken color near edges to simulate ink shading
    float edgeFactor = smoothstep(0.0, 1.0, edge * uEdgeBias);
    vec3 color = uBaseColor * uLightColor * quant * (1.0 - 0.6*edgeFactor);
    // Add slight noise-like shade by modulating with vUV (cheap fake)
    float hatch = fract(sin(dot(vUV.xy ,vec2(12.9898,78.233))) * 43758.5453);
    color *= mix(1.0, 0.95, hatch * 0.2);
    FragColor = vec4(color, 1.0);
}
