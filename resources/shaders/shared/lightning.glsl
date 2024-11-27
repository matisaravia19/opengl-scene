float getShadowMultiplier(mat4 lightSpaceMatrix, vec4 worldPosition, sampler2D shadowMap) {
    vec4 shadowCoords = vec4(lightSpaceMatrix * worldPosition);
    shadowCoords = (shadowCoords / shadowCoords.w) * 0.5 + 0.5;
    if (shadowCoords.z > 1.0) {
        return 1.0;
    }

    float shadow = 0.0;
    vec2 texelSize = vec2(1.0 / textureSize(shadowMap, 0));
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap, shadowCoords.xy + vec2(x, y) * texelSize).r;
            shadow += shadowCoords.z < pcfDepth ? 1.0 : 0.0;
        }
    }

    if (shadow == 0.0) {
        discard;
    }

    return shadow / 25.0;
}