#version 460 core

out vec4 oFragColor;

layout (binding = 0) uniform sampler2D image;
layout (binding = 1) uniform sampler2D metadata;
layout (binding = 2) uniform sampler2D worldPosition;
layout (binding = 3) uniform sampler2D depth;

uniform vec2 windowSize;
uniform vec4 fogColor;
uniform float fogDensity;

vec4 distanceFog(vec4 color, float distance) {
    float fogFactor = exp((distance - 1) / fogDensity);
    return mix(color, fogColor, fogFactor);
}

vec4 hdr(vec4 color) {
    const float exposure = 1.0;
    vec4 mapped = vec4(1.0) - exp(-color * exposure);

    const float gamma = 2.2;
    return pow(mapped, vec4(1.0 / gamma));
}

void main() {
    vec2 uv = gl_FragCoord.xy / windowSize;

    vec4 color = texture(image, uv);
    if (color.w == 0.0) {
        discard;
    }

    vec4 fragMetadata = texture(metadata, uv);
    if (fragMetadata.r == 0.0) {
        color = distanceFog(color, texture(depth, uv).r);
    }

    //color = hdr(color);

    oFragColor = color;
}