#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uAlbedo;
uniform vec3 uLightDir;
uniform vec3 uViewPos;
uniform float uDesat; // 0..1
uniform float uContrast;
uniform float uChromAb; // chromatic aberration strength
uniform vec3 uColorGrade; // e.g., slight teal shadows, warm highlights

vec3 TonemapACES(vec3 x) {
    // simple ACES approximation
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec3 sampleChromatic(sampler2D s, vec2 uv, float strength) {
    // sample shifted uv for R and B channels
    vec2 off = (uv - 0.5) * strength;
    float r = texture(s, uv + off).r;
    float g = texture(s, uv).g;
    float b = texture(s, uv - off).b;
    return vec3(r,g,b);
}

void main() {
    vec3 base = texture(uAlbedo, vUV).rgb;
    float NdotL = max(dot(normalize(vNormal), normalize(-uLightDir)), 0.0);
    vec3 lit = base * (0.18 + 0.82 * NdotL);
    // desaturate
    float l = dot(lit, vec3(0.2126,0.7152,0.0722));
    lit = mix(lit, vec3(l), uDesat);
    // contrast and grade
    lit = ((lit - 0.5) * uContrast) + 0.5;
    lit = mix(lit, lit * uColorGrade, 0.12);
    // chromatic aberration (cheap)
    if(uChromAb > 0.0) lit = sampleChromatic(uAlbedo, vUV, uChromAb);
    // tonemap
    lit = TonemapACES(lit);
    FragColor = vec4(lit, 1.0);
}
