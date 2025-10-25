#version 330 core
in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in mat3 vTBN;

out vec4 FragColor;

uniform vec3 uViewPos;
uniform vec3 uLightDir;
uniform vec3 uAlbedo;
uniform float uSpecPower;
uniform float uSpecIntensity;

// Texture samplers (stubs if not bound)
uniform sampler2D uAlbedoTex;
uniform sampler2D uNormalTex;

vec3 getNormal(){
    vec3 n = texture(uNormalTex, vTexCoord).rgb;
    n = n * 2.0 - 1.0;
    return normalize(vTBN * n);
}

vec3 BlinnPhong(vec3 N, vec3 V, vec3 L, float shininess, float intensity){
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float spec = 0.0;
    if(NdotL > 0.0){
        spec = pow(NdotH, shininess) * intensity;
    }
    spec *= 1.0 / max(0.0001, shininess * 0.02);
    return vec3(spec);
}

void main(){
    vec3 N = normalize(vNormal);
    // if normal map present, sample it (textures may be stubbed)
    N = getNormal();
    vec3 L = normalize(-uLightDir);
    vec3 V = normalize(uViewPos - vPosition);
    vec3 baseColor = texture(uAlbedoTex, vTexCoord).rgb * uAlbedo;
    vec3 diffuse = baseColor * max(dot(N, L), 0.0);
    vec3 spec = BlinnPhong(N, V, L, uSpecPower, uSpecIntensity);
    vec3 ambient = 0.08 * baseColor;
    vec3 color = ambient + diffuse + spec;
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}
