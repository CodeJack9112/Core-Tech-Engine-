#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uAlbedo;
uniform vec3 uLightDir;
uniform float uContrast;
uniform float uVignette; // 0..1
uniform vec3 uWarmth; // e.g., vec3(1.05,1.02,0.95)

vec3 filmic(vec3 color) {
    color = max(vec3(0.0), color - 0.002);
    return (color*(6.2*color+0.5))/(color*(6.2*color+1.7)+0.06);
}

void main() {
    vec3 base = texture(uAlbedo, vUV).rgb;
    float NdotL = max(dot(normalize(vNormal), normalize(-uLightDir)), 0.0);
    vec3 lit = base * (0.15 + 0.85 * NdotL);
    lit = ((lit - 0.5) * uContrast) + 0.5;
    lit *= uWarmth;
    lit = filmic(lit);
    // vignette
    vec2 pos = vUV - vec2(0.5);
    float vig = smoothstep(0.8, uVignette + 0.4, length(pos));
    lit *= mix(1.0, 0.6, vig);
    FragColor = vec4(lit, 1.0);
}
