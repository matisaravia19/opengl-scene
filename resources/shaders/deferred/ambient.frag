#version 460 core

#include "../shared/pbr.glsl"
#include "../shared/lightning.glsl"

layout (location = 0) out vec4 oFragColor;
layout (location = 1) out vec4 oMetadata;

layout (binding = 0) uniform sampler2D worldPosition;
layout (binding = 1) uniform sampler2D albedo;

uniform vec2 windowSize;
uniform vec3 ambientColor;

void main() {
    vec2 uv = gl_FragCoord.xy / windowSize;

    vec4 fWorldPosition = texture(worldPosition, uv);
    if (fWorldPosition.w == 0.0) {
        discard;
    }

    vec3 fAlbedo = texture(albedo, uv).rgb;

    oFragColor = vec4(fAlbedo * ambientColor, 1.0);
    oMetadata = vec4(0.0);
}