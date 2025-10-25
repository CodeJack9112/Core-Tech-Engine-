#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform float uScale; // outline scale
out vec4 vPos;

void main() {
  vec3 pos = aPosition + aNormal * uScale;
  vPos = uProj * uView * uModel * vec4(pos, 1.0);
  gl_Position = vPos;
}
