#version 330 core
out vec2 FragColor; in vec2 vUV;
// placeholder BRDF LUT generation - real implementation should integrate microfacet geometry
void main(){ FragColor = vUV; }