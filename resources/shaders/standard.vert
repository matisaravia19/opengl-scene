#version 460

#include "shared/util.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec3 vBitangent;
layout (location = 4) in vec2 vTexCoords;
layout (location = 5) in ivec4 vBoneIDs;
layout (location = 6) in vec4 vWeights;

out vec3 fWorldPosition;
out vec3 fNormal;
out vec3 fTangent;
out vec3 fBitangent;
out vec2 fTexCoords;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 bones[MAX_BONES];

void main() {
    vec4 position = vec4(0.0);
    vec4 normal = vec4(0.0);
    vec4 tangent = vec4(0.0);
    vec4 bitangent = vec4(0.0);

    if (vBoneIDs[0] == -1) {
        position = modelMatrix * vec4(vPosition, 1.0);
        normal = vec4(vNormal, 0.0);
        tangent = vec4(vTangent, 0.0);
        bitangent = vec4(vBitangent, 0.0);
    } else {
        for (int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
            if (vBoneIDs[i] == -1) {
                break;
            }

            position += (bones[vBoneIDs[i]] * vec4(vPosition, 1.0)) * vWeights[i];
            normal += (bones[vBoneIDs[i]] * vec4(vNormal, 0.0)) * vWeights[i];
            tangent += (bones[vBoneIDs[i]] * vec4(vTangent, 0.0)) * vWeights[i];
            bitangent += (bones[vBoneIDs[i]] * vec4(vBitangent, 0.0)) * vWeights[i];
        }
    }

    fWorldPosition = vec3(position);
    fNormal = normalMatrix * vec3(normal);
    fTangent = normalMatrix * vec3(tangent);
    fBitangent = normalMatrix * vec3(bitangent);
    fTexCoords = vTexCoords;

    gl_Position = projectionMatrix * viewMatrix * position;
}