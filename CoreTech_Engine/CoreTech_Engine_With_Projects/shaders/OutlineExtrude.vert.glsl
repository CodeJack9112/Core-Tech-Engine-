#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uOutlineScale; // e.g., 0.03

void main() {
    // Extrude along normal in model space, scaled by outline thickness
    vec3 pos = aPosition + aNormal * uOutlineScale;
    gl_Position = uProjection * uView * uModel * vec4(pos, 1.0);
}
