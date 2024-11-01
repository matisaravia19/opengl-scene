#version 460 core
layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iTexCoords;

out vec2 oTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    oTexCoords = iTexCoords;
    gl_Position = projection * view * model * vec4(iPosition, 1.0);
}