#version 330 core
in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

// Material properties
uniform vec3 uViewPos;
uniform vec3 uLightDir; // directional light (normalized)
uniform vec3 uAlbedo; // base color
uniform float uSpecPower; // shininess exponent (UE3 uses Blinn-Phong style)
uniform float uSpecIntensity; // specular intensity
uniform float uRoughness; // roughness-like control to modulate specular width (approx)

// UE3-style Blinn-Phong approximation with energy conservation tweak
vec3 BlinnPhong(vec3 N, vec3 V, vec3 L, float shininess, float intensity){
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float spec = 0.0;
    if(NdotL > 0.0){
        spec = pow(NdotH, shininess) * intensity;
    }
    // Energy compensation (simple)
    spec *= 1.0 / max(0.0001, shininess * 0.02);
    return vec3(spec);
}

void main(){
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir); // light direction is from light to surface in this convention
    vec3 V = normalize(uViewPos - vPosition);
    vec3 diffuse = uAlbedo * max(dot(N, L), 0.0);

    vec3 spec = BlinnPhong(N, V, L, uSpecPower, uSpecIntensity);

    // Simple ambient term (UE3 had ambient + environment terms)
    vec3 ambient = 0.1 * uAlbedo;

    vec3 color = ambient + diffuse + spec;
    // Gamma correct-ish
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
