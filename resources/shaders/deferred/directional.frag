#version 460 core

#include "../shared/pbr.glsl"

out vec4 oFragColor;

layout (binding = 0) uniform sampler2D worldPosition;
layout (binding = 1) uniform sampler2D albedo;
layout (binding = 2) uniform sampler2D normal;
layout (binding = 3) uniform sampler2D metallicRoughness;

uniform vec2 windowSize;
uniform vec3 cameraPosition;
uniform vec3 lightColor;
uniform vec3 lightDirection;

void main() {
    vec2 uv = gl_FragCoord.xy / windowSize;

    vec4 fWorldPosition = texture(worldPosition, uv);
    if (fWorldPosition.w == 0.0) {
        discard;
    }

    vec3 fAlbedo = texture(albedo, uv).rgb;
    vec3 fNormal = texture(normal, uv).xyz;
    vec4 fMetallicRoughness = texture(metallicRoughness, uv);

    vec3 fragToCamera = cameraPosition - fWorldPosition.xyz;
    vec3 viewDir = normalize(fragToCamera);

    oFragColor = vec4(pbr(fWorldPosition.xyz, fAlbedo, fNormal, fMetallicRoughness.b, fMetallicRoughness.g, -lightDirection, viewDir, lightColor), 1.0);
}