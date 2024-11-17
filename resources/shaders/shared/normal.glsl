vec3 normalMap(vec3 normal, vec3 tangent, vec3 bitangent, sampler2D normalMap, vec2 texCoords) {
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 sampledNormals = texture(normalMap, texCoords).rgb * 2.0 - 1.0;
    return normalize(TBN * sampledNormals);
}