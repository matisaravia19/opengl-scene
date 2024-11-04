#version 460 core
in vec2 fTexCoords;
in vec3 fNormal;

out vec4 oFragColor;

layout (binding = 0) uniform sampler2D albedo;

void main() {
    oFragColor = texture(albedo, fTexCoords);
}