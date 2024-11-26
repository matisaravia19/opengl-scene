#version 460 core

#include "../shared/pbr.glsl"
#include "../shared/lightning.glsl"

layout (location = 0) out vec4 oFragColor;
layout (location = 1) out vec4 oMetadata;

layout (binding = 0) uniform sampler2D worldPosition;
layout (binding = 1) uniform sampler2D albedo;
layout (binding = 2) uniform sampler2D normal;
layout (binding = 3) uniform sampler2D metallicRoughness;

layout (binding = 8) uniform sampler2D shadowMap;

uniform vec2 windowSize;
uniform vec3 cameraPosition;
uniform vec3 lightColor;
uniform vec3 lightDirection;

uniform mat4 lightSpaceMatrix;

void main() {
    vec2 uv = gl_FragCoord.xy / windowSize;

    vec4 fWorldPosition = texture(worldPosition, uv);
    if (fWorldPosition.w == 0.0) {
        discard;
    }

    float shadow = getShadowMultiplier(lightSpaceMatrix, fWorldPosition, shadowMap);

    vec3 fAlbedo = texture(albedo, uv).rgb;
    vec3 fNormal = texture(normal, uv).xyz;
    vec4 fMetallicRoughness = texture(metallicRoughness, uv);

    vec3 fragToCamera = cameraPosition - fWorldPosition.xyz;
    vec3 viewDir = normalize(fragToCamera);

    vec3 color = pbr(fWorldPosition.xyz, fAlbedo, fNormal, fMetallicRoughness.b, fMetallicRoughness.g, -lightDirection, viewDir, lightColor);
    oFragColor = vec4(color * shadow, 1.0);
    oMetadata = vec4(0.0);
}