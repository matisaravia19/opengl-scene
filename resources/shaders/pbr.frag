#version 460 core

#include "shared/normal.glsl"
#include "shared/pbr.glsl"

in vec3 fWorldPosition;
in vec3 fNormal;
in vec3 fTangent;
in vec3 fBitangent;
in vec2 fTexCoords;

out vec4 oFragColor;

layout (binding = 0) uniform sampler2D albedo;
layout (binding = 1) uniform sampler2D normal;
layout (binding = 2) uniform sampler2D metallicRoughness;
layout (binding = 3) uniform sampler2D ao;

void main() {
    vec3 sampledAlbedo = texture(albedo, fTexCoords).rgb;
    vec3 sampledNormal = normalMap(fNormal, fTangent, fBitangent, normal, fTexCoords);
    vec4 sampledMetallicRoughness = texture(metallicRoughness, fTexCoords);

    vec3 color = pbr(fWorldPosition, sampledAlbedo, sampledNormal, sampledMetallicRoughness.b, sampledMetallicRoughness.g);

    oFragColor = vec4(color, 1.0);
}