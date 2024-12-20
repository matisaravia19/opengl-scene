#include "util.glsl"

vec3 fresnelSchlick(vec3 halfwayDir, vec3 viewDir, vec3 albedo, float metallic) {
    vec3 f0 = mix(vec3(0.04), albedo, metallic);
    return f0 + (1.0 - f0) * pow(saturate(1.0 - positiveDot(halfwayDir, viewDir)), 5.0);
}

float distributionGGX(vec3 normal, vec3 halfwayDir, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;

    float nDotH = positiveDot(normal, halfwayDir);
    float nDotH2 = nDotH * nDotH;

    float numerator = a2;
    float denominator = (nDotH * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return numerator / denominator;
}

float geometrySchlickGGX(vec3 normal, vec3 direction, float roughness) {
    float nDotD = positiveDot(normal, direction);

    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float numerator = nDotD;
    float denominator = nDotD * (1.0 - k) + k;

    return numerator / denominator;
}

float geometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
    float ggx2 = geometrySchlickGGX(normal, viewDir, roughness);
    float ggx1 = geometrySchlickGGX(normal, lightDir, roughness);

    return ggx1 * ggx2;
}

vec3 pbr(
    vec3 worldPosition,
    vec3 albedo,
    vec3 normal,
    float metallic,
    float roughness,
    vec3 lightDir,
    vec3 viewDir,
    vec3 radiance
) {
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 f = fresnelSchlick(halfwayDir, viewDir, albedo, metallic);
    float ndf = distributionGGX(normal, halfwayDir, roughness);
    float g = geometrySmith(normal, viewDir, lightDir, roughness);

    vec3 kS = f;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 diffuse = kD * albedo / PI;

    vec3 numerator = ndf * g * f;
    float denominator = 4.0 * positiveDot(normal, viewDir) * positiveDot(normal, lightDir) + 0.0001;
    vec3 specular = numerator / denominator;

    return (diffuse + specular) * radiance * positiveDot(normal, lightDir);
}

//vec3 pbrOld(
//    vec3 worldPosition,
//    vec3 albedo,
//    vec3 normal,
//    float metallic,
//    float roughness
//) {
//    vec3 fragToCamera = cameraPosition - worldPosition;
//    vec3 viewDir = normalize(fragToCamera);
//
//    vec3 color = vec3(0.0);
//
//    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
//        if (pointLights[i].intensity == 0.0) {
//            break;
//        }
//
//        vec3 fragToLight = pointLights[i].position - worldPosition;
//        vec3 lightDir = normalize(fragToLight);
//
//        float distanceToLight = length(fragToLight);
//        float attenuation = 1.0 / (distanceToLight * distanceToLight);
//        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;
//
//        color += pbrSingleLight(worldPosition, albedo, normal, metallic, roughness, lightDir, viewDir, radiance);
//    }
//
//    for (int i = 0; i < MAX_DIR_LIGHTS; i++) {
//        if (directionalLights[i].intensity == 0.0) {
//            break;
//        }
//
//        vec3 lightDir = -normalize(directionalLights[i].direction);
//
//        vec3 radiance = directionalLights[i].color * directionalLights[i].intensity;
//
//        color += pbrSingleLight(worldPosition, albedo, normal, metallic, roughness, lightDir, viewDir, radiance);
//    }
//
//    color = color / (color + vec3(1.0));
//    color = pow(color, vec3(1.0 / 2.2));
//
//    return color;
//}