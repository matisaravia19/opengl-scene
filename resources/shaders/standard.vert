#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec3 fWorldPosition;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    fWorldPosition = vec3(modelMatrix * vec4(vPosition, 1.0));
    fNormal = normalMatrix * vNormal;
    fTexCoords = vTexCoords;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
}