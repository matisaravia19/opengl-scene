#version 460 core

out vec4 oFragColor;

layout (location = 0) uniform sampler2D color;

uniform vec2 windowSize;

void main() {
    vec2 uv = gl_FragCoord.xy / windowSize;

    vec4 hdrColor = texture(color, uv);
    if (hdrColor.w == 0.0) {
        discard;
    }

    const float exposure = 1.0;
    vec4 mapped = vec4(1.0) - exp(-hdrColor * exposure);

    const float gamma = 2.2;
    mapped = pow(mapped, vec4(1.0 / gamma));

    oFragColor = mapped;
}