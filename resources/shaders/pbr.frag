#version 460 core

#include "shared/normal.glsl"

in vec3 fWorldPosition;
in vec3 fNormal;
in vec3 fTangent;
in vec3 fBitangent;
in vec2 fTexCoords;

layout (location = 0) out vec4 oWorldPosition;
layout (location = 1) out vec4 oAlbedo;
layout (location = 2) out vec4 oNormal;
layout (location = 3) out vec4 oMetallicRoughness;

layout (binding = 0) uniform sampler2D albedo;
layout (binding = 1) uniform sampler2D normal;
layout (binding = 2) uniform sampler2D metallicRoughness;
layout (binding = 3) uniform sampler2D ao;

void main() {
    oWorldPosition = vec4(fWorldPosition, 1.0);
    oAlbedo = texture(albedo, fTexCoords);
    oNormal = vec4(normalMap(fNormal, fTangent, fBitangent, normal, fTexCoords), 1.0);
    oMetallicRoughness = texture(metallicRoughness, fTexCoords);
}