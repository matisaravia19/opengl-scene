#version 460 core
in vec2 fTexCoords;
in vec3 fNormal;

out vec4 oFragColor;

void main() {
    oFragColor = vec4(abs(fNormal), 1.0);
}