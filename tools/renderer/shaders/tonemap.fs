#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D uScene;
void main(){
    vec3 c = texture(uScene, TexCoords).rgb;
    // simple Reinhard tonemap
    c = c / (c + vec3(1.0));
    FragColor = vec4(c,1.0);
}