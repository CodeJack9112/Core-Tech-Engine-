#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
uniform sampler2D uAlbedo;
uniform vec3 uColor;
void main(){
    vec3 N = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5,1.0,0.3));
    float diff = max(dot(N, lightDir), 0.0);
    vec3 alb = texture(uAlbedo, TexCoords).rgb * uColor;
    vec3 col = alb * diff;
    FragColor = vec4(col,1.0);
}